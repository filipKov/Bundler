#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"

#include "Camera.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "Shader.h"
#include "AlbedoShader.h"

#define TEXTURE_UNIT_DIFFUSE 0
#define TEXTURE_UNIT_SHADOW 2
#define TEXTURE_UNIT_AO 3

#define TEXTURE_DIFFUSE GL_TEXTURE0 + TEXTURE_UNIT_DIFFUSE
#define TEXTURE_SHADOW GL_TEXTURE0 + TEXTURE_UNIT_SHADOW
#define TEXTURE_AO GL_TEXTURE0 + TEXTURE_UNIT_AO

AlbedoShader::AlbedoShader( __in const GLuint shaderProgram, __in const Viewport& viewport ) : 
	ShaderProgram( SHADER_TYPE::ALBEDO, shaderProgram, viewport ) 
{
	m_pCamera = nullptr;
}

void AlbedoShader::SetupShadowMap( __in const bool shadowsEnabled, __in const GLuint mapId ) {
	LoadUniform( mGlProgram, UNIFORM_FS_SHADOWS_ENABLED, glUniform1i, shadowsEnabled );
	if ( shadowsEnabled ) {
		LoadUniform( mGlProgram, UNIFORM_FS_SHADOW_MAP, glUniform1i, TEXTURE_UNIT_SHADOW );
		glActiveTexture( TEXTURE_SHADOW );
		glBindTexture( GL_TEXTURE_2D, mapId );
	}
}

void AlbedoShader::SetupAmbientOcclusionMap( __in const bool ssaoEnabled, __in const GLuint mapId ) {
	LoadUniform( mGlProgram, UNIFORM_FS_AO_ENABLED, glUniform1i, ssaoEnabled );
	if ( ssaoEnabled ) {
		LoadUniform( mGlProgram, UNIFORM_FS_AO_MAP, glUniform1i, TEXTURE_UNIT_AO );
		glActiveTexture( TEXTURE_AO );
		glBindTexture( GL_TEXTURE_2D, mapId );
	}
}

void AlbedoShader::SetupDirectionalLight( __in const uint lightI, __in const DirectionalLight* pLight ) {
	Vector4f lightDir = m_pCamera->GetViewMatrix() * pLight->GetDirection();

	LoadUniform( mGlProgram, UNIFORM_FS_DIR_LIGHT_DIR( mUniformNameBuffer, lightI ), glUniform3fv, 1, lightDir.Elements() );
	LoadUniform( mGlProgram, UNIFORM_FS_DIR_LIGHT_COLOR( mUniformNameBuffer, lightI ), glUniform3fv, 1, pLight->GetColor().Elements() );
}

void AlbedoShader::SetupPointLight( __in const uint lightI, __in const PointLight* pLight ) {
	Vector4f lightPos = m_pCamera->GetViewMatrix() * pLight->GetPosition();

	LoadUniform( mGlProgram, UNIFORM_FS_POINT_LIGHT_POS( mUniformNameBuffer, lightI ), glUniform3fv, 1, lightPos.Elements() );
	LoadUniform( mGlProgram, UNIFORM_FS_POINT_LIGHT_COLOR( mUniformNameBuffer, lightI ), glUniform3fv, 1, pLight->GetColor().Elements() );
	LoadUniform( mGlProgram, UNIFORM_FS_POINT_LIGHT_ATTENUATION( mUniformNameBuffer, lightI ), glUniform1f, pLight->GetAttenuation() );
}
