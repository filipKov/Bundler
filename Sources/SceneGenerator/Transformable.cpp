#include "stdafx.h"
#include "Transformable.h"

using namespace LinearAlgebra;

Transformable::Transformable() : Transformable( Vector4f( { 0,0,0,1 } ) ) {}

Transformable::Transformable( __in const Vector4f& pos ) {
	mPivot = pos;
	mTransformation.SetIdentity();
}

Transformable::Transformable( __in const Vector4f& pos, __in const Matrix4x4f& transform ) {
	mPivot = pos;
	mTransformation = transform;
}

const Vector4f& Transformable::GetPivot() const {
	return mPivot;
}

const Matrix4x4f& Transformable::GetTransform() const {
	return mTransformation;
}

void Transformable::SetPivot( __in_ecount( 4 ) const float* pPos ) {
	ShallowCopy( pPos, 4, mPivot.Elements() );
}

void Transformable::SetTransform( __in_ecount( 16 ) const float* pMatrix ) {
	ShallowCopy( pMatrix, 16, mTransformation.Elements() );
}

// Manipulation
void Transformable::Move( __in const Vector3f& translation ) {
	float dx = translation[ 0 ];
	float dy = translation[ 1 ];
	float dz = translation[ 2 ];

	mPivot += Vector4f( { dx, dy, dz, 0 } );
	M44Translate( translation.Elements( ), mTransformation.Elements( ) );
}

void Transformable::MoveTo( __in const Vector3f& position ) {
	Vector3f translation( { position[ 0 ] - mPivot[ 0 ], position[ 1 ] - mPivot[ 1 ], position[ 2 ] - mPivot[ 2 ] } );
	Move( translation );
}

void Transformable::Rotate( __in const float angle, __in const Vector3f& rotVector ) {
	float rotTransform[16];
	M44Identity( rotTransform );
	M44Translate( ( -mPivot ).Elements( ), rotTransform );
	M44Rotate( angle, rotVector.Elements(), rotTransform );
	M44Translate( mPivot.Elements( ), rotTransform );

	float transformTmp[16];
	ShallowCopy( mTransformation.Elements( ), 16, transformTmp );

	M44MulM44( rotTransform, transformTmp, mTransformation.Elements( ) );
}

void Transformable::Scale( __in const Vector3f& scale ) {
	float sclTrnasform[16];
	M44Identity( sclTrnasform );
	M44Translate( ( -mPivot ).Elements( ), sclTrnasform );
	M44Scale( scale.Elements( ), sclTrnasform );
	M44Translate( mPivot.Elements( ), sclTrnasform );

	float transformTmp[16];
	ShallowCopy( mTransformation.Elements( ), 16, transformTmp );

	M44MulM44( sclTrnasform, transformTmp, mTransformation.Elements( ) );
}

void Transformable::Scale( __in const float uniformFactor ) {
	float scale[3] = { uniformFactor, uniformFactor, uniformFactor };
	
	float sclTrnasform[16];
	M44Identity( sclTrnasform );
	M44Translate( ( -mPivot ).Elements( ), sclTrnasform );
	M44Scale( scale, sclTrnasform );
	M44Translate( mPivot.Elements( ), sclTrnasform );

	float transformTmp[16];
	ShallowCopy( mTransformation.Elements( ), 16, transformTmp );

	M44MulM44( sclTrnasform, transformTmp, mTransformation.Elements( ) );
}