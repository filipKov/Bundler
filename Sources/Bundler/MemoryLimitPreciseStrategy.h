#pragma once

namespace Bundler {

	template < class CameraModel >
	class MemoryLimitedPreciseStrategy
	{
	protected:

		struct Temp
		{
			Containers::PagedVector< const CameraModel*, 5 > cameras;
			Containers::PagedVector< Vector3, 10 > points;
			Containers::PagedVector< Projection, 13 > projections;
			Containers::PagedVector< Structure::LocalBundleStructureMapping, 5 > mapping;

			std::unordered_map< uint, uint > indexTable;
		};

	public:

		static void InitializeForCameras(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraStartIx,
			__in const int64 memoryLimitKB,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			Temp temp;

			int64 memoryLeft = memoryLimitKB;
			for ( uint cameraIx = cameraStartIx; cameraIx < ( uint )pProvider->GetCameraCount( ); cameraIx++ )
			{
				memoryLeft -= GetMemoryForCameraKB( pProvider, cameraIx, &temp.indexTable );
				if ( memoryLeft < 0 )
				{
					break;
				}

				temp.cameras.Add( pProvider->m_pCameras + cameraIx );

				Structure::LocalBundleStructureMapping cameraMappingEntry;
				cameraMappingEntry.startIndex = ( uint )temp.projections.Length( );

				for ( uint projI = 0; projI < ( uint )pProvider->GetCameraProjectionCount( cameraIx ); projI++ )
				{
					const uint projIx = ( uint )pProvider->GetCameraProjectionIndex( cameraIx, projI );

					Projection newProjection = *( pProvider->m_pProjections + projIx );

					if ( temp.indexTable.find( newProjection.pointIndex ) == temp.indexTable.end( ) )
					{
						temp.indexTable[newProjection.pointIndex] = ( uint )temp.points.Length( );
						temp.points.Add( ELEMENT( pProvider->m_pPoints, newProjection.pointIndex ) );
					}

					newProjection.pointIndex = temp.indexTable[newProjection.pointIndex];
					newProjection.cameraIndex = cameraIx - cameraStartIx;
					temp.projections.Add( newProjection );
				}

				cameraMappingEntry.projectionCount = ( uint )temp.projections.Length( ) - cameraMappingEntry.startIndex;
				temp.mapping.Add( cameraMappingEntry );
			}

			ConvertTempToProvider( &temp, pLocalProvider );

			AddGlobalMappingForCameraInit( cameraStartIx, &temp.indexTable, pLocalProvider );
		}

		static void InitializeForPoints(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint pointStartIx,
			__in const size_t memoryLimitKB,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			Temp temp;

			int64 memoryLeft = memoryLimitKB;
			for ( uint pointIx = pointStartIx; pointIx < ( uint )pProvider->GetPointCount( ); pointIx++ )
			{
				memoryLeft -= GetMemoryForPointKB( pProvider, pointIx, &temp.indexTable );
				if ( memoryLeft < 0 )
				{
					break;
				}

				temp.points.Add( ELEMENT( pProvider->m_pPoints, pointIx ) );

				Structure::LocalBundleStructureMapping pointMappingEntry;
				pointMappingEntry.startIndex = ( uint )temp.projections.Length( );

				for ( uint projI = 0; projI < ( uint )pProvider->GetPointProjectionCount( pointIx ); projI++ )
				{
					const uint projIx = ( uint )pProvider->GetPointProjectionIndex( pointIx, projI );

					Projection newProjection = *( pProvider->m_pProjections + projIx );

					if ( temp.indexTable.find( newProjection.cameraIndex ) == temp.indexTable.end( ) )
					{
						temp.indexTable[newProjection.cameraIndex] = ( uint )temp.cameras.Length( );
						temp.cameras.Add( pProvider->m_pCameras + newProjection.cameraIndex );
					}

					newProjection.pointIndex = pointIx - pointStartIx;
					newProjection.cameraIndex = temp.indexTable[newProjection.cameraIndex];
					temp.projections.Add( newProjection );
				}

				pointMappingEntry.projectionCount = ( uint )temp.projections.Length( ) - pointMappingEntry.startIndex;
				temp.mapping.Add( pointMappingEntry );
			}

			ConvertTempToProvider( &temp, pLocalProvider );

			AddGlobalMappingForPointInit( pointStartIx, &temp.indexTable, pLocalProvider );
		}

	protected:

		static int64 ByteToKB( __in size_t byteSize )
		{
			return max( 1, int64( ceil( byteSize / 1000.0 ) ) );
		}

