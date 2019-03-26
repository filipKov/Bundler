#include "stdafx.h"

#include "Light.h"
#include "PointLight.h"


PointLight::PointLight() {
	mType = LIGHT_TYPE::POINT;
	mPosition = Vector4f( { 0.0f, 0.0f, 0.0f, 1.0f } );
	mRgbColor = Vector3f( { 1.0f, 1.0f, 1.0f } );
	mAttenuation = 0.0f;
}

PointLight::PointLight( __in const Vector4f& position ) {
	mType = LIGHT_TYPE::POINT;
	mPosition = position;
	mRgbColor = Vector3f( { 1.0f, 1.0f, 1.0f } );
	mAttenuation = 0.0f;
}

PointLight::PointLight( __in const Vector4f& position, __in const Vector3f& color ) {
	mType = LIGHT_TYPE::POINT;
	mPosition = position;
	mRgbColor = color;
	mAttenuation = 0.0f;
}

PointLight::PointLight( __in const Vector4f& position, __in const float attenuation ) {
	mType = LIGHT_TYPE::POINT;
	mPosition = position;
	mRgbColor = Vector3f( { 1.0f, 1.0f, 1.0f } );
	mAttenuation = attenuation;
}

PointLight::PointLight( __in const Vector4f& position, __in const Vector3f& color, __in const float attenuation ) {
	mType = LIGHT_TYPE::POINT;
	mPosition = position;
	mRgbColor = color;
	mAttenuation = attenuation;
}

const Vector4f& PointLight::GetPosition() const {
	return mPosition;
}

const float PointLight::GetAttenuation() const {
	return mAttenuation;
}

void PointLight::SetPosition( __in const Vector4f& pos ) {
	mPosition = pos;
}

void PointLight::SetAttenuation( __in const float attenuation ) {
	mAttenuation = attenuation;
}