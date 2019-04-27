#include "stdafx.h"
#include "BundleNormalization.h"

namespace Bundler { namespace Preprocess {

	template < typename CameraFnc, typename PointFnc >
	void IterateThroughBundle( __inout Bundle* pBundle, __in CameraFnc camFnc, __in PointFnc ptFnc )
	{
		const size_t pointCount = pBundle->points.Length( );
		Vector3* pPoint = pBundle->points.Data( );
		for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
		{
			ptFnc( pPoint );
			pPoint++;
		}

		const size_t cameraCount = pBundle->cameras.Length( );
		Camera* pCamera = pBundle->cameras.Data( );
		for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
		{
			camFnc( pCamera );
			pCamera++;
		}
	}

	template < typename CameraFnc, typename PointFnc >
	void IterateThroughBundle( __in const Bundle* pBundle, __in CameraFnc camFnc, __in PointFnc ptFnc )
	{
		const size_t pointCount = pBundle->points.Length( );
		const Vector3* pPoint = pBundle->points.Data( );
		for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
		{
			ptFnc( pPoint );
			pPoint++;
		}

		const size_t cameraCount = pBundle->cameras.Length( );
		const Camera* pCamera = pBundle->cameras.Data( );
		for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
		{
			camFnc( pCamera );
			pCamera++;
		}
	}

	void GetCameraCenter( __in const Camera* pCamera, __out_ecount( 3 ) Scalar* pCenter )
	{
		Scalar ri[9];
		M33Transpose( pCamera->r.Elements( ), ri );
		M33MulC( ri, -1, ri );

		M33MulV3( ri, pCamera->t.Elements( ), pCenter );
	}

	void GetMean( __in const Bundle* pBundle, __out_ecount( 3 ) double* pMean )
	{
		V3Zero( pMean );

		IterateThroughBundle(
			pBundle,
			[ &pMean ] ( __in const Camera* pCamera ) { 
				Scalar camCenter[3];
				GetCameraCenter( pCamera, camCenter );
				V3AddV3( pMean, camCenter, pMean ); 
			},
			[ &pMean ] ( __in const Vector3* pPoint ) { V3AddV3( pMean, pPoint->Elements( ), pMean ); } 
		);

		size_t elementCount = pBundle->points.Length( ) + pBundle->cameras.Length( );

		V3MulC( pMean, 1.0 / elementCount, pMean );
	}

	namespace Internal {
		
		// Assuming that mean = 0
		void GetStdev( __in const Bundle* pBundle, __out double* pStdev )
		{
			double variance = 0;

			IterateThroughBundle(
				pBundle,
				[ &variance ] ( __in const Camera* pCamera ) { variance += V3Dot( pCamera->t.Elements( ), pCamera->t.Elements( ) ); },
				[ &variance] ( __in const Vector3* pPoint ) { variance += V3Dot( pPoint->Elements( ), pPoint->Elements( ) ); } 
			);

			size_t elementCount = pBundle->points.Length( ) + pBundle->cameras.Length( );

			*pStdev = sqrt( variance / elementCount );
		}

		void GetStdev( __in const Bundle* pBundle, __in_ecount_opt( 3 ) double* pMean, __out double* pStdev )
		{
			double variance = 0;

			IterateThroughBundle(
				pBundle,
				[ &variance, &pMean ] ( __in const Camera* pCamera ) { variance += V3DistanceSq( pCamera->t.Elements( ), pMean ); },
				[ &variance, &pMean ] ( __in const Vector3* pPoint ) { variance += V3DistanceSq( pPoint->Elements( ), pMean ); }
			);

			size_t elementCount = pBundle->points.Length( ) + pBundle->cameras.Length( );

			*pStdev = sqrt( variance / elementCount );
		}
	}

	void GetStdev( __in const Bundle* pBundle, __in_ecount_opt( 3 ) double* pMean, __out double* pStdev )
	{
		if ( !pMean )
		{
			Internal::GetStdev( pBundle, pStdev );
		}
		else
		{
			Internal::GetStdev( pBundle, pMean, pStdev );
		}
	}

	void ShiftBundle( __in_ecount( 3 ) const double* pMean, __inout Bundle* pBundle )
	{
		Scalar shiftFactor[3];
		V3Cast( pMean, shiftFactor );

		IterateThroughBundle(
			pBundle,
			[ &shiftFactor ] ( __inout Camera* pCamera ) { 
				Scalar camCenter[3];
				GetCameraCenter( pCamera, camCenter );
				V3SubV3( camCenter, shiftFactor, camCenter );

				M33MulV3( pCamera->r.Elements( ), camCenter, pCamera->t.Elements( ) );
				V3MulC( pCamera->t.Elements( ), -1, pCamera->t.Elements( ) );
			},
			[ &shiftFactor ] ( __inout Vector3* pPoint ) { V3SubV3( pPoint->Elements( ), shiftFactor, pPoint->Elements( ) ); }
		);
	}

	void ScaleBundle( __in const double stdev, __inout Bundle* pBundle )
	{
		Scalar scaleFactor = Scalar( 0.5 / stdev );

		IterateThroughBundle(
			pBundle,
			[ scaleFactor ] ( __inout Camera* pCamera ) { V3MulC( pCamera->t.Elements( ), scaleFactor, pCamera->t.Elements( ) ); },
			[ scaleFactor ] ( __inout Vector3* pPoint ) { V3MulC( pPoint->Elements( ), scaleFactor, pPoint->Elements( ) ); }
		);
	}

	void CopyToNewBundle(
		__in const Bundle* pBundle,
		__out Bundle* pBundleOut )
	{
		pBundleOut->cameras.SetCopy( pBundle->cameras );
		pBundleOut->points.SetCopy( pBundle->points );
		pBundleOut->projections.SetCopy( pBundle->projections );
	}

	void Normalize(
		__in const Bundle* pBundle,
		__out Bundle* pBundleOut,
		__out_ecount_opt( 3 ) Scalar* pMean,
		__out_opt Scalar* pStdev )
	{
		double mean[3];
		GetMean( pBundle, mean );
		
		double stdev = 0;
		GetStdev( pBundle, mean, &stdev );

		if ( pBundleOut != pBundle )
		{
			CopyToNewBundle( pBundle, pBundleOut );
		}

		ShiftBundle( mean, pBundleOut );
		ScaleBundle( stdev, pBundleOut );

		if ( pMean != NULL )
		{
			V3Cast< double, Scalar >( mean, pMean );
		}

		if ( pStdev != NULL )
		{
			*pStdev = Scalar( stdev );
		}
	}

} }
