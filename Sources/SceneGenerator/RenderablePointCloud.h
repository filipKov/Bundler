#pragma once

struct GlPointCloud {
	GLuint vertexBuffer;
	GLuint vertexColorBuffer;

	GLuint arrayObject;

	void Release();
};

class RenderablePointCloud : public IRenderable 
{
public:
	RenderablePointCloud();
	~RenderablePointCloud();

	void SetPointCloud( __in const uint vertexCount, __in const GlPointCloud& pc, __in const BoundingBox& bbox );

	void Release();

	void Render( __in const IRenderingContext* pContext ) const override;

	const BoundingBox& GetBoundingBox() const;

protected:

	void RenderColored( __in const IRenderingContext* pContext ) const;

	void SetTransformMatrices( __in const IRenderingContext* pContext ) const;

	virtual Matrix<float, 4, 4> GetWorldTransform() const = 0;

	static void BindVertexArrayObject( __in const GlPointCloud& pc );

	static void UnbindVertexArrayObject();

protected:

	uint mVertexCount;

	GlPointCloud mPointCloud;
	
	BoundingBox mBBox;

};

