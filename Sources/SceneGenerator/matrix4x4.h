#pragma once

class Matrix4x4 : public MatrixNxN< 4 > {

public:

	using MatrixNxN< 4 >::MatrixNxN;

	void SetIdentity();
	
	void Translate( __in const float dx, __in const float dy, __in const float dz );

	void Scale( __in const float x, __in const float y, __in const float z );

	void Rotate( __in const float angle, __in const float x, __in const float y, __in const float z );

	void SetPerspective( 
		__in const float angle_of_view, 
		__in const float aspect_ratio,
		__in const float z_near, 
		__in const float z_far );
	
	void SetOrtho( 
		__in const float left,
		__in const float right,
		__in const float top,
		__in const float bottom, 
		__in const float near,
		__in const float far );

};