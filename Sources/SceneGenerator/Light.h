#pragma once

enum class LIGHT_TYPE {
	DIRECTIONAL,
	POINT,
	SPOT,
	UNKNOWN
};

class Light {

public:

	Light() {
		mType = LIGHT_TYPE::UNKNOWN;
		mCastsShadows = false;
	}

	const LIGHT_TYPE GetType() const;

	const Vector3f& GetColor() const;

	void SetColor( __in const Vector3f& color );

	bool CastsShadows() const;

protected:

	LIGHT_TYPE mType;
	Vector3f mRgbColor;
	bool mCastsShadows;
};

