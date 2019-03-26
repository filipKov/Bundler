#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"

#include "Camera.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "Shader.h"
#include "RenderToTextureShader.h"

RenderToTextureShader::RenderToTextureShader( __in const SHADER_TYPE type, __in const GLuint shaderProgram, __in const Viewport& viewport ) : 
	ShaderProgram( type, shaderProgram, viewport ) 
{
	mAllocatedTextureCnt = 0;
	mActiveTextureCnt = 0;

	mAllocatedRenderbufferCnt = 0;
	mActiveRenderbufferCnt = 0;

	mActiveFramebufferAttachmentCnt = 0;
	mActiveFramebufferAttachmentIx = 0;

	glGenFramebuffers( 1, &mFramebufferObject );
}
RenderToTextureShader::~RenderToTextureShader() {
	glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferObject );
	glDeleteFramebuffers( 1, &mFramebufferObject );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glDeleteTextures( mAllocatedTextureCnt, mTextureStorage );
	glDeleteRenderbuffers( mAllocatedRenderbufferCnt, mRenderbufferStorage );
}

void RenderToTextureShader::GetFramebufferAttachmentsPtr( __out uint& count, __deref_out FramebufferAttachment** ppAttachments ) {
	count = mActiveFramebufferAttachmentCnt;
	*ppAttachments = mFramebufferAttachments;
}

void RenderToTextureShader::Clear() {
	mActiveTextureCnt = 0;
	mActiveRenderbufferCnt = 0;
	mActiveFramebufferAttachmentCnt = 0;
	mActiveFramebufferAttachmentIx = 0xFFFFFFFF;
}

void RenderToTextureShader::Render( __in const uint objectCnt, __in_ecount( objectCnt ) IRenderable* const* ppObjects ) {
	glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferObject );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	for ( mActiveFramebufferAttachmentIx = 0; mActiveFramebufferAttachmentIx < mActiveFramebufferAttachmentCnt; mActiveFramebufferAttachmentIx++ ) {
		BindFramebufferAttachments( mFramebufferAttachments[mActiveFramebufferAttachmentIx] );
		SetupActiveTextureProperties();
		ShaderProgram::Render( objectCnt, ppObjects );
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void RenderToTextureShader::BindFramebufferAttachments( __in const FramebufferAttachment& attachment ) {
	GLuint renderAttachments[MAX_COLOR_ATTACHMENTS];
	for ( uint i = 0; i < attachment.colorAttachmentCnt; i++ ) {
		glBindTexture( GL_TEXTURE_2D, attachment.colorAttachments[i] );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachment.colorAttachments[i], 0 );
		renderAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	if ( attachment.depthAttachmentActive ) {
		if ( attachment.depthAttachmentRenderbuffer ) {
			glBindRenderbuffer( GL_RENDERBUFFER, attachment.depthAttachment );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, attachment.depthAttachment );
		} else {
			glBindTexture( GL_TEXTURE_2D, attachment.depthAttachment );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, attachment.depthAttachment, 0 );
		}
	}

	if ( attachment.stencilAttachmentActive ) {
		glBindTexture( GL_TEXTURE_2D, attachment.stencilAttachment );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachment.stencilAttachment, 0 );
	}

	if ( attachment.colorAttachmentCnt == 0 ) {
		glDrawBuffer( GL_NONE );
	} else {
		glDrawBuffers( attachment.colorAttachmentCnt, renderAttachments );
	}

	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
		throw WrongStateException( "GL_FRAMEBUFFER", "Framebuffer is not complete!" );
	}
}

bool RenderToTextureShader::GenerateFramebufferAttachment( __inout FramebufferAttachment& fba ) {
	if ( mActiveFramebufferAttachmentCnt < MAX_FRAMEBUFFER_ATTACHMENTS ) {
		fba.colorAttachmentCnt = min( fba.colorAttachmentCnt, MAX_COLOR_ATTACHMENTS );
		for ( uint i = 0; i < fba.colorAttachmentCnt; i++ ) {
			AllocateTexture( fba.colorAttachments[i] );
		}

		if ( fba.depthAttachmentActive ) {
			if ( fba.depthAttachmentRenderbuffer ) {
				AllocateRenderbuffer( fba.depthAttachment );
			} else {
				AllocateTexture( fba.depthAttachment );
			}
		}

		if ( fba.stencilAttachmentActive ) {
			AllocateTexture( fba.stencilAttachment );
		}

		mFramebufferAttachments[mActiveFramebufferAttachmentCnt] = fba;
		mActiveFramebufferAttachmentCnt++;
		return true;
	}
	return false;
}

