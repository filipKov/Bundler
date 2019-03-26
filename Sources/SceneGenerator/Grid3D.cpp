#include "stdafx.h"

#include "glslang.h"
#include "RenderingHelpers.h"

#include "IRenderingContext.h"
#include "IRenderable.h"
#include "RenderableCartoonMesh.h"

#include "Grid3D.h"

using namespace Containers;

Grid3D::Grid3D( __in const uint size ) {
	this->size = size;
	bEnabled = false;
	LoadShader();
	GenerateVertices();
}

void Grid3D::Release() {
	gpuObject.Release();

	if ( mShaderProgram != 0 ) {
		glDeleteProgram( mShaderProgram );
	}

	ByteFill( 0, 1, this );
}

void Grid3D::SetShaderUniforms( __in const Matrix4x4f& viewMatrix, __in const Matrix4x4f& projectionMatrix ) const {
	LoadUniform( mShaderProgram, UNIFORM_VS_PROJECTION_MATRIX, glUniformMatrix4fv, 1, (GLboolean)GL_FALSE, projectionMatrix.Elements() );
	LoadUniform( mShaderProgram, UNIFORM_VS_VIEW_MATRIX, glUniformMatrix4fv, 1, (GLboolean)GL_TRUE, viewMatrix.Elements() );
}

void Grid3D::SwapEnabled() {
	bEnabled = !bEnabled;
}

void Grid3D::LoadShader() {
	mShaderProgram = createShaderProgram( GET_SHADER_PATH( "lineSceneWidget.vert" ), GET_SHADER_PATH( "lineSceneWidget.frag" ) );
}

void Grid3D::GenerateVertices() {
	uint gridSideVertices = size + 1;
	Buffer< float > vertexBuffer;
	vertexBuffer.Allocate( 3 * 4 * gridSideVertices );
	Buffer< uint > indexBuffer;
	indexBuffer.Allocate( 4 * gridSideVertices );
	lineIndexCount = (uint)indexBuffer.Length();

	float gridStart = -( (float)size / 2 );
	float gridEnd = -gridStart;

	float* pPositions = vertexBuffer.Data();
	uint* pIndices = indexBuffer.Data();
	uint indexCounter = 0;

	for ( float gridLineI = gridStart; gridLineI <= gridEnd; gridLineI++ ) {
		// vertical
		WriteVertex( gridLineI, 0, gridStart, pPositions );
		pPositions += 3;
		WriteVertex( gridLineI, 0, gridEnd, pPositions );
		pPositions += 3;

		*pIndices = indexCounter;
		*( pIndices + 1 ) = indexCounter + 1;
		pIndices += 2;
		indexCounter += 2;

		// horizontal
		WriteVertex( gridStart, 0, gridLineI, pPositions );
		pPositions += 3;
		WriteVertex( gridEnd, 0, gridLineI, pPositions );
		pPositions += 3;

		*pIndices = indexCounter;
		*( pIndices + 1 ) = indexCounter + 1;
		pIndices += 2;
		indexCounter += 2;
	}

	glGenBuffers( 1, &gpuObject.vertexBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, gpuObject.vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, vertexBuffer.Length() * sizeof( float ), vertexBuffer.Data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &gpuObject.polygonIndexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gpuObject.polygonIndexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexBuffer.Length() * sizeof( uint ), indexBuffer.Data(), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &gpuObject.arrayObject );
	glBindVertexArray( gpuObject.arrayObject );
	
	glBindBuffer( GL_ARRAY_BUFFER, gpuObject.vertexBuffer );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gpuObject.polygonIndexBuffer );
}

void Grid3D::WriteVertex( __in const float x, __in const float y, __in const float z, __out float* pDst ) {
	*pDst = x;
	*( pDst + 1 ) = y;
	*( pDst + 2 ) = z;
}