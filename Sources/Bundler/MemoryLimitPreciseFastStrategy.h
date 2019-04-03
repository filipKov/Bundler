#pragma once

namespace Bundler {

	template < class CameraModel >
	class MemoryLimitedPreciseFastStrategy
	{
	protected:
	
		struct Temp
		{
			Containers::PagedVector< const CameraModel*, 5 > cameras;
			Containers::PagedVector< Vector3, 10 > points;
			Containers::PagedVector< Projection, 13 > projections;
			Containers::PagedVector< Structure::LocalBundleStructureMapping, 5 > mapping;
	
			Containers::Buffer< uint > indexTable;
		};

		static const uint INVALID_INDEX = 0xFFFFFFFF;
	
	public:
	
		static void InitializeForCameras(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraStartIx,
			__in const int64 memoryLimitKB,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			Temp temp;
			const uint totalPointCount = ( uint )pProvider->GetPointCount( );
			temp.indexTable.Allocate( totalPointCount );
			temp.indexTable.Fill( INVALID_INDEX );

			uint* pIndexTable = temp.indexTable.Data( );

			int64 memoryLeft = memoryLimitKB;
			for ( uint cameraIx = cameraStartIx; cameraIx < ( uint )pProvider->GetCameraCount( ); cameraIx++ )
			{
				memoryLeft -= GetMemoryForCameraKB( pProvider, cameraIx, pIndexTable );
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
	
					if ( ELEMENT( pIndexTable, newProjection.pointIndex ) == INVALID_INDEX )
					{
						ELEMENT( pIndexTable, newProjection.pointIndex ) = ( uint )temp.points.Length( );
						temp.points.Add( ELEMENT( pProvider->m_pPoints, newProjection.pointIndex ) );
					}
	
					newProjection.pointIndex = ELEMENT( pIndexTable, newProjection.pointIndex );
					newProjection.cameraIndex = cameraIx - cameraStartIx;
					temp.projections.Add( newProjection );
				}
	
				cameraMappingEntry.projectionCount = ( uint )temp.projections.Length( ) - cameraMappingEntry.startIndex;
				temp.mapping.Add( cameraMappingEntry );
			}
	
			ConvertTempToProvider( &temp, pLocalProvider );
	
			AddGlobalMappingForCameraInit( cameraStartIx, totalPointCount, pIndexTable, pLocalProvider );
		}
	
		static void InitializeForPoints(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint pointStartIx,
			__in const size_t memoryLimitKB,
			__out LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			Temp temp;
			const uint totalCameraCount = ( uint )pProvider->GetCameraCount( );
			temp.indexTable.Allocate( totalCameraCount );
			temp.indexTable.Fill( INVALID_INDEX );

			uint* pIndexTable = temp.indexTable.Data( );
	
			int64 memoryLeft = memoryLimitKB;
			for ( uint pointIx = pointStartIx; pointIx < ( uint )pProvider->GetPointCount( ); pointIx++ )
			{
				memoryLeft -= GetMemoryForPointKB( pProvider, pointIx, pIndexTable );
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
	
					if ( ELEMENT( pIndexTable, newProjection.cameraIndex ) == INVALID_INDEX )
					{
						ELEMENT( pIndexTable, newProjection.cameraIndex ) = ( uint )temp.cameras.Length( );
						temp.cameras.Add( pProvider->m_pCameras + newProjection.cameraIndex );
					}
	
					newProjection.pointIndex = pointIx - pointStartIx;
					newProjection.cameraIndex = ELEMENT( pIndexTable, newProjection.cameraIndex );
					temp.projections.Add( newProjection );
				}
	
				pointMappingEntry.projectionCount = ( uint )temp.projections.Length( ) - pointMappingEntry.startIndex;
				temp.mapping.Add( pointMappingEntry );
			}
	
			ConvertTempToProvider( &temp, pLocalProvider );
	
			AddGlobalMappingForPointInit( pointStartIx, totalCameraCount, pIndexTable, pLocalProvider );
		}
	
	protected:
	
		static int64 ByteToKB( __in size_t byteSize )
		{
			return max( 1, int64( ceil( byteSize / 1000.0 ) ) );
		}
	
		static int64 GetMemoryForCameraKB(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraIx,
			__in const uint* pPointIndexTable )
		{
			const uint projectionCount = ( uint )pProvider->GetCameraProjectionCount( cameraIx );
			uint newPoints = 0;
	
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = ( uint )pProvider->GetCameraProjectionIndex( cameraIx, projI );
				const uint pointIx = ELEMENT( pProvider->m_pProjections, projectionIx ).pointIndex;
				if ( ELEMENT( pPointIndexTable, pointIx ) == INVALID_INDEX )
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
			__in const uint* pCameraIndexTable )
		{
			const uint projectionCount = ( uint )pProvider->GetPointProjectionCount( pointIx );
			uint newCameras = 0;
	
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = ( uint )pProvider->GetPointProjectionIndex( pointIx, projI );
				const uint cameraIx = ELEMENT( pProvider->m_pProjections, projectionIx ).cameraIndex;
				if ( ELEMENT( pCameraIndexTable, cameraIx ) == INVALID_INDEX )
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
	
			Camera* pCamDst = pLocalProvider->m_cameras.Data( );
			CameraModel* pCamModelDst = pLocalProvider->m_cameraModels.Data( );
			for ( uint cameraI = 0; cameraI < cameraCount; cameraI++ )
			{
				pTemp->cameras[cameraI]->MakeCopy( pCamDst, pCamModelDst );
				pCamDst++;
				pCamModelDst++;
			}
		}
	
		static void AddGlobalMappingForCameraInit(
			__in const uint cameraStartIx,
			__in const uint indexTableSize,
			__in_ecount( indexTableSize ) const uint* pIndexTable,
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
			for ( uint i = 0; i < indexTableSize; i++ )
			{
				if ( ELEMENT( pIndexTable, i ) != INVALID_INDEX )
				{
					ELEMENT( pPtMappingDst, ELEMENT( pIndexTable, i ) ) = i;
				}
			}
		}
	
		static void AddGlobalMappingForPointInit(
			__in const uint pointStartIx,
			__in const uint indexTableSize,
			__in_ecount( indexTableSize ) const uint* pIndexTable,
			__inout LocalProjectionProvider< CameraModel >* pLocalProvider )
		{
			const uint cameraCount = pLocalProvider->GetCameraCount( );
	
			pLocalProvider->m_cameraGlobalMapping.Allocate( cameraCount );
			uint* pCamMappingDst = pLocalProvider->m_cameraGlobalMapping.Data( );
			for ( uint i = 0; i < indexTableSize; i++ )
			{
				if ( ELEMENT( pIndexTable, i ) != INVALID_INDEX )
				{
					ELEMENT( pCamMappingDst, ELEMENT( pIndexTable, i ) ) = i;
				}
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