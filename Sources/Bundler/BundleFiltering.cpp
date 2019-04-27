#include "stdafx.h"
#include "BundleStructureMapping.h"
#include "BundleFiltering.h"

#ifdef _DEBUG
	#define FILTER_SAFE 1
#endif

namespace Bundler { namespace Preprocess {

	void GetOutliers(
		__in const Bundle* pBundle,
		__in_ecount( 3 ) const Scalar* pCenter,
		__in const Scalar radius,
		__out Containers::PagedVector< size_t >* pOutlierIndices )
	{
		const Vector3* pPoint = pBundle->points.Data( );
		for ( size_t pointIx = 0; pointIx < pBundle->points.Length( ); pointIx++ )
		{
			Scalar pointDistance = sqrt( V3DistanceSq( pPoint->Elements( ), pCenter ) );
			if ( pointDistance > ( radius ) )
			{
				pOutlierIndices->Add( pointIx );
			}
			pPoint++;
		}
	}

	void RemoveOutliers(
		__in const Bundle* pBundle,
		__in const Containers::PagedVector< size_t >* pOutliersIndices,
		__out Bundle* pOutBundle )
	{
		Bundler::Structure::BundleStructureMapping structure;
		structure.Initialize(
			pBundle->cameras.Length( ),
			pBundle->points.Length( ),
			pBundle->projections.Length( ),
			pBundle->projections.Data( ) );

		const size_t originalPointCount = pBundle->points.Length( );
		const size_t removedPoints = pOutliersIndices->Length( );
		
		const size_t pointsLeft = originalPointCount - removedPoints;
		size_t projectionsLeft = pBundle->projections.Length( );
		for ( size_t i = 0; i < removedPoints; i++ )
		{
			projectionsLeft -= structure.GetPointProjectionCount( ( *pOutliersIndices )[i] );
		}

		pOutBundle->cameras.SetCopy( pBundle->cameras );
		pOutBundle->points.Allocate( pointsLeft );
		pOutBundle->projections.Allocate( projectionsLeft );
		
		size_t removedIx = 0;
		size_t pointDstIx = 0;
		size_t projDstIx = 0;

		for ( size_t sourceIx = 0; sourceIx < originalPointCount; sourceIx++ )
		{
			if ( ( removedIx < removedPoints ) && ( ( *pOutliersIndices )[removedIx] == sourceIx ) )
			{
				removedIx++;
			}
			else
			{
				pOutBundle->points[pointDstIx] = pBundle->points[sourceIx];
				
				const size_t projCount = structure.GetPointProjectionCount( sourceIx );
				for ( size_t i = 0; i < projCount; i++ )
				{
					const size_t srcProjIx = structure.GetPointProjectionIndex( sourceIx, i );
					Projection proj = pBundle->projections[srcProjIx];
					proj.pointIndex = (uint)pointDstIx;

					pOutBundle->projections[projDstIx] = proj;
					projDstIx++;
				}

				pointDstIx++;
			}
		}
	}

	void CheckFilteredBundle( __in const Bundle* pBundle )
	{
		const size_t cameraCount = pBundle->cameras.Length( );
		const size_t pointCount = pBundle->points.Length( );
		const size_t projectionCount = pBundle->projections.Length( );

		Bundler::Structure::BundleStructureMapping structure;
		structure.Initialize( cameraCount, pointCount, projectionCount, pBundle->projections.Data( ) );

		for ( uint camIx = 0; camIx < cameraCount; camIx++ )
		{
			_ASSERT_EXPR( structure.GetCameraProjectionCount( camIx ) > 0, "Warning: Empty camera left in the system after filtering" );
		}

		for ( uint projIx = 0; projIx < projectionCount; projIx++ )
		{
			_ASSERT_EXPR( pBundle->projections[projIx].pointIndex < pointCount, "Error: Projection has wrong point index!" );
		}
	}

	void FilterAroundCenter(
		__in const Bundle* pBundle,
		__in_ecount( 3 ) const Scalar* pCenter,
		__in const Scalar radius,
		__out Bundle* pOutBundle,
		__out_opt FilteringStatistics* pStats )
	{
		_ASSERT_EXPR( pBundle != pOutBundle, "Cannot filter bundle in-place" );

		Containers::PagedVector< size_t > outlierIndices;
		GetOutliers( pBundle, pCenter, radius, &outlierIndices );

		RemoveOutliers( pBundle, &outlierIndices, pOutBundle );

		#if FILTER_SAFE
			CheckFilteredBundle( pOutBundle );
		#endif

		if ( pStats )
		{
			pStats->pointsBefore = pBundle->points.Length( );
			pStats->pointsAfter = pOutBundle->points.Length( );
			pStats->projectionsBefore = pBundle->projections.Length( );
			pStats->projectionsAfter = pOutBundle->projections.Length( );
		}
	}

} }
