#include "stdafx.h"

#include "Light.h"
#include "DirectionalLight.h"


DirectionalLight::DirectionalLight() {
	mType = LIGHT_TYPE::DIRECTIONAL;
	mDirection = Vector4f( { 0.0f, 1.0f, 0.0f, 1.0f } );
	mRgbColor = Vector3f( { 1.0f, 1.0f, 1.0f } );
	mCastsShadows = true;
}

DirectionalLight::DirectionalLight( __in const Vector4f& dir ) {
	mType = LIGHT_TYPE::DIRECTIONAL;
	mDirection = dir;
	mRgbColor = Vector3f( { 1.0f, 1.0f, 1.0f } );
	mCastsShadows = true;
}

DirectionalLight::DirectionalLight( __in const Vector4f& dir, __in const Vector3f& color ) {
	mType = LIGHT_TYPE::DIRECTIONAL;
	mDirection = dir;
	mRgbColor = color;
	mCastsShadows = true;
}

const Vector4f& DirectionalLight::GetDirection() const {
	return mDirection;
}

void DirectionalLight::SetDirection( __in const Vector4f& dir ) {
	mDirection = dir;
}