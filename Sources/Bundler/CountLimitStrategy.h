#pragma once

namespace Bundler {

	template < class CameraModel >
	class CountLimitStrategy
	{
	protected:
	
		static const uint INVALID_INDEX = 0xFFFFFFFF;

	public:

		static void InitializeForCameras(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraStartIx,
			__in const double fracCount,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			const uint cameraCount = pProvider->GetCameraCount( );
			const uint count = (uint)ceil( cameraCount * fracCount );
			InitializeForCameras( pProvider, cameraStartIx, count, pLocalProvider );
		}

		static void InitializeForCameras(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraStartIx,
			__in const uint count,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			Containers::Buffer< uint > pointIdices;
			pointIdices.Allocate( pProvider->GetPointCount( ) );
			pointIdices.Fill( INVALID_INDEX );
			bool* pPointIndices = pointIdices.Data( );

			uint projectionToCopyCount = 0;
			uint pointToCopyCount = 0;
			for ( uint cameraIx = cameraStartIx; cameraIx < cameraStartIx + count; cameraIx++ )
			{
				const uint projectionCount = ( uint )pProvider->GetCameraProjectionCount( cameraIx );
				projectionToCopyCount += projectionCount;

				for ( uint projI = 0; projI < projectionCount; projI++ )
				{
					const uint projectionIx = pProvider->GetCameraProjectionIndex( cameraIx, projI );
					const Projection* pProj = pProvider->m_pProjections + projectionIx;

					if ( ELEMENT( pPointIndices, pProj->pointIndex ) == INVALID_INDEX )
					{
						ELEMENT( pPointIndices, pProj->pointIndex ) = pointToCopyCount;
						pointToCopyCount++;
					}
				}
			}

			pLocalProvider->m_cameras.Allocate( count );
			pLocalProvider->m_cameraModels.Allocate( count );
			pLocalProvider->m_cameraGlobalMapping.Allocate( count );
			pLocalProvider->m_mapping.Allocate( count );

			pLocalProvider->m_points.Allocate( pointToCopyCount );
			pLocalProvider->m_pointGlobalMapping.Allocate( pointToCopyCount );

			pLocalProvider->m_projections.Allocate( projectionToCopyCount );
			
			for ( uint i = 0; i < count; i++ )
			{
				const uint srcCameraIx = cameraStartIx + i;
				CopyCamera( srcCameraIx, i, pProvider, pLocalProvider );


				const uint projectionCount = ( uint )pProvider->GetCameraProjectionCount( srcCameraIx );
				

				for ( uint projI = 0; projI < projectionCount; projI++ )
				{
					const uint projectionIx = pProvider->GetCameraProjectionIndex( cameraIx, projI );
					const Projection* pProj = pProvider->m_pProjections + projectionIx;

					if ( ELEMENT( pPointIndices, pProj->pointIndex ) == INVALID_INDEX )
					{
						ELEMENT( pPointIndices, pProj->pointIndex ) = pointToCopyCount;
						pointToCopyCount++;
					}
				}
			}
		}

		static void InitializeForPoints(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint pointStartIx,
			__in const double fracCount,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			const uint pointCount = pProvider->GetPointCount( );
			const uint count = ( uint )ceil( pointCount * fracCount );
			InitializeForPoints( pProvider, pointStartIx, count, pLocalProvider );
		}

		static void InitializeForPoints(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint pointStartIx,
			__in const uint count,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			
		}

	protected:

		static void CopyCamera( 
			__in const uint srcCameraIx, 
			__in const uint i,
			__in const ProjectionProvider< CameraModel >* pProvider,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{

		}

	};

}