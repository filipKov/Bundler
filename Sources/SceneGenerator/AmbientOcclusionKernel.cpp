#include "stdafx.h"

#include "AmbientOcclusionKernel.h"

void AmbientOcclusionKernel::GetKernel( __inout uint& size, __deref_out Vector3f** ppKernelPtr, __inout GLuint noiseTexture ) {
	size = min( size, MAX_KERNEL_SIZE );
	GenerateKernel( size );
	GetNoiseTexture( noiseTexture );
	*ppKernelPtr = mRandVectors;
}

void AmbientOcclusionKernel::GenerateKernel( __in const uint size ) {
	for ( uint i = 0; i < size; i++ ) {
		Vector3f* pSample = mRandVectors + i;
		
		SetRandomVector( pSample );
		pSample->Normalize();
		float scale = (float)i / size;
		scale = Lerp( 0.1f, 1.0f, scale * scale );
		(*pSample) *= scale;
	}

	for ( uint i = 0; i < NOISE_SCALE * NOISE_SCALE; i++ ) {
		Vector3f* pNoise = mNoiseVectors + i;
		SetRandomVector( pNoise );
		(*pNoise)[2] = 0.0f;
	}
}

void AmbientOcclusionKernel::SetRandomVector( __inout Vector3f* pVector ) {
	( *pVector )[0] = GetRandom( -1.0f, 1.0f );
	( *pVector )[1] = GetRandom( -1.0f, 1.0f );
	( *pVector )[2] = GetRandom( 0.0f, 1.0f );
}

inline float AmbientOcclusionKernel::GetRandom( __in const float min, __in const float max ) {
	return ( (float)rand() / RAND_MAX ) * ( max - min ) + min ;
}

float AmbientOcclusionKernel::Lerp( __in const float a, __in const float b, __in const float inVal ) {
	return a * inVal + inVal * ( b - a );
}

void AmbientOcclusionKernel::GetNoiseTexture( __inout GLuint noiseTexture ) {
	glBindTexture( GL_TEXTURE_2D, noiseTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, NOISE_SCALE, NOISE_SCALE, 0, GL_RGB, GL_FLOAT, mNoiseVectors );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
}