#pragma once

class RenderableObject {

public:

	virtual void Render( __in const GLuint shader ) const = 0;

};

class RenderablePointCloud : public RenderableObject {

public:

	RenderablePointCloud();
	RenderablePointCloud( 
		__in const uint pointCount, 
		__in_ecount( pointCount ) const Bundler::Vector3* pPoints, 
		__in_ecount_opt( pointCount ) const uint* pVertexColors );

	void Render( __in const GLuint shader ) const override;

protected:

	uint mPointCount;

	GLuint mColorBufferId;
	GLuint mVertexBufferId;
	
};

class RenderableWireframe : public RenderableObject {

public:

	// RenderableWireframe(  __in const BundleCamera& camera );
	// RenderableWireframe( __in const BundlePointTrack& track );
	
	void SetGrid( __in const Vector3f& startPos, __in const Vector3f& endPos, __in const uint segments );

	void Render( __in const GLuint shader ) const override;

protected:

	uint indexBufferLength;

	GLuint mIndexBufferId;
	GLuint mVertexBufferId;

	GLuint mVaoId;

};