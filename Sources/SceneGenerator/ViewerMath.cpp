#include "stdafx.h"

#define PI 3.14159265f

float degToRad( float a )
{
	return ( a * PI / 180.0f );
}

void M44Translate( __in_ecount( 3 ) const float* tV, __inout_ecount( 16 ) float* M )
{
	ELEMENT( M, 0 ) += ELEMENT( tV, 0 ) * ELEMENT( M, 12 );
	ELEMENT( M, 1 ) += ELEMENT( tV, 0 ) * ELEMENT( M, 13 );
	ELEMENT( M, 2 ) += ELEMENT( tV, 0 ) * ELEMENT( M, 14 );
	ELEMENT( M, 3 ) += ELEMENT( tV, 0 ) * ELEMENT( M, 15 );

	ELEMENT( M, 4 ) += ELEMENT( tV, 1 ) * ELEMENT( M, 12 );
	ELEMENT( M, 5 ) += ELEMENT( tV, 1 ) * ELEMENT( M, 13 );
	ELEMENT( M, 6 ) += ELEMENT( tV, 1 ) * ELEMENT( M, 14 );
	ELEMENT( M, 7 ) += ELEMENT( tV, 1 ) * ELEMENT( M, 15 );

	ELEMENT( M, 8 ) += ELEMENT( tV, 2 ) * ELEMENT( M, 12 );
	ELEMENT( M, 9 ) += ELEMENT( tV, 2 ) * ELEMENT( M, 13 );
	ELEMENT( M, 10 ) += ELEMENT( tV, 2 ) * ELEMENT( M, 14 );
	ELEMENT( M, 11 ) += ELEMENT( tV, 2 ) * ELEMENT( M, 15 );
}

void M44Rotate( __in const float angle, __in_ecount( 3 ) const float* rV, __inout_ecount( 16 ) float* M )
{
	float c = cos( degToRad( angle ) );
	float s = sin( degToRad( angle ) );
	float t = 1.0f - c;

	float nrV[3];
	V3Normalize( rV, nrV );

	const float nX = nrV[0];
	const float nY = nrV[1];
	const float nZ = nrV[2];

	float rotMatrix[16] = {
		t*nX*nX + c, t*nX*nY - s*nZ, t*nX*nZ + s*nY, 0,
		t*nX*nY + s*nZ, t*nY*nY + c, t*nY*nZ - s*nX, 0,
		t*nX*nZ - s*nY, t*nY*nZ + s*nX, t*nZ*nZ + c, 0,
		0, 0, 0, 1 };

	float tmpMatrix[16];
	ShallowCopy( M, 16, tmpMatrix );

	M44MulM44( rotMatrix, tmpMatrix, M );
}

void M44Scale( __in_ecount( 3 ) const float* sV, __inout_ecount( 16 ) float* M )
{
	V4MulC( M, ELEMENT( sV, 0 ), M );
	V4MulC( M + 4, ELEMENT( sV, 1 ), M + 4 );
	V4MulC( M + 8, ELEMENT( sV, 2 ), M + 8 );
}

void M44SetPerspective(
	__in const float angle_of_view,
	__in const float aspect_ratio,
	__in const float z_near,
	__in const float z_far,
	__out_ecount( 16 ) float* M )
{
	float angle = degToRad( angle_of_view );

	ELEMENT( M, 0 ) = 1.0f / ( tan( angle )*aspect_ratio );	ELEMENT( M, 1 ) = 0.0f;					ELEMENT( M, 2 ) = 0.0f;										 ELEMENT( M, 3 ) = 0.0f;
	ELEMENT( M, 4 ) = 0.0f;									ELEMENT( M, 5 ) = 1.0f / tan( angle );  ELEMENT( M, 6 ) = 0.0f;										 ELEMENT( M, 7 ) = 0.0f;
	ELEMENT( M, 8 ) = 0.0f;									ELEMENT( M, 9 ) = 0.0f;					ELEMENT( M, 10 ) = -( z_far + z_near ) / ( z_far - z_near ); ELEMENT( M, 11 ) = -1.0f;
	ELEMENT( M, 12 ) = 0.0f;								ELEMENT( M, 13 ) = 0.0f;				ELEMENT( M, 14 ) = -2.0f*z_far*z_near / ( z_far - z_near );	 ELEMENT( M, 15 ) = 0.0f;
}

void M44SetPerspectiveClippingPlanes( __in const float zNear, __in const float zFar, __out_ecount( 16 ) float* M )
{
	ELEMENT( M, 10 ) = -( zFar + zNear ) / ( zFar - zNear );
	ELEMENT( M, 14 ) = -2.0f*zFar*zNear / ( zFar - zNear );
}