bool RenderToTextureShader::AllocateTexture( __out GLuint& textureId ) {
	if ( mActiveTextureCnt < mAllocatedTextureCnt ) {
		textureId = mTextureStorage[mActiveTextureCnt];
		mActiveTextureCnt++;
		return true;
	}

	if ( mAllocatedTextureCnt < MAX_TOTAL_TEXTURES ) {
		glGenTextures( 1, &textureId );
		mAllocatedTextureCnt++;
		mTextureStorage[mActiveTextureCnt] = textureId;
		mActiveTextureCnt++;
		return true;
	}
	
	return false;
}

bool RenderToTextureShader::AllocateRenderbuffer( __out GLuint& rbId ) {
	if ( mActiveRenderbufferCnt < mAllocatedRenderbufferCnt ) {
		rbId = mRenderbufferStorage[mActiveRenderbufferCnt];
		mActiveRenderbufferCnt++;
		return true;
	}

	if ( mAllocatedRenderbufferCnt < MAX_TOTAL_TEXTURES ) {
		glGenRenderbuffers( 1, &rbId );
		mAllocatedRenderbufferCnt++;
		mRenderbufferStorage[mActiveRenderbufferCnt] = rbId;
		mActiveRenderbufferCnt++;
		return true;
	}

	return false;
}

void RenderToTextureShader::GenerateDepthTexture( __in const GLint internalFormat, __in const uint width, __in const uint height, __in const GLuint textureId ) {
	TextureParam params[] = {
		TextureParam{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		TextureParam{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
		TextureParam{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		TextureParam{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
		TextureParam{ GL_DEPTH_TEXTURE_MODE, GL_INTENSITY },
		TextureParam{ GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE },
		TextureParam{ GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL },
	};
	
	TextureParameters texParams;
	texParams.internalFormat = internalFormat;
	texParams.width = width;
	texParams.height = height;
	texParams.type = GL_FLOAT;
	texParams.format = GL_DEPTH_COMPONENT;
	texParams.paramCnt = 6;
	texParams.pParams = params;

	GenerateTexture( textureId, texParams );

}

void RenderToTextureShader::GenerateColorTexture( __in const GLint internalFormat, __in const uint width, __in const uint height, __in const GLuint textureId, __in const GLint type ) {
	TextureParam params[] = {
		TextureParam { GL_TEXTURE_MAG_FILTER, GL_LINEAR },
		TextureParam { GL_TEXTURE_MIN_FILTER, GL_LINEAR },
		TextureParam { GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		TextureParam { GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
	};
	
	TextureParameters texParams;
	texParams.internalFormat = internalFormat;
	texParams.width = width;
	texParams.height = height;
	texParams.type = type;
	texParams.format = GL_BGRA;
	texParams.paramCnt = 4;
	texParams.pParams = params;

	GenerateTexture( textureId, texParams );
}

void RenderToTextureShader::GenerateTexture( __in const GLuint textureId, __in const TextureParameters& texParams ) {
	glBindTexture( GL_TEXTURE_2D, textureId );
	glTexImage2D( GL_TEXTURE_2D, 0, texParams.internalFormat, texParams.width, texParams.height, 0, texParams.format, texParams.type, NULL );

	for ( uint i = 0; i < texParams.paramCnt; i++ ) {
		glTexParameteri( GL_TEXTURE_2D, texParams.pParams[i].parameterName, texParams.pParams[i].parameterValue );
	}
}

void RenderToTextureShader::GenerateRenderbuffer( __in const GLuint rbId, __in const TextureParameters& params ) {
	glBindRenderbuffer( GL_RENDERBUFFER, rbId );
	glRenderbufferStorage( GL_RENDERBUFFER, params.internalFormat, params.width, params.height );
}
