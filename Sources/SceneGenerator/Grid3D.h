#pragma once


struct Grid3D {

	Grid3D() { bEnabled = false; };
	Grid3D( __in const uint size );

	void Release();

	bool bEnabled;
	uint size;
	
	GlMesh gpuObject;
	uint lineIndexCount;
	GLuint mShaderProgram;

	void SetShaderUniforms( __in const Matrix4x4f& viewMatrix, __in const Matrix4x4f& projectionMatrix ) const;

	void SwapEnabled();

protected:

	void LoadShader();
	void GenerateVertices();

	void WriteVertex( __in const float x, __in const float y, __in const float z, __out float* pDst );

};

