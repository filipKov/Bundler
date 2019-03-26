#pragma once

class DirectionalLight : public Light {
public:
	DirectionalLight();
	DirectionalLight( __in const Vector4f& dir );
	DirectionalLight( __in const Vector4f& dir, __in const Vector3f& color );

	const Vector4f& GetDirection() const;

	void SetDirection( __in const Vector4f& dir );

protected:

	Vector4f mDirection;

};

