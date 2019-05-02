#include "stdafx.h"
#include "BundlerUtils.h"
#include "SceneGenerator.h"

namespace Bundler { namespace SceneGenerator {

	void SceneGen::GetAutoNoise(
		__in const Bundle* pBundle,
		__in const uint noiseMask,
		__out SceneGenNoiseSettings* pNoise )
	{
		if ( noiseMask & SceneGenAutoNoiseMask::POINTS )
		{
			GetAutoNoisePoints( 
				( uint )pBundle->points.Length( ), 
				pBundle->points.Data( ), 
				&pNoise->pointSettings 
			);
		}

		if ( noiseMask & SceneGenAutoNoiseMask::CAMAERA_TRANSLATION )
		{
			GetAutoNoiseCameraTranslation(
				( uint )pBundle->cameras.Length( ),
				pBundle->cameras.Data( ),
				&pNoise->cameraSettings.translationNoise
			);
		}

		if ( noiseMask & SceneGenAutoNoiseMask::CAMAERA_ROTATION )
		{
			pNoise->cameraSettings.rotationNoise.minDelta = Scalar( -0.1 );
			pNoise->cameraSettings.rotationNoise.minDelta = Scalar( 0.1 );
		}

		if ( noiseMask & SceneGenAutoNoiseMask::CAMAERA_CALIBRATION )
		{
			GetAutoNoiseCameraCalibration(
				( uint )pBundle->cameras.Length( ),
				pBundle->cameras.Data( ),
				&pNoise->cameraSettings.focalLengthNoise
			);
		}
	}

	void SceneGen::AddNoise(
		__in const SceneGenNoiseSettings* pNoise,
		__in const Bundle* pGroundTruth,
		__out Bundle* pNoisedScene )
	{
		Bundler::Utils::CopyBundle( pGroundTruth, pNoisedScene );
	
		const size_t pointCount = pNoisedScene->points.Length( );
		for ( size_t ptIx = 0; ptIx < pointCount; ptIx++ )
		{
			AddNoise( &pNoise->pointSettings, POINT_PARAM_COUNT, pNoisedScene->points[ptIx].Elements( ) );
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
	
	void SceneGen::AddNoise(
		__in const SceneGenRange* pSettings,
		__in const uint n,
		__inout_ecount( n ) Scalar* pValues )
	{
		for ( uint i = 0; i < n; i++ )
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
		__in const Scalar minVal,
		__in const Scalar maxVal,
		__in const uint n,
		__inout_ecount( n ) Scalar* pValues )
	{
		for ( uint i = 0; i < n; i++ )
		{
			ELEMENT( pValues, i ) = SceneGenerator::Clamp< Scalar >( ELEMENT( pValues, i ), minVal, maxVal );
		}
	}

	template < typename ElementFnc >
	Vector3 GetByElements( __in ElementFnc fnc, __in const Vector3 v1, __in const Vector3 v2 )
	{
		return Vector3( { fnc( v1[0], v2[0] ), fnc( v1[1], v2[1] ), fnc( v1[2], v2[2] ) } );
	}

	Scalar GetMin( __in const Scalar val1, __in const Scalar val2 )
	{
		return min( val1, val2 );
	}

	Scalar GetMax( __in const Scalar val1, __in const Scalar val2 )
	{
		return max( val1, val2 );
	}

	void SceneGen::GetAutoNoisePoints(
		__in const uint count,
		__in_ecount( count ) const Vector3* pPoints,
		__out SceneGenRange* pNoise )
	{
		// Vector3 minPt( { FLT_MAX, FLT_MAX, FLT_MAX } );
		// Vector3 maxPt = -minPt;
		// for ( uint i = 0; i < count; i++ )
		// {
		// 	minPt = GetByElements( GetMin, minPt, *pPoints );
		// 	maxPt = GetByElements( GetMax, maxPt, *pPoints );
		// 	pPoints++;
		// }
		// 
		// Scalar noiseFactor = minPt.Distance( maxPt ) * Scalar( 0.05 );

		pPoints;
		count;

		Scalar noiseFactor = Scalar( 1 );

		pNoise->minDelta = -noiseFactor;
		pNoise->maxDelta = noiseFactor;
	}

	void SceneGen::GetAutoNoiseCameraTranslation(
		__in const uint count,
		__in_ecount( count ) const Camera* pCameras,
		__out SceneGenRange* pNoise )
	{
		Vector3 minPt( { FLT_MAX, FLT_MAX, FLT_MAX } );
		Vector3 maxPt = -minPt;

		for ( uint i = 0; i < count; i++ )
		{
			minPt = GetByElements( GetMin, minPt, pCameras->t );
			maxPt = GetByElements( GetMax, maxPt, pCameras->t );
			pCameras++;
		}

		Scalar noiseFactor = minPt.Distance( maxPt ) * Scalar( 0.005 );

		pNoise->minDelta = -noiseFactor;
		pNoise->maxDelta = noiseFactor;
	}

	void SceneGen::GetAutoNoiseCameraCalibration(
		__in const uint count,
		__in_ecount( count ) const Camera* pCameras,
		__out SceneGenRange* pNoise )
	{
		Scalar minF = FLT_MAX;
		Scalar maxF = -minF;

		for ( uint i = 0; i < count; i++ )
		{
			Scalar focal = pCameras->focalLength;
			if ( focal < minF )
			{
				minF = focal;
			}
			else if ( focal > maxF )
			{
				maxF = focal;
			}

			pCameras++;
		}

		Scalar noiseFactor = ( maxF - minF ) * Scalar( 0.05 );

		pNoise->minDelta = -noiseFactor;
		pNoise->maxDelta = noiseFactor;
	}

} }

