#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"

#include "Camera.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "Shader.h"

ShaderProgram::ShaderProgram( __in const SHADER_TYPE type, __in const GLuint shaderProgram, __in const Viewport& viewport ) {
	mGlProgram = shaderProgram;
	mType = type;
	SetViewport( viewport );
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram( mGlProgram );
	mType = SHADER_TYPE::UNKNOWN;
}

void ShaderProgram::SetActive() const {
	glUseProgram( mGlProgram );
	glViewport( mViewport.x, mViewport.y, mViewport.width, mViewport.height );
}

void ShaderProgram::SetViewport( __in const Viewport& viewport ) {
	mViewport = viewport;
}

void ShaderProgram::SetCamera( __in const Camera* pCamera ) {
	m_pCamera = pCamera;
	LoadUniform( mGlProgram, UNIFORM_VS_PROJECTION_MATRIX, glUniformMatrix4fv, 1, (GLboolean)GL_FALSE, pCamera->GetProjectionMatrix().Elements() );
}

void ShaderProgram::Render( __in const uint objectCnt, __in_ecount( objectCnt ) IRenderable* const* ppObjects ) {
	for ( uint objectI = 0; objectI < objectCnt; objectI++ ) {
		( *ppObjects )->Render( this );
		ppObjects++;
	}
}

void ShaderProgram::SetupLights( __in const uint lightCnt, __in_ecount( lightCnt ) Light* const* ppLights ) {
	const uint clampedLightCnt = min( lightCnt, GL_MAX_LIGHTS );
	uint dirLightI = 0;
	uint pointLightI = 0;

	for ( uint lightI = 0; lightI < lightCnt; lightI++ ) {
		switch ( ( *ppLights )->GetType() ) {
		case LIGHT_TYPE::DIRECTIONAL:
			if ( dirLightI < clampedLightCnt ) {
				SetupDirectionalLight( dirLightI, (DirectionalLight*)( *ppLights ) );
				dirLightI++;
			}
			break;
		case LIGHT_TYPE::POINT:
			if ( pointLightI < clampedLightCnt ) {
				SetupPointLight( pointLightI, (PointLight*)( *ppLights ) );
				pointLightI++;
			}
			break;
		default:
			continue;
		}
		ppLights++;
	}

	LoadUniform( mGlProgram, UNIFORM_FS_DIR_LIGHT_CNT, glUniform1ui, dirLightI );
	LoadUniform( mGlProgram, UNIFORM_FS_POINT_LIGHT_CNT, glUniform1ui, pointLightI );
}


Shader ShaderProgram::GetShader() const {
	return Shader{ mType, mGlProgram };
}

const Matrix4x4f& ShaderProgram::GetViewMatrix() const {
	return m_pCamera->GetViewMatrix();
}

const Matrix4x4f& ShaderProgram::GetProjectionMatrix() const {
	return m_pCamera->GetProjectionMatrix();
}

void ShaderProgram::SetupDirectionalLight( __in const uint lightI, __in const DirectionalLight* pLight ) {
	// Left empty intentionally
	UNREFERENCED_PARAMETER( lightI );
	UNREFERENCED_PARAMETER( pLight );
}

void ShaderProgram::SetupPointLight( __in const uint lightI, __in const PointLight* pLight ) {
	// Left empty intentionally
	UNREFERENCED_PARAMETER( lightI );
	UNREFERENCED_PARAMETER( pLight );
}
