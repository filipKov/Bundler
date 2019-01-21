#include "stdafx.h"

#include "GL\glew.h"
#include "GL\glut.h"

#include "AlgebraLib.h"

#include "RenderableObject.h"

using namespace Containers;

RenderablePointCloud::RenderablePointCloud() {
	mPointCount = 0;
	mVertexBufferId = 0;
	mColorBufferId = 0;
}

RenderablePointCloud::RenderablePointCloud( __in const uint pointCount, __in const Bundler::Point* pPoints ) {
	mPointCount = pointCount;

	Buffer< float > tempVertexBuffer;
	tempVertexBuffer.Allocate( mPointCount * 3 );

	Buffer< uint > tempColorBuffer;
	tempColorBuffer.Allocate( mPointCount );

	auto pSrc = pPoints;
	float* pPosDst = tempVertexBuffer.Data();
	uint* pColDst = tempColorBuffer.Data();
	for ( uint i = 0; i < mPointCount; i++ ) {
		( *pPosDst ) = (float)pSrc->position[0];
		( *( pPosDst +1) ) = (float)pSrc->position[1];
		( *( pPosDst +2) ) = (float)pSrc->position[2];
		pPosDst += 3;

		*pColDst = pSrc->color;
		pColDst++;

		pSrc++;
	}

	glGenBuffers( 1, &mVertexBufferId );
	glBindBuffer( GL_ARRAY_BUFFER, mVertexBufferId );
	glBufferData( GL_ARRAY_BUFFER, mPointCount * sizeof( float ), tempVertexBuffer.Data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &mColorBufferId );
	glBindBuffer( GL_ARRAY_BUFFER, mColorBufferId );
	glBufferData( GL_ARRAY_BUFFER, mPointCount * sizeof( uint ), tempColorBuffer.Data(), GL_STATIC_DRAW );
}

void RenderablePointCloud::Render( __in const GLuint shader ) const {
	int vertexColorPresentLoc = glGetUniformLocation( shader, "hasVertexColors" );
	if ( vertexColorPresentLoc > -1 ) {
		glUniform1i( vertexColorPresentLoc, true );
	}

	glBindBuffer( GL_ARRAY_BUFFER, mVertexBufferId );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, mColorBufferId );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 1, GL_UNSIGNED_INT, GL_FALSE, 0, NULL );

	glDrawArrays( GL_POINTS, 0, mPointCount );
}


// RenderableWireframe::RenderableWireframe( __in const BundleCamera& camera ) {
// 	UNREFERENCED_PARAMETER( camera );
// }
// 
// RenderableWireframe::RenderableWireframe( __in const BundlePointTrack& track ) {
// 	UNREFERENCED_PARAMETER( track );
// }

void RenderableWireframe::SetGrid( __in const Vector3f& startPos, __in const Vector3f& endPos, __in const uint segments ) {
	Vector3f startX = startPos;
	Vector3f startY = startPos;

	Vector3f endX = startPos;
	endX[2] = endPos[2];
	Vector3f endY = startPos;
	endY[0] = endPos[0];

	Vector3f delta = ( endPos - startPos ) * ( 1.0f / segments );

	uint indexCounter = 0;
	Buffer< float > vertexBuffer;
	vertexBuffer.Allocate( 3 * 4 * ( segments + 1 ) );

	indexBufferLength = 4 * ( segments + 1 );
	Buffer< uint > indexBuffer;
	indexBuffer.Allocate( indexBufferLength );

	float* pPositions = vertexBuffer.Data();
	uint* pIndices = indexBuffer.Data();

	for ( uint i = 0; i < segments+1; i++ ) {
		// horizontal
		ShallowCopy( startX.Elements(), 3, pPositions );
		pPositions += 3;
		ShallowCopy( endX.Elements(), 3, pPositions );
		pPositions += 3;

		*pIndices = indexCounter;
		*( pIndices + 1 ) = indexCounter + 1;
		pIndices += 2;
		indexCounter += 2;

		startX[0] += delta[0];
		endX[0] += delta[0];

		// vertical
		ShallowCopy( startY.Elements(), 3, pPositions );
		pPositions += 3;
		ShallowCopy( endY.Elements(), 3, pPositions );
		pPositions += 3;

		*pIndices = indexCounter;
		*( pIndices + 1 ) = indexCounter + 1;
		pIndices += 2;
		indexCounter += 2;

		startY[2] += delta[2];
		endY[2] += delta[2];
	}

	glGenBuffers( 1, &mVertexBufferId );
	glBindBuffer( GL_ARRAY_BUFFER, mVertexBufferId );
	glBufferData( GL_ARRAY_BUFFER, vertexBuffer.Length() * sizeof( float ), vertexBuffer.Data(), GL_STATIC_DRAW );

	glGenBuffers( 1, &mIndexBufferId );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexBuffer.Length() * sizeof( uint ), indexBuffer.Data(), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &mVaoId );
}

void RenderableWireframe::Render( __in const GLuint shader ) const {
	int vertexColorPresentLoc = glGetUniformLocation( shader, "hasVertexColors" );
	if ( vertexColorPresentLoc > -1 ) {
		glUniform1i( vertexColorPresentLoc, false );
	}

	glBindVertexArray( mVaoId );
	glBindBuffer( GL_ARRAY_BUFFER, mVertexBufferId );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId );
	
	glDrawElements( GL_LINES, indexBufferLength, GL_UNSIGNED_INT, NULL );
}