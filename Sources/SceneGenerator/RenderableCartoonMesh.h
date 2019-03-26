#pragma once

struct GlMesh {
	GLuint polygonIndexBuffer;
	GLuint vertexBuffer;
	GLuint vertexNormalBuffer;
	GLuint textureCoordinateBuffer;
	GLuint vertexColorBuffer;

	GLuint arrayObject;

	GlMesh();

	void Release();
};

struct MeshInfo {
	uint vertexCount;
	uint faceCount;
	bool hasNormals;
	bool hasTextureCoordinates;
	bool hasVertexColors;
};

class RenderableCartoonMesh : public IRenderable
{
public:
	RenderableCartoonMesh();
	~RenderableCartoonMesh();

	void SetMesh( __in const GlMesh& mesh, __in const MeshInfo& info, __in const BoundingBox& bbox );

	void Release();

	void Render( __in const IRenderingContext* pContext ) const override;

	const BoundingBox& GetBoundingBox() const;

protected:

	void RenderColor( __in const IRenderingContext* pContext ) const;

	void RenderAmbientOcclusion( __in const IRenderingContext* pContext ) const;

	void SetTransformMatrices( __in const IRenderingContext* pContext ) const;

	virtual Matrix<float, 4, 4> GetWorldTransform() const = 0;

	void SetTexture( __in const GLuint shaderId ) const;

	bool IsTextured() const;

	virtual const GLuint GetGlTexture() const = 0;

	void RenderWireframe( __in const uint faceCount ) const;

	void RenderSolid( __in const uint faceCount ) const;

	static void BindVertexArrayObject( __in const GlMesh& pc );

	static void UnbindVertexArrayObject();

protected:

	GlMesh mMesh;

	MeshInfo mInfo;

	BoundingBox mBBox;

	static const uint RENDER_PASS_OUTLINE = 0;
	static const uint RENDER_PASS_COLOR = 1;

};

