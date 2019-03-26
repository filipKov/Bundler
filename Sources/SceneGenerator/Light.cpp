#include "stdafx.h"

#include "Light.h"


const LIGHT_TYPE Light::GetType() const {
	return mType;
}

const Vector3f& Light::GetColor() const {
	return mRgbColor;
}

void Light::SetColor( __in const Vector3f& color ) {
	mRgbColor = color;
}

bool Light::CastsShadows() const {
	return mCastsShadows;
}