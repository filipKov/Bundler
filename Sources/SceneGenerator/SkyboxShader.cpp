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
#include "SkyboxShader.h"

void SkyboxShader::SetCamera( __in const Camera* pCamera ) {
	m_pCamera = pCamera;
	Matrix4x4f projection = m_pCamera->GetProjectionMatrix();
	M44SetPerspectiveClippingPlanes( 0.1f, 10000.0f, projection.Elements() );
	LoadUniform( mGlProgram, UNIFORM_VS_PROJECTION_MATRIX, glUniformMatrix4fv, 1, (GLboolean)GL_FALSE, projection.Elements() );
}

void SkyboxShader::Render( __in const uint objectCnt, __in_ecount( objectCnt ) IRenderable* const* ppObjects ) {
	Vector4f lightPos = Vector4f( { 0,0,0,1 } );
	lightPos[1] -= 90.0f;
	PointLight centerLight( lightPos, 0.0f );
	Light* const pLight = &centerLight;
	SetupLights( 1, &pLight );

	glDisable( GL_DEPTH_TEST );
	AlbedoShader::Render( objectCnt, ppObjects );
	glEnable( GL_DEPTH_TEST );
}
