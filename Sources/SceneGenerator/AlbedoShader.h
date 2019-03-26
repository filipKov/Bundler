#pragma once

class AlbedoShader : public ShaderProgram {
public:

	AlbedoShader( __in const GLuint shaderProgram, __in const Viewport& viewport );

	void SetupShadowMap( __in const bool shadowsEnabled, __in const GLuint mapId );
	void SetupAmbientOcclusionMap( __in const bool ssaoEnabled, __in const GLuint mapId );

protected:

	void SetupDirectionalLight( __in const uint lightI, __in const DirectionalLight* pLight ) override;
	void SetupPointLight( __in const uint lightI, __in const PointLight* pLight ) override;

};