		static int64 GetMemoryForCameraKB(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraIx,
			__in const std::unordered_map< uint, uint >* pPointIndexTable )
		{
			const uint projectionCount = ( uint )pProvider->GetCameraProjectionCount( cameraIx );
			uint newPoints = 0;

			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = ( uint )pProvider->GetCameraProjectionIndex( cameraIx, projI );
				const uint pointIx = ELEMENT( pProvider->m_pProjections, projectionIx ).pointIndex;
				if ( pPointIndexTable->find( pointIx ) == pPointIndexTable->end( ) )
				{
					newPoints++;
				}
			}

			size_t byteSize =
				sizeof( Camera ) + sizeof( CameraModel ) +
				projectionCount * sizeof( Projection ) +
				newPoints * sizeof( Vector3 );

			return ByteToKB( byteSize );
		}

		static int64 GetMemoryForPointKB(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint pointIx,
			__in const std::unordered_map< uint, uint >* pCameraIndexTable )
		{
			const uint projectionCount = ( uint )pProvider->GetPointProjectionCount( pointIx );
			uint newCameras = 0;

			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = ( uint )pProvider->GetPointProjectionIndex( pointIx, projI );
				const uint cameraIx = ELEMENT( pProvider->m_pProjections, projectionIx ).cameraIndex;
				if ( pCameraIndexTable->find( cameraIx ) == pCameraIndexTable->end( ) )
				{
					newCameras++;
				}
			}

			size_t byteSize =
				sizeof( Vector3 ) +
				newCameras * ( sizeof( Camera ) + sizeof( CameraModel ) ) +
				projectionCount * sizeof( Projection );

			return ByteToKB( byteSize );
		}

		static void ConvertTempToProvider(
			__in const Temp* pTemp,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			pLocalProvider->m_points.SetCopy( pTemp->points );
			pLocalProvider->m_projections.SetCopy( pTemp->projections );
			pLocalProvider->m_mapping.SetCopy( pTemp->mapping );

			const uint cameraCount = ( uint )pTemp->cameras.Length( );
			pLocalProvider->m_cameras.Allocate( cameraCount );
			pLocalProvider->m_cameraModels.Allocate( cameraCount );
			
			Camera* pCamDst = pLocalProvider->m_cameras.Data();
			CameraModel* pCamModelDst = pLocalProvider->m_cameraModels.Data();
			for ( uint cameraI = 0; cameraI < cameraCount; cameraI++ )
			{
				pTemp->cameras[cameraI]->MakeCopy( pCamDst, pCamModelDst );
				pCamDst++;
				pCamModelDst++;
			}
		}

		static void AddGlobalMappingForCameraInit(
			__in const uint cameraStartIx,
			__in const std::unordered_map< uint, uint >* pIndexTable,
			__inout LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			const uint cameraCount = pLocalProvider->GetCameraCount( );

			pLocalProvider->m_cameraGlobalMapping.Allocate( cameraCount );
			uint* pCamMappingDst = pLocalProvider->m_cameraGlobalMapping.Data( );
			for ( uint i = 0; i < cameraCount; i++ )
			{
				*pCamMappingDst = cameraStartIx + i;
				pCamMappingDst++;
			}

			const uint pointCount = pLocalProvider->GetPointCount( );

			pLocalProvider->m_pointGlobalMapping.Allocate( pointCount );
			uint* pPtMappingDst = pLocalProvider->m_pointGlobalMapping.Data( );
			for ( auto it = pIndexTable->begin( ); it != pIndexTable->end( ); it++ )
			{
				ELEMENT( pPtMappingDst, it->second ) = it->first;
			}
		}

		static void AddGlobalMappingForPointInit(
			__in const uint pointStartIx,
			__in const std::unordered_map< uint, uint >* pIndexTable,
			__inout LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			const uint cameraCount = pLocalProvider->GetCameraCount( );

			pLocalProvider->m_cameraGlobalMapping.Allocate( cameraCount );
			uint* pCamMappingDst = pLocalProvider->m_cameraGlobalMapping.Data( );
			for ( auto it = pIndexTable->begin(); it != pIndexTable->end(); it++ )
			{
				ELEMENT( pCamMappingDst, it->second ) = it->first;
			}

			const uint pointCount = pLocalProvider->GetPointCount( );

			pLocalProvider->m_pointGlobalMapping.Allocate( pointCount );
			uint* pPtMappingDst = pLocalProvider->m_pointGlobalMapping.Data( );
			for ( uint i = 0; i < pointCount; i++ )
			{
				*pPtMappingDst = pointStartIx + i;
				pPtMappingDst++;
			}
		}

	};

}