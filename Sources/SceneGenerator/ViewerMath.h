#pragma once

using Vector3f = LinearAlgebra::Vector< float, 3 >;
using Vector4f = LinearAlgebra::Vector< float, 4 >;

using Matrix3x3f = LinearAlgebra::Matrix< float, 3, 3 >;
using Matrix3x4f = LinearAlgebra::Matrix< float, 3, 4 >;
using Matrix4x4f = LinearAlgebra::Matrix< float, 4, 4 >;

float degToRad( float a );

void M44Translate( __in_ecount( 3 ) const float* tV, __inout_ecount( 16 ) float* M );

void M44Rotate( __in const float angle, __in_ecount( 3 ) const float* rV, __inout_ecount( 16 ) float* M );

void M44Scale( __in_ecount( 3 ) const float* sV, __inout_ecount( 16 ) float* M );

void M44SetPerspective(
	__in const float angle_of_view,
	__in const float aspect_ratio,
	__in const float z_near,
	__in const float z_far,
	__out_ecount( 16 ) float* M );

void M44SetPerspectiveClippingPlanes( __in const float zNear, __in const float zFar, __out_ecount( 16 ) float* M );