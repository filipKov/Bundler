#pragma once

class GlRenderer
{
public:

	void Init();

	void Render() const;

	void SetViewportCamera( __in Camera& camera );

	void RegisterObject( __in_z const char* pId, __in RenderableObject* pObject );

	void ClearObjects();

protected:

	void InitializeShader() const;

protected:

	Camera* m_pCamera;
	std::map< std::string, RenderableObject* > m_objects;

	GLuint m_vertexColorDiffuseShader;

};

