#pragma once

#define MAX_KERNEL_SIZE 128

class AmbientOcclusionKernel
{
public:

	void GetKernel( __inout uint& size, __deref_out Vector3f** ppKernelPtr, __inout GLuint noiseTexture );
	
	static const uint NOISE_SCALE = 4;

protected:

	void GenerateKernel( __in const uint size );

	void SetRandomVector( __inout Vector3f* pVector );

	inline float GetRandom( __in const float min, __in const float max );

	float Lerp( __in const float a, __in const float b, __in const float inVal );

	void GetNoiseTexture( __inout GLuint noiseTexture );

protected:

	Vector3f mRandVectors[MAX_KERNEL_SIZE];
	Vector3f mNoiseVectors[NOISE_SCALE * NOISE_SCALE];

};

