#include "stdafx.h"
#include "SceneGenerator.h"

using namespace Bundler;

namespace SceneGenerator {

	void SceneGen::AddNoise(
		__in const SceneGenNoiseSettings* pNoise,
		__in const Bundler::Bundle* pGroundTruth,
		__out Bundler::Bundle* pNoisedScene )
	{
		CopyGroundTruth( pGroundTruth, pNoisedScene );

		const size_t pointCount = pNoisedScene->points.Length( );
		for ( size_t ptIx = 0; ptIx < pointCount; ptIx++ )
		{
			AddNoise( &pNoise->pointSettings, POINT_PARAM_COUNT, pNoisedScene->points[ptIx].Elements() );
		}

		const size_t cameraCount = pNoisedScene->cameras.Length( );
		for ( size_t camIx = 0; camIx < cameraCount; camIx++ )
		{
			Camera* pCamera = &pNoisedScene->cameras[camIx];

			AddNoise( &pNoise->cameraSettings.translationNoise, 3, pCamera->t.Elements( ) );
			AddNoise( &pNoise->cameraSettings.rotationNoise, 9, pCamera->r.Elements( ) );
			Clamp( Scalar( -1 ), Scalar( 1 ), 9, pCamera->r.Elements( ) );
			
			pCamera->focalLength += Random< Scalar >::Value(
				pNoise->cameraSettings.focalLengthNoise.minDelta,
				pNoise->cameraSettings.focalLengthNoise.maxDelta );
		}
	}

	void SceneGen::CopyGroundTruth(
		__in const Bundler::Bundle* pGroundTruth,
		__out Bundler::Bundle* pNoisedScene )
	{
		pNoisedScene->cameras.SetCopy( pGroundTruth->cameras );
		pNoisedScene->points.SetCopy( pGroundTruth->points );
		pNoisedScene->projections.SetCopy( pGroundTruth->projections );
	}

	void SceneGen::AddNoise(
		__in const SceneGenRange* pSettings,
		__in const uint n,
		__inout_ecount( n ) Bundler::Scalar* pValues )
	{
		for ( uint i = 0; i < 9; i++ )
		{
			ELEMENT( pValues, i ) += Random< Scalar >::Value( pSettings->minDelta, pSettings->maxDelta );
		}
	}

	template < typename T >
	T Clamp( __in const T value, __in const T minVal, __in const T maxVal )
	{
		return ( value > maxVal ) ? maxVal : ( value < minVal ) ? minVal : value;
	}

	void SceneGen::Clamp(
		__in const Bundler::Scalar minVal,
		__in const Bundler::Scalar maxVal,
		__in const uint n,
		__inout_ecount( n ) Bundler::Scalar* pValues )
	{
		for ( uint i = 0; i < 9; i++ )
		{
			ELEMENT( pValues, i ) = SceneGenerator::Clamp< Scalar >( ELEMENT( pValues, i ), Scalar( -1 ), Scalar( 1 ) );
		}
	}

}

