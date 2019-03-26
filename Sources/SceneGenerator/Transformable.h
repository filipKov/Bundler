#pragma once

class Transformable {
public:

	Transformable();

	Transformable( __in const Vector4f& pos );

	Transformable( __in const Vector4f& pos, __in const Matrix4x4f& transform );

	const Vector4f& GetPivot() const;

	const Matrix4x4f& GetTransform() const;

	void SetPivot( __in_ecount( 4 ) const float* pPos );

	void SetTransform( __in_ecount( 16 ) const float* pMatrix );

	// Manipulation
	void Move( __in const Vector3f& translation );

	void MoveTo( __in const Vector3f& position );

	void Rotate( __in const float angle, __in const Vector3f& rotVector );

	void Scale( __in const Vector3f& scale );

	void Scale( __in const float uniformFactor );

protected:

	Vector4f mPivot;

	Matrix4x4f mTransformation;

};