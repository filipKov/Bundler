#pragma once

namespace Bundler {

	namespace Structure {
		struct LocalBundleStructureMapping
		{
			uint startIndex;
			uint projectionCount;
		};
	}

	struct LocalProjectionProviderDataCountsAMP
	{
		uint cameraCount;
		uint pointCount;
		uint projectionCount;
	};


	namespace Internal {
		template < class CameraModel >
		struct LocalProjectionProviderDataTemp
		{
			Containers::Buffer< CameraModel > cameras;
			Containers::Buffer< Vector3 > points;
			Containers::Buffer< Projection > projections;

			Containers::Buffer< uint > camerasGlobalMapping;
			Containers::Buffer< uint > pointGlobalMapping;
			Containers::Buffer< Structure::LocalBundleStructureMapping > projectionMapping;

			template < bool forCameras >
			void Initialize( __in const LocalProjectionProviderDataCountsAMP* pCounts )
			{
				cameras.Allocate( pCounts->cameraCount );
				points.Allocate( pCounts->pointCount );
				projections.Allocate( pCounts->projectionCount );
				camerasGlobalMapping.Allocate( pCounts->cameraCount );
				pointGlobalMapping.Allocate( pCounts->pointCount );
				
				if ( forCameras )
				{
					projectionMapping.Allocate( pCounts->cameraCount );
				}
				else
				{
					projectionMapping.Allocate( pCounts->pointCount );
				}
			}
		};
	}

	template < class CameraModel >
	class LocalProjectionProviderAMP;

	template < class CameraModel >
	class LocalProjectionProviderDataAMP
	{
	public:
	
		static void GetCountsForCameras(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const uint cameraStartIx,
			__in const size_t memoryLimitKB,
			__out LocalProjectionProviderDataCountsAMP* pCounts ) __CPU_ONLY
		{
			const uint totalCameraCount = ( uint )pGlobalProvider->GetCameraCount( );
			int64 memoryLeftBytes = int64( memoryLimitKB * 1024 );
			
			Containers::Buffer< bool > pointMask;
			pointMask.Allocate( pGlobalProvider->GetPointCount( ) );
			ByteFill( 0, pGlobalProvider->GetPointCount( ), pointMask.Data( ) );

			for ( uint lastCameraIx = cameraStartIx; ( lastCameraIx < totalCameraCount ); lastCameraIx++ )
			{
				const uint projectionCount = ( uint )pGlobalProvider->GetCameraProjectionCount( lastCameraIx );

				uint camPoints = 0;
				for ( uint projI = 0; projI < projectionCount; projI++ )
				{
					uint pointIx = ( uint )pGlobalProvider->GetPointIndex( pGlobalProvider->GetCameraProjectionIndex( lastCameraIx, projI ) );
					if ( !pointMask[pointIx] )
					{
						pointMask[pointIx] = true;
						camPoints++;
					}
				}

				int64 camMemory = int64( sizeof( CameraModel )
					+ sizeof( uint )
					+ sizeof( Structure::LocalBundleStructureMapping )
					+ projectionCount * sizeof( Projection )
					+ camPoints * ( sizeof( Vector3 ) + sizeof( uint ) ) );
				
				memoryLeftBytes -= camMemory;
				if ( memoryLeftBytes < 0 )
				{
					break;
				}

				pCounts->cameraCount++;
				pCounts->pointCount += camPoints;
				pCounts->projectionCount += projectionCount;
			}
		}

		static void GetCountsForPoints(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const uint pointStartIx,
			__in const size_t memoryLimitKB,
			__out LocalProjectionProviderDataCountsAMP* pCounts ) __CPU_ONLY
		{
			const uint totalPointCount = ( uint )pGlobalProvider->GetPointCount( );
			int64 memoryLeftBytes = int64( memoryLimitKB * 1000 );

			Containers::Buffer< bool > cameraMask;
			cameraMask.Allocate( pGlobalProvider->GetCameraCount( ) );
			ByteFill( 0, pGlobalProvider->GetCameraCount( ), cameraMask.Data( ) );

			for ( uint lastPointIx = pointStartIx; ( lastPointIx < totalPointCount ); lastPointIx++ )
			{
				const uint projectionCount = ( uint )pGlobalProvider->GetPointProjectionCount( lastPointIx );

				uint pointCameras = 0;
				for ( uint projI = 0; projI < projectionCount; projI++ )
				{
					uint cameraIx = ( uint )pGlobalProvider->GetCameraIndex( pGlobalProvider->GetPointProjectionIndex( lastPointIx, projI ) );
					if ( !cameraMask[cameraIx] )
					{
						cameraMask[cameraIx] = true;
						pointCameras++;
					}
				}

				int64 pointMemory = int64( sizeof( Vector3 )
					+ sizeof( uint )
					+ sizeof( Structure::LocalBundleStructureMapping )
					+ projectionCount * sizeof( Projection )
					+ pointCameras * ( sizeof( CameraModel ) + sizeof( uint ) ) );

				memoryLeftBytes -= pointMemory;
				if ( memoryLeftBytes < 0 )
				{
					break;
				}

				pCounts->cameraCount += pointCameras;
				pCounts->pointCount++;
				pCounts->projectionCount += projectionCount;
			}
		}

