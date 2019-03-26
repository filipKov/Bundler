#pragma once

class PointLight : public Light {
public:

	PointLight();
	PointLight( __in const Vector4f& position );
	PointLight( __in const Vector4f& position, __in const Vector3f& color );
	PointLight( __in const Vector4f& position, __in const float attenuation );
	PointLight( __in const Vector4f& position, __in const Vector3f& color, __in const float attenuation );

	const Vector4f& GetPosition() const;
	const float GetAttenuation() const;

	void SetPosition( __in const Vector4f& pos );
	void SetAttenuation( __in const float attenuation );

protected:

	Vector4f mPosition;
	float mAttenuation;

};

