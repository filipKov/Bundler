#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"
#include "RenderableCartoonMesh.h"

#include "Transformable.h"
#include "SceneObject.h"

#include "Texture.h"
#include "SceneMeshPart.h"

#include "SceneMesh.h"

size_t SceneMesh::GetPartCount() const {
	return mParts.Length();
}

const SceneMeshPart* SceneMesh::GetPartPtr( __in const uint partIndex ) const {
	return &mParts[ partIndex ];
}

IRenderable* SceneMesh::GetRenderable() {
	return this;
}

void SceneMesh::SetPartsCount( __in const uint count ) {
	mParts.EnsureLength( count );
	
	for ( uint i = 0; i < count; i++ ) {
		mParts[ i ].SetParent( this );
	}
}

SceneMeshPart* SceneMesh::GetPartPtr( __in const uint partIndex ) {
	return &mParts[ partIndex ];
}

void SceneMesh::Render( __in const IRenderingContext* pContext ) const {
	for ( size_t i = 0; i < mParts.Length(); i++ ) {
		mParts[ i ].Render( pContext );
	}
}

template < typename Fnc >
Vector3f ApplyPerElement( __in const Vector3f a, __in const Vector3f b ) {
	return Vector3f( { Fnc( a[ 0 ], b[ 0 ] ), Fnc( a[ 1 ], b[ 1 ] ), Fnc( a[ 2 ], b[ 2 ] ) } );
}

float fltMin( const float a, const float b ) { return min( a, b ); };
float fltMax( const float a, const float b ) { return max( a, b ); };

BoundingBox SceneMesh::GetBoundingBox() const {
	BoundingBox bbox;

	for ( size_t i = 0; i < mParts.Length(); i++ ) {
		const BoundingBox& partBox = mParts[ i ].GetBoundingBox();

		bbox.minVert = Vector3f( { min( bbox.minVert[ 0 ], partBox.minVert[ 0 ] ), min( bbox.minVert[ 1 ], partBox.minVert[ 1 ] ), min( bbox.minVert[ 2 ], partBox.minVert[ 2 ] ) } );
		bbox.maxVert = Vector3f( { max( bbox.maxVert[ 0 ], partBox.maxVert[ 0 ] ), max( bbox.maxVert[ 1 ], partBox.maxVert[ 1 ] ), max( bbox.maxVert[ 2 ], partBox.maxVert[ 2 ] ) } );
	}

	return bbox;
}