		static void CreateForCameras(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const uint cameraStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__in accelerator_view dstAccelerator,
			__deref_out LocalProjectionProviderDataAMP< CameraModel >** ppData ) __CPU_ONLY
		{
			Internal::LocalProjectionProviderDataTemp< CameraModel > temp;
			temp.Initialize< true >( pCounts );

			FillTempForCameras( pGlobalProvider, cameraStartIx, pCounts, &temp );

			*ppData = new LocalProjectionProviderDataAMP< CameraModel >( &temp, dstAccelerator );
		}

		static void CreateForPoints(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const uint pointStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__in accelerator_view dstAccelerator,
			__deref_out LocalProjectionProviderDataAMP< CameraModel >** ppData ) __CPU_ONLY
		{
			Internal::LocalProjectionProviderDataTemp< CameraModel > temp;
			temp.Initialize< false >( pCounts );

			FillTempForPoints( pGlobalProvider, pointStartIx, pCounts, &temp );

			*ppData = new LocalProjectionProviderDataAMP< CameraModel >( &temp, dstAccelerator );
		}

	protected:

		LocalProjectionProviderDataAMP( 
			__in const Internal::LocalProjectionProviderDataTemp< CameraModel >* pTemp,
			__in accelerator_view dstAccelerator ) __CPU_ONLY
			:
			m_cameras( (int)pTemp->cameras.Length(), dstAccelerator ),
			m_points( (int)pTemp->points.Length(), dstAccelerator ),
			m_projections( (int)pTemp->projections.Length(), dstAccelerator ),
			m_globalMappingCameras( (int)pTemp->camerasGlobalMapping.Length(), dstAccelerator ),
			m_globalMappingPoints( (int)pTemp->pointGlobalMapping.Length(), dstAccelerator ),
			m_projectionMapping( (int)pTemp->projectionMapping.Length(), dstAccelerator )
		{
			copy( pTemp->cameras.Data( ), m_cameras );
			copy( pTemp->points.Data( ), m_points );
			copy( pTemp->projections.Data( ), m_projections );
			copy( pTemp->camerasGlobalMapping.Data( ), m_globalMappingCameras );
			copy( pTemp->pointGlobalMapping.Data( ), m_globalMappingPoints );
			copy( pTemp->projectionMapping.Data( ), m_projectionMapping );
		}

