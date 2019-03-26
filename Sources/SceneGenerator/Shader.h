#pragma once

class ShaderProgram : public IRenderingContext {
public: 

	~ShaderProgram();

	void SetActive() const;

	virtual void SetViewport( __in const Viewport& viewport );

	virtual void SetCamera( __in const Camera* pCamera );

	virtual void Render( __in const uint objectCnt, __in_ecount( objectCnt ) IRenderable* const* ppObjects );

	void SetupLights( __in const uint lightCnt, __in_ecount( lightCnt ) Light* const* ppLights );

	// IRenderingContext
	Shader GetShader() const override;

	const Matrix4x4f& GetViewMatrix() const override;

	const Matrix4x4f& GetProjectionMatrix() const override;

protected:

	ShaderProgram( __in const SHADER_TYPE type,  __in const GLuint shaderProgram, __in const Viewport& viewport );
	ShaderProgram( const ShaderProgram& src ) = default;
	ShaderProgram( ShaderProgram&& src ) = default;

	virtual void SetupDirectionalLight( __in const uint lightI, __in const DirectionalLight* pLight );

	virtual void SetupPointLight( __in const uint lightI, __in const PointLight* pLight );

protected:

	GLuint mGlProgram;
	SHADER_TYPE mType;

	char mUniformNameBuffer[ 256 ];

	Viewport mViewport;

	const Camera* m_pCamera;

};
