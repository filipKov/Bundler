#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"

#include "Camera.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "AmbientOcclusionKernel.h"

#include "Shader.h"
#include "RenderToTextureShader.h"
#include "AmbientOcclusionShader.h"


AmbientOcclusionShader::AmbientOcclusionShader( __in const GLuint shaderProgram, __in const Settings& settings ) : 
	RenderToTextureShader( SHADER_TYPE::AMBIENT_OCCLUSION, shaderProgram, settings.viewport ) 
{
	// InitializeFramebuffers();

	mKernelScale = settings.sampleCount;
	glGenTextures( 1, &mKernelNoiseTexture );
	mKernel.GetKernel( mKernelScale, &m_pKernelSamples, mKernelNoiseTexture );
	mKernelNoiseScale[0] = (float)mViewport.width / mKernel.NOISE_SCALE;
	mKernelNoiseScale[1] = (float)mViewport.height / mKernel.NOISE_SCALE;
	mKernelRadius = settings.kernelRadius;
	mBias = settings.bias;
}

void AmbientOcclusionShader::Clear() {
	mActiveFramebufferAttachmentIx = 0xFFFFFFFF;
}

void AmbientOcclusionShader::SetViewport( __in const Viewport& viewport ) {
	ShaderProgram::SetViewport( viewport );

	// for low-end hardware: better performance, slightly worse quality
	mViewport.width /= 2;
	mViewport.height /= 2;

	mActiveFramebufferAttachmentCnt = 0;
	mActiveRenderbufferCnt = 0;
	mActiveTextureCnt = 0;
	mActiveFramebufferAttachmentIx = 0;
	InitializeFramebuffers();
	mKernelNoiseScale[0] = (float)mViewport.width / mKernel.NOISE_SCALE;
	mKernelNoiseScale[1] = (float)mViewport.height / mKernel.NOISE_SCALE;
}

void AmbientOcclusionShader::GetAmbientOcclusionTexture( __out GLuint& texId ) {
	if ( mActiveFramebufferAttachmentCnt > 1 ) {
		texId = mFramebufferAttachments[mActiveFramebufferAttachmentCnt-1].colorAttachments[0];
	} else {
		texId = 0xFFFFFFFF;
	}
}

void AmbientOcclusionShader::GetPositionMap( __out GLuint& texId ) {
	if ( mActiveFramebufferAttachmentCnt > 0 ) {
		texId = mFramebufferAttachments[0].colorAttachments[0];
	} else {
		texId = 0xFFFFFFFF;
	}
}

void AmbientOcclusionShader::GetNormalMap( __out GLuint& texId ) {
	if ( mActiveFramebufferAttachmentCnt > 0 ) {
		texId = mFramebufferAttachments[0].colorAttachments[1];
	}
	else {
		texId = 0xFFFFFFFF;
	}
}

