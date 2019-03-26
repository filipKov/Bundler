#pragma once

class SceneMeshPart : public Transformable, public RenderableCartoonMesh
{
public:

	SceneMeshPart();
	~SceneMeshPart();

	void SetTexture( __in const Texture* pTex );

	void SetParent( __in const Transformable* pParent );

	// RenderableCartoonMesh
	Matrix<float, 4, 4> GetWorldTransform() const override;

	const GLuint GetGlTexture() const override;

protected:

	const Texture* m_pTexture;

	const Transformable* m_pParent;

};

