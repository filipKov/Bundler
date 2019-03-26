#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"

#include "Camera.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "RenderableCartoonMesh.h"
#include "Grid3D.h"

#include "Shader.h"
#include "AlbedoShader.h"
#include "SkyboxShader.h"
#include "AlbedoPointCloudShader.h"
#include "RenderToTextureShader.h"
#include "AmbientOcclusionKernel.h"
#include "AmbientOcclusionShader.h"

#include "GlRenderer.h"


void GlRenderer::Initialize() {
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );

	glEnable( GL_DEPTH_TEST );
}

void GlRenderer::SetComponents( __in const RendererComponents& components ) {
	mComponents = components;
}

void GlRenderer::SetViewport( __in const Viewport& renderingWindow ) {
	if ( mComponents.pAlbedoShader ) {
		mComponents.pAlbedoShader->SetViewport( renderingWindow );
	}
	if ( mComponents.pAmbientOcclusionShader ) {
		mComponents.pAmbientOcclusionShader->SetViewport( renderingWindow );
	}
	if ( mComponents.pShadowComponent ) {
		mComponents.pShadowComponent->SetViewport( renderingWindow );
	}
	if ( mComponents.pSkyboxShader ) {
		mComponents.pSkyboxShader->SetViewport( renderingWindow );
	}

	if ( mComponents.pPointCloudShader ) {
		mComponents.pPointCloudShader->SetViewport( renderingWindow );
	}
}

size_t GlRenderer::RegisterLight( __in Light* pLight ) {
	mLights.Add( pLight );
	return mLights.Length() - 1;
}

size_t GlRenderer::RegisterRenderable( __in IRenderable* pObject ) {
	mRenderables.Add( pObject );
	return mRenderables.Length() - 1;
}

void GlRenderer::RegisterSkybox( __in IRenderable* pSkybox ) {
	m_pSkybox = pSkybox;
}

void GlRenderer::Render( __in const Camera& camera ) const {
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	GLuint ambientOcclusionTexture = 0xFFFFFFFF;
	if ( mComponents.pAmbientOcclusionShader && mComponents.bAoEnabled ) {
		AmbientOcclusionShader* pAoComponent = (AmbientOcclusionShader*)mComponents.pAmbientOcclusionShader;
		pAoComponent->SetActive();
		pAoComponent->SetCamera( &camera );
		pAoComponent->Render( (uint)mRenderables.Length(), mRenderables.Data() );
		pAoComponent->GetAmbientOcclusionTexture( ambientOcclusionTexture );
	}
	
	if ( m_pSkybox && mComponents.pSkyboxShader ) {
		AlbedoShader* pSkyboxShader = (AlbedoShader*)mComponents.pSkyboxShader;
		pSkyboxShader->SetActive();
		pSkyboxShader->SetCamera( &camera );
		pSkyboxShader->Render( 1, &(m_pSkybox) );
	}
	
	if ( m_pGrid && m_pGrid->bEnabled ) {
		RenderGrid( camera );
	}

	if ( mComponents.pPointCloudShader ) {
		AlbedoPointCloudShader* pAlbedoShader = (AlbedoPointCloudShader*)mComponents.pPointCloudShader;
		pAlbedoShader->SetActive();
		pAlbedoShader->SetCamera( &camera );
		pAlbedoShader->Render( (uint)mRenderables.Length(), mRenderables.Data() );
	}

	if ( mComponents.pAlbedoShader ) {
		AlbedoShader* pAlbedoShader = (AlbedoShader*)mComponents.pAlbedoShader;
		pAlbedoShader->SetActive();
		pAlbedoShader->SetCamera( &camera );
		pAlbedoShader->SetupLights( (uint)mLights.Length(), mLights.Data() );
		pAlbedoShader->SetupAmbientOcclusionMap( mComponents.bAoEnabled, ambientOcclusionTexture );
		pAlbedoShader->Render( (uint)mRenderables.Length(), mRenderables.Data() );
	}
	
	if ( mComponents.pAmbientOcclusionShader ) {
		( (RenderToTextureShader*)mComponents.pAmbientOcclusionShader )->Clear();
	}
	
	glutSwapBuffers();
}

void GlRenderer::SetGrid( __in Grid3D* pGrid ) {
	m_pGrid = pGrid;
}

 void GlRenderer::Release() {
	mComponents.Release();
	mRenderables.Clear();
	mLights.Clear();
	m_pSkybox = nullptr;
 }

void GlRenderer::SwitchSSAOState() {
	mComponents.bAoEnabled = !mComponents.bAoEnabled;
}
void GlRenderer::SwitchShadowsState() {
	mComponents.bShadowsEnabled = !mComponents.bShadowsEnabled;
}

void GlRenderer::RenderGrid( __in const Camera& camera ) const {
	glUseProgram( m_pGrid->mShaderProgram );
	m_pGrid->SetShaderUniforms( camera.GetViewMatrix(), camera.GetProjectionMatrix() );

	glBindVertexArray( m_pGrid->gpuObject.arrayObject );

	glLineWidth( 1.0f );
	glDrawElements( GL_LINES, m_pGrid->lineIndexCount, GL_UNSIGNED_INT, NULL );
}

void GlRenderer::ShowTexture( __in const GLuint textureId ) const {
	glUseProgram( 0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, textureId );

	glBegin( GL_QUADS );
	glTexCoord2f( 0.0f, 0.0f );
	glVertex3f( -1.0f, -1.0f, 0.0f );
	glTexCoord2f( 1.0f, 0.0f );
	glVertex3f( 1.0f, -1.0f, 0.0f );
	glTexCoord2f( 1.0f, 1.0f );
	glVertex3f( 1.0f, 1.0f, 0.0f );
	glTexCoord2f( 0.0f, 1.0f );
	glVertex3f( -1.0f, 1.0f, 0.0f );
	glEnd();
}