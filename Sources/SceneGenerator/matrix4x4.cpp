#include "stdafx.h"
#include "MatrixNxN.h"
#include "matrix4x4.h"

#define PI 3.14159265f


float degToRad(float a){
	return (a * PI / 180.0f);
}

void Matrix4x4::SetIdentity() {
	ByteFill( 0, 16, m_pData );
	for ( uint i = 0; i < 4; i++ ) {
		m_pData[4 * i + i] = 1.0f;
	}
}

void Matrix4x4::Translate( __in const float dx, __in const float dy, __in const float dz ) {
	Matrix4x4 translationMatrix( {
		{1, 0, 0, dx},
		{0, 1, 0, dy},
		{0, 0, 1, dz},
		{0, 0, 0, 1} } );

	this->MultiplyLeft( translationMatrix );
}

void Matrix4x4::Scale( __in const float x, __in const float y, __in const float z ) {
	Matrix4x4 scaleMatrix( {
		{ x, 0, 0, 0 },
		{ 0, y, 0, 0 },
		{ 0, 0, z, 0 },
		{ 0, 0, 0, 1 } } );

	this->MultiplyLeft( scaleMatrix );
}

void Matrix4x4::Rotate( __in const float angle, __in const float x, __in const float y, __in const float z ) {
	//http://www.cprogramming.com/tutorial/3d/rotation.html

	float c = cos( degToRad( angle ) );
	float s = sin( degToRad( angle ) );
	float t = 1.0f - c;

	float len = sqrt( x*x + y*y + z*z );
	float nX = x / len;
	float nY = y / len;
	float nZ = z / len;

	Matrix4x4 rotationMatrix( {
		{ t*nX*nX + c, t*nX*nY - s*nZ, t*nX*nZ + s*nY, 0 },
		{ t*nX*nY + s*nZ, t*nY*nY + c, t*nY*nZ - s*nX, 0 },
		{ t*nX*nZ - s*nY, t*nY*nZ + s*nX, t*nZ*nZ + c, 0 },
		{ 0, 0, 0, 1 } } );

	this->MultiplyLeft( rotationMatrix );
}

void Matrix4x4::SetPerspective(
	__in const float angle_of_view,
	__in const float aspect_ratio,
	__in const float z_near,
	__in const float z_far ) 
{
	float angle = degToRad(angle_of_view);
	float *m = m_pData;

	m[0] = 1.0f / ( tan( angle )*aspect_ratio );	m[1] = 0.0f;				m[2] = 0.0f;										m[3] = 0.0f;
	m[4] = 0.0f;									m[5] = 1.0f / tan( angle ); m[6] = 0.0f;										m[7] = 0.0f;
	m[8] = 0.0f;									m[9] = 0.0f;				m[10] = -( z_far + z_near ) / ( z_far - z_near );	m[11] = -1.0f;
	m[12] = 0.0f;									m[13] = 0.0f;				m[14] = -2.0f*z_far*z_near / ( z_far - z_near );	m[15] = 0.0f;
}

void Matrix4x4::SetOrtho(
	__in const float left,
	__in const float right,
	__in const float top,
	__in const float bottom,
	__in const float z_near,
	__in const float z_far )
{
	//http://www.songho.ca/opengl/gl_projectionmatrix.html

	ByteFill( 0, 16, m_pData ) ;
	m_pData[0] = 2.0f / ( right - left );
	m_pData[5] = 2.0f / ( top - bottom );
	m_pData[10] = -2.0f / ( z_far - z_near );
	m_pData[12] = ( -right - left ) / ( right - left );
	m_pData[13] = ( -top - bottom ) / ( top - bottom );
	m_pData[14] = ( -z_far - z_near ) / ( z_far - z_near );
	m_pData[15] = 1.0f;
}