		static void FillTempForCameras( 
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__out Internal::LocalProjectionProviderDataTemp< CameraModel >* pTemp ) __CPU_ONLY
		{
			constexpr const uint INVALID_INDEX = 0xFFFFFFFF;

			CameraModel* pCamerasDst = pTemp->cameras.Data( );
			uint* pCameraGlobalMapping = pTemp->camerasGlobalMapping.Data( );

			Vector3* pPointDst = pTemp->points.Data( );
			uint* pPointGlobalMapping = pTemp->pointGlobalMapping.Data( );

			Projection* pProjectionDst = pTemp->projections.Data( );
			Structure::LocalBundleStructureMapping* pProjectionMapping = pTemp->projectionMapping.Data( );

			Containers::Buffer< uint > pointMask;
			pointMask.Allocate( pProvider->GetPointCount( ) );
			pointMask.Fill( INVALID_INDEX );

			uint pointsWritten = 0;
			uint projectionsWritten = 0;
			for ( uint localCameraIx = 0; localCameraIx < pCounts->cameraCount; localCameraIx++ )
			{
				const uint globalCameraIx = localCameraIx + cameraStartIx;
				const uint projectionCount = ( uint )pProvider->GetCameraProjectionCount( globalCameraIx );

				pCamerasDst->SetCopy( pProvider->GetCamera( globalCameraIx ) );
				pCamerasDst++;

				*pCameraGlobalMapping = globalCameraIx;
				pCameraGlobalMapping++;

				pProjectionMapping->startIndex = projectionsWritten;
				pProjectionMapping->projectionCount = projectionCount;
				projectionsWritten += projectionCount;
				pProjectionMapping++;
				
				for ( uint projI = 0; projI < projectionCount; projI++ )
				{
					const uint projectionIx = ( uint )pProvider->GetCameraProjectionIndex( globalCameraIx, projI );
					const Projection* pSrcProjection = pProvider->GetProjection( projectionIx );

					ShallowCopy( pSrcProjection, 1, pProjectionDst );

					pProjectionDst->cameraIndex = localCameraIx;

					uint pointIndex = pointMask[pProjectionDst->pointIndex];
					if ( pointIndex == INVALID_INDEX )
					{
						pointIndex = pointsWritten;
						pointsWritten++;

						*pPointGlobalMapping = pProjectionDst->pointIndex;
						pPointGlobalMapping++;

						*pPointDst = *pProvider->GetPoint( pProjectionDst->pointIndex );
						pPointDst++;

						pointMask[pProjectionDst->pointIndex] = pointIndex;
					}

					pProjectionDst->pointIndex = pointIndex;
					pProjectionDst++;
				}
			}
		}

		static void FillTempForPoints(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint pointStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__out Internal::LocalProjectionProviderDataTemp< CameraModel >* pTemp ) __CPU_ONLY
		{
			constexpr const uint INVALID_INDEX = 0xFFFFFFFF;

			CameraModel* pCamerasDst = pTemp->cameras.Data( );
			uint* pCameraGlobalMapping = pTemp->camerasGlobalMapping.Data( );

			Vector3* pPointDst = pTemp->points.Data( );
			uint* pPointGlobalMapping = pTemp->pointGlobalMapping.Data( );

			Projection* pProjectionDst = pTemp->projections.Data( );
			Structure::LocalBundleStructureMapping* pProjectionMapping = pTemp->projectionMapping.Data( );

			Containers::Buffer< uint > cameraMask;
			cameraMask.Allocate( pProvider->GetCameraCount( ) );
			cameraMask.Fill( INVALID_INDEX );

			uint camerasWritten = 0;
			uint projectionsWritten = 0;
			for ( uint localPointIx = 0; localPointIx < pCounts->pointCount; localPointIx++ )
			{
				const uint globalPointIx = localPointIx + pointStartIx;
				const uint projectionCount = ( uint )pProvider->GetPointProjectionCount( globalPointIx );

				*pPointDst = *pProvider->GetPoint( globalPointIx );
				pPointDst++;

				*pPointGlobalMapping = globalPointIx;
				pPointGlobalMapping++;

				pProjectionMapping->startIndex = projectionsWritten;
				pProjectionMapping->projectionCount = projectionCount;
				projectionsWritten += projectionCount;
				pProjectionMapping++;

				for ( uint projI = 0; projI < projectionCount; projI++ )
				{
					const uint projectionIx = ( uint )pProvider->GetPointProjectionIndex( globalPointIx, projI );
					const Projection* pSrcProjection = pProvider->GetProjection( projectionIx );

					ShallowCopy( pSrcProjection, 1, pProjectionDst );

					pProjectionDst->pointIndex = localPointIx;

					uint cameraIndex = cameraMask[pProjectionDst->cameraIndex];
					if ( cameraIndex == INVALID_INDEX )
					{
						cameraIndex = camerasWritten;
						camerasWritten++;

						*pCameraGlobalMapping = pProjectionDst->cameraIndex;
						pCameraGlobalMapping++;

						pCamerasDst->SetCopy( pProvider->GetCamera( pProjectionDst->cameraIndex ) );
						pCamerasDst++;

						cameraMask[pProjectionDst->cameraIndex] = cameraIndex;
					}

					pProjectionDst->cameraIndex = cameraIndex;
					pProjectionDst++;
				}
			}
		}

	protected:

		array< CameraModel, 1 > m_cameras;
		array< Vector3, 1 > m_points;
		array< Projection, 1 > m_projections;

		// Mapping
		array< uint, 1 > m_globalMappingCameras;
		array< uint, 1 > m_globalMappingPoints;

		array< Structure::LocalBundleStructureMapping, 1 > m_projectionMapping;

		friend class LocalProjectionProviderAMP< CameraModel >;
	};

}