void AmbientOcclusionShader::InitializeFramebuffers() {
	TextureParameters rbParams;
	rbParams.internalFormat = GL_DEPTH_COMPONENT32;
	rbParams.width = mViewport.width;
	rbParams.height = mViewport.height;

	FramebufferAttachment fbaDepth;
	fbaDepth.colorAttachmentCnt = 2;
	fbaDepth.depthAttachmentActive = true;
	fbaDepth.depthAttachmentRenderbuffer = true;
	if ( GenerateFramebufferAttachment( fbaDepth ) ) {
		GenerateRenderbuffer( fbaDepth.depthAttachment, rbParams );

		TextureParam samplerParams[] = {
			TextureParam { GL_TEXTURE_MAG_FILTER, GL_NEAREST },
			TextureParam { GL_TEXTURE_MIN_FILTER, GL_NEAREST },
			TextureParam { GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT },
			TextureParam { GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT },
		};
		TextureParameters colorPosParams = rbParams;
		colorPosParams.paramCnt = 4;
		colorPosParams.pParams = samplerParams;
		colorPosParams.internalFormat = GL_RGBA16F;
		colorPosParams.format = GL_RGB;
		colorPosParams.type = GL_FLOAT;
		GenerateTexture( fbaDepth.colorAttachments[0], colorPosParams );
		GenerateTexture( fbaDepth.colorAttachments[1], colorPosParams );
	}


	TextureParam samplerParams[] = {
		TextureParam { GL_TEXTURE_MAG_FILTER, GL_LINEAR },
		TextureParam { GL_TEXTURE_MIN_FILTER, GL_LINEAR },
		TextureParam { GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		TextureParam { GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
	};
	TextureParameters aoMapParams = rbParams;
	aoMapParams.paramCnt = 4;
	aoMapParams.pParams = samplerParams;
	aoMapParams.internalFormat = GL_RGBA;
	aoMapParams.format = GL_RGB;
	aoMapParams.type = GL_FLOAT;

	FramebufferAttachment fbaAo;
	fbaAo.colorAttachmentCnt = 1;
	fbaAo.depthAttachment = true;
	fbaAo.depthAttachmentRenderbuffer = true;
	if ( GenerateFramebufferAttachment( fbaAo ) ) {
		GenerateRenderbuffer( fbaAo.depthAttachment, rbParams );
		GenerateTexture( fbaAo.colorAttachments[0], aoMapParams );
	}

	FramebufferAttachment fbaBlur;
	fbaBlur.colorAttachmentCnt = 1;
	fbaBlur.depthAttachment = true;
	fbaBlur.depthAttachmentRenderbuffer = true;
	if ( GenerateFramebufferAttachment( fbaBlur ) ) {
		GenerateRenderbuffer( fbaBlur.depthAttachment, rbParams );
		GenerateColorTexture( GL_RGBA, mViewport.width, mViewport.height, fbaBlur.colorAttachments[0] );
	}
}

void AmbientOcclusionShader::SetupActiveTextureProperties() {
	const uint renderPass = mActiveFramebufferAttachmentIx;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	LoadUniform( mGlProgram, UNIFORM_RENDER_PASS, glUniform1ui, renderPass );

	if ( renderPass == RENDER_PASS_DEPTH ) {

	} else if ( renderPass == RENDER_PASS_AO_MAP ) {
		FramebufferAttachment& depthFba = mFramebufferAttachments[renderPass - 1];
		
		LoadUniform( mGlProgram, UNIFORM_FS_POSITION_MAP, glUniform1i, 0 );
		glActiveTexture( GL_TEXTURE0 + 0 );
		glBindTexture( GL_TEXTURE_2D, depthFba.colorAttachments[0] );

		LoadUniform( mGlProgram, UNIFORM_FS_NORMAL_MAP, glUniform1i, 1 );
		glActiveTexture( GL_TEXTURE0 + 1 );
		glBindTexture( GL_TEXTURE_2D, depthFba.colorAttachments[1] );

		LoadUniform( mGlProgram, UNIFORM_FS_AO_KERNEL_SCALE, glUniform1ui, mKernelScale );
		LoadUniform( mGlProgram, UNIFORM_FS_AO_KERNEL_NOISE_SCALE, glUniform2fv, 1, mKernelNoiseScale.Elements() );
		LoadUniform( mGlProgram, UNIFORM_FS_AO_KERNEL_RADIUS, glUniform1f, mKernelRadius );
		LoadUniform( mGlProgram, UNIFORM_FS_AO_BIAS, glUniform1f, mBias );
		
		for ( uint i = 0; i < mKernelScale; i++ ) {
			LoadUniform( mGlProgram, UNIFORM_FS_AO_KERNEL( mUniformNameBuffer, i ), glUniform3fv, 1, m_pKernelSamples[i].Elements() );
		}

		glActiveTexture( GL_TEXTURE0 + 2 );
		glBindTexture( GL_TEXTURE_2D, mKernelNoiseTexture );
		LoadUniform( mGlProgram, UNIFORM_FS_AO_KERNEL_NOISE, glUniform1i, 2 );

	} else if ( renderPass == RENDER_PASS_BLUR ) {
		FramebufferAttachment& aoFba = mFramebufferAttachments[renderPass - 1];
		glActiveTexture( GL_TEXTURE0 + 3 );
		glBindTexture( GL_TEXTURE_2D, aoFba.colorAttachments[0] );
		LoadUniform( mGlProgram, UNIFORM_FS_AO_MAP, glUniform1i, 3 );

		float gaussian[81] = {
			0.00401f , 0.005895f, 0.007763f, 0.009157f, 0.009675f, 0.009157f, 0.007763f, 0.005895f, 0.00401f ,
			0.005895f, 0.008667f, 0.011412f, 0.013461f, 0.014223f, 0.013461f, 0.011412f, 0.008667f, 0.005895f,
			0.007763f, 0.011412f, 0.015028f, 0.017726f, 0.018729f, 0.017726f, 0.015028f, 0.011412f, 0.007763f,
			0.009157f, 0.013461f, 0.017726f, 0.020909f, 0.022092f, 0.020909f, 0.017726f, 0.013461f, 0.009157f,
			0.009675f, 0.014223f, 0.018729f, 0.022092f, 0.023342f, 0.022092f, 0.018729f, 0.014223f, 0.009675f,
			0.009157f, 0.013461f, 0.017726f, 0.020909f, 0.022092f, 0.020909f, 0.017726f, 0.013461f, 0.009157f,
			0.007763f, 0.011412f, 0.015028f, 0.017726f, 0.018729f, 0.017726f, 0.015028f, 0.011412f, 0.007763f,
			0.005895f, 0.008667f, 0.011412f, 0.013461f, 0.014223f, 0.013461f, 0.011412f, 0.008667f, 0.005895f,
			0.00401f , 0.005895f, 0.007763f, 0.009157f, 0.009675f, 0.009157f, 0.007763f, 0.005895f, 0.00401f
		
			// 0.0f	 , 0.000001f, 0.000014f, 0.000055f, 0.000088f, 0.000055f, 0.000014f, 0.000001f, 0.0f	 ,
			// 0.000001f, 0.000036f, 0.000362f, 0.001445f, 0.002289f, 0.001445f, 0.000362f, 0.000036f, 0.000001f,
			// 0.000014f, 0.000362f, 0.003672f, 0.014648f, 0.023205f, 0.014648f, 0.003672f, 0.000362f, 0.000014f,
			// 0.000055f, 0.001445f, 0.014648f, 0.058434f, 0.092566f, 0.058434f, 0.014648f, 0.001445f, 0.000055f,
			// 0.000088f, 0.002289f, 0.023205f, 0.092566f, 0.146634f, 0.092566f, 0.023205f, 0.002289f, 0.000088f,
			// 0.000055f, 0.001445f, 0.014648f, 0.058434f, 0.092566f, 0.058434f, 0.014648f, 0.001445f, 0.000055f,
			// 0.000014f, 0.000362f, 0.003672f, 0.014648f, 0.023205f, 0.014648f, 0.003672f, 0.000362f, 0.000014f,
			// 0.000001f, 0.000036f, 0.000362f, 0.001445f, 0.002289f, 0.001445f, 0.000362f, 0.000036f, 0.000001f,
			// 0.0f	 , 0.000001f, 0.000014f, 0.000055f, 0.000088f, 0.000055f, 0.000014f, 0.000001f, 0.0f	 ,
		};



		// float gaussian[25] = {
		// 0.031827f, 0.037541f, 0.039665f, 0.037541f, 0.031827f,
		// 0.037541f, 0.044281f, 0.046787f, 0.044281f, 0.037541f,
		// 0.039665f, 0.046787f, 0.049434f, 0.046787f, 0.039665f,
		// 0.037541f, 0.044281f, 0.046787f, 0.044281f, 0.037541f,
		// 0.031827f, 0.037541f, 0.039665f, 0.037541f, 0.031827f,
		// };

		LoadUniform( mGlProgram, UNIFORM_FS_AO_GAUSSIAN, glUniform1fv, 81, gaussian );
	}
}
