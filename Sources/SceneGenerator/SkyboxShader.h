#pragma once

class SkyboxShader : public AlbedoShader {
public:
	
	using AlbedoShader::AlbedoShader;

	void SetCamera( __in const Camera* pCamera ) override;

	void Render( __in const uint objectCnt, __in_ecount( objectCnt ) IRenderable* const* ppObjects ) override;

};

