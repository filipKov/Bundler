#pragma once

struct RendererComponents {
	RendererComponents() {
		pAlbedoShader = nullptr;
		pShadowComponent = nullptr;
		pSkyboxShader = nullptr;
		pAmbientOcclusionShader = nullptr;
		bShadowsEnabled = false;
		bAoEnabled = false;
	}

	void Release() {
		if ( pAlbedoShader ) delete pAlbedoShader;
		if ( pShadowComponent ) delete pShadowComponent;
		if ( pSkyboxShader ) delete pSkyboxShader;
		if ( pAmbientOcclusionShader ) delete pAmbientOcclusionShader;
		if ( pPointCloudShader ) delete pPointCloudShader;
	}

	bool bShadowsEnabled;
	ShaderProgram* pShadowComponent;

	bool bAoEnabled;
	ShaderProgram* pAmbientOcclusionShader;

	ShaderProgram* pAlbedoShader;
	ShaderProgram* pSkyboxShader;
	ShaderProgram* pPointCloudShader;
};

class GlRenderer {
public:

	void Initialize();

	void SetViewport( __in const Viewport& renderingWindow );

	void SetComponents( __in const RendererComponents& components );

	
	size_t RegisterLight( __in Light* pLight );


	size_t RegisterRenderable( __in IRenderable* pObject );

	void UnregisterRenderable( __in const size_t ix );

	void ClearRenderables( );

	void RegisterSkybox( __in IRenderable* pSkybox );


	// void Render( __in const Scene& scene ) const;
	void Render( __in const Camera& camera ) const;

	void SetGrid( __in Grid3D* pGrid );

	void Release();

	void SwitchSSAOState();

	void SwitchShadowsState();

protected:

	void RenderGrid( __in const Camera& camera ) const;

	void ShowTexture( __in const GLuint textureId ) const;

protected:

	RendererComponents mComponents;
	
	Grid3D* m_pGrid;

	IRenderable* m_pSkybox;
	Utils::Containers::Vector< IRenderable* > mRenderables;
	Utils::Containers::Vector< Light* > mLights;
};

