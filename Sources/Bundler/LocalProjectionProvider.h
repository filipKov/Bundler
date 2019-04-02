#pragma once

namespace Bundler {

	namespace Structure {
		struct LocalBundleStructureMapping
		{
			uint startIndex;
			uint projectionCount;
		};
	}

	template < class CameraModel >
	class LocalProjectionProvider
	{
	public:

		uint InitializeForCameras(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraStartIx,
			__in const int64 memoryLimitKB )
		{
			Containers::PagedVector< const CameraModel*, 5 > tempCameras;
			Containers::PagedVector< Vector3, 10 > tempPoints;
			Containers::PagedVector< Projection, 13 > tempProjections;
			Containers::PagedVector< Structure::LocalBundleStructureMapping, 5 > tempMapping;

			std::unordered_map< uint, uint > pointIndexTable;
		
			int64 memoryLeft = memoryLimitKB;
			for( uint cameraIx = cameraStartIx; cameraIx < (uint)pProvider->GetCameraCount( ); cameraIx++ )
			{
				memoryLeft -= GetMemoryForCameraKB( pProvider, cameraIx, &pointIndexTable );
				if ( memoryLeft < 0 )
				{
					break;
				}

				tempCameras.Add( pProvider->m_pCameras + cameraIx );

				Structure::LocalBundleStructureMapping cameraMappingEntry;
				cameraMappingEntry.startIndex = (uint)tempProjections.Length( );
				
				for ( uint projI = 0; projI < ( uint )pProvider->GetCameraProjectionCount( cameraIx ); projI++ )
				{
					const uint projIx = ( uint )pProvider->GetCameraProjectionIndex( cameraIx, projI );
					
					Projection newProjection = *( pProvider->m_pProjections + projIx );

					if ( pointIndexTable.find( newProjection.pointIndex ) == pointIndexTable.end( ) )
					{
						pointIndexTable[newProjection.pointIndex] = (uint)tempPoints.Length( );
						tempPoints.Add( ELEMENT( pProvider->m_pPoints, newProjection.pointIndex ) );
					}

					newProjection.pointIndex = pointIndexTable[newProjection.pointIndex];
					newProjection.cameraIndex = cameraIx - cameraStartIx;
					tempProjections.Add( newProjection );
				}

				cameraMappingEntry.projectionCount = (uint)tempProjections.Length( ) - cameraMappingEntry.startIndex;
				tempMapping.Add( cameraMappingEntry );
			}
			 			
			m_points.SetCopy( tempPoints );
			m_projections.SetCopy( tempProjections );
			m_mapping.SetCopy( tempMapping );

			const uint cameraCount = ( uint )tempCameras.Length( );
			m_cameras.Allocate( cameraCount );
			m_cameraModels.Allocate( cameraCount );
			for ( uint cameraI = 0; cameraI < cameraCount; cameraI++ )
			{
				tempCameras[cameraI]->MakeCopy( &m_cameras[cameraI], &m_cameraModels[cameraI] );
			}

			m_cameraGlobalMapping.Allocate( cameraCount );
			for ( uint i = 0; i < cameraCount; i++ )
			{
				m_cameraGlobalMapping[i] = cameraStartIx + i;
			}

			m_pointGlobalMapping.Allocate( m_points.Length( ) );
			for ( auto it = pointIndexTable.begin(); it != pointIndexTable.end(); it++ )
			{
				m_pointGlobalMapping[it->second] = it->first;
			}

			return cameraCount;
		}

		uint InitializeForPoints(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint pointStartIx,
			__in const size_t memoryLimitKB )
		{
			Containers::PagedVector< const CameraModel*, 5 > tempCameras;
			Containers::PagedVector< Vector3, 10 > tempPoints;
			Containers::PagedVector< Projection, 13 > tempProjections;
			Containers::PagedVector< Structure::LocalBundleStructureMapping, 5 > tempMapping;

			std::unordered_map< uint, uint > cameraIndexTable;

			int64 memoryLeft = memoryLimitKB;
			for ( uint pointIx = pointStartIx; pointIx < ( uint )pProvider->GetPointCount( ); pointIx++ )
			{
				memoryLeft -= GetMemoryForPointKB( pProvider, pointIx, &cameraIndexTable );
				if ( memoryLeft < 0 )
				{
					break;
				}

				tempPoints.Add( ELEMENT( pProvider->m_pPoints, pointIx ) );

				Structure::LocalBundleStructureMapping pointMappingEntry;
				pointMappingEntry.startIndex = ( uint )tempProjections.Length( );

				for ( uint projI = 0; projI < ( uint )pProvider->GetPointProjectionCount( pointIx ); projI++ )
				{
					const uint projIx = ( uint )pProvider->GetPointProjectionIndex( pointIx, projI );

					Projection newProjection = *( pProvider->m_pProjections + projIx );

					if ( cameraIndexTable.find( newProjection.cameraIndex ) == cameraIndexTable.end( ) )
					{
						cameraIndexTable[newProjection.cameraIndex] = ( uint )tempCameras.Length( );
						tempCameras.Add( pProvider->m_pCameras + newProjection.cameraIndex );
					}

					newProjection.pointIndex = pointIx - pointStartIx;
					newProjection.cameraIndex = cameraIndexTable[newProjection.cameraIndex];
					tempProjections.Add( newProjection );
				}

				pointMappingEntry.projectionCount = ( uint )tempProjections.Length( ) - pointMappingEntry.startIndex;
				tempMapping.Add( pointMappingEntry );
			}

			m_points.SetCopy( tempPoints );
			m_projections.SetCopy( tempProjections );
			m_mapping.SetCopy( tempMapping );

			const uint cameraCount = ( uint )tempCameras.Length( );
			m_cameras.Allocate( cameraCount );
			m_cameraModels.Allocate( cameraCount );
			for ( uint cameraI = 0; cameraI < cameraCount; cameraI++ )
			{
				tempCameras[cameraI]->MakeCopy( &m_cameras[cameraI], &m_cameraModels[cameraI] );
			}

			m_cameraGlobalMapping.Allocate( cameraCount );
			for ( auto it = cameraIndexTable.begin(); it != cameraIndexTable.end(); it++ )
			{
				m_cameraGlobalMapping[it->second] = it->first;
			}

			m_pointGlobalMapping.Allocate( m_points.Length( ) );
			for ( uint i = 0; i < (uint)m_points.Length(); i++ )
			{
				m_pointGlobalMapping[i] = pointStartIx + i;
			}

			return ( uint )m_points.Length( );
		}

		uint GetCameraCount( ) const
		{
			return ( uint )m_cameras.Length( );
		}

		uint GetCameraProjectionCount( __in const uint cameraIx ) const
		{
			return m_mapping[cameraIx].projectionCount;
		}

		uint GetCameraProjectionIndex( __in const uint cameraIx, __in const uint ix ) const
		{
			return m_mapping[cameraIx].startIndex + ix;
		}

		uint GetCameraIndex( __in const uint projectionIx ) const
		{
			return m_projections[projectionIx].cameraIndex;
		}

		uint GetGlobalCameraIndex( __in const uint localCameraIx ) const
		{
			return m_cameraGlobalMapping[localCameraIx];
		}


		uint GetPointCount( ) const
		{
			return ( uint )m_points.Length( );
		}

		uint GetPointProjectionCount( __in const uint pointIx ) const
		{
			return m_mapping[pointIx].projectionCount;
		}

		uint GetPointProjectionIndex( __in const uint pointIx, __in const uint ix ) const
		{
			return m_mapping[pointIx].startIndex + ix;
		}

		uint GetPointIndex( __in const uint projectionIx ) const
		{
			return m_projections[projectionIx].pointIndex;
		}

		uint GetGlobalPointIndex( __in const uint localPointIx ) const
		{
			return m_pointGlobalMapping[localPointIx];
		}


		uint GetProjectionCount( ) const
		{
			return ( uint )m_projections.Length( );
		}

		template < bool getCameraBlock, bool getPointBlock, bool getResiduals >
		void GetProjectionBlock(
			__in const size_t projectionIx,
			__out_ecount_opt( 2 * CameraModel::cameraParameterCount ) Scalar* pCameraBlock,
			__out_ecount_opt( 2 * POINT_PARAM_COUNT ) Scalar* pPointBlock,
			__out_ecount_opt( 2 ) Scalar* pResiduals ) const
		{
			DScalar< CameraModel::totalParamCount > residuals[2];
			GetProjection( projectionIx, residuals );

			if ( getCameraBlock )
			{
				_ASSERT_EXPR( pCameraBlock != NULL, "Camera block must not be null" );
				ExtractCameraParameters( residuals + 0, pCameraBlock + 0 );
				ExtractCameraParameters( residuals + 1, pCameraBlock + CameraModel::cameraParameterCount );
			}

			if ( getPointBlock )
			{
				_ASSERT_EXPR( pPointBlock != NULL, "Point block must not be null" );
				ExtractPointParameters( residuals + 0, pPointBlock + 0 );
				ExtractPointParameters( residuals + 1, pPointBlock + POINT_PARAM_COUNT );
			}

			if ( getResiduals )
			{
				_ASSERT_EXPR( pResiduals != NULL, "Residual block must not be null" );
				ELEMENT( pResiduals, 0 ) = ELEMENT( residuals, 0 ).GetFx( );
				ELEMENT( pResiduals, 1 ) = ELEMENT( residuals, 1 ).GetFx( );
			}
		}

	protected:

		int64 ByteToKB( __in size_t byteSize )
		{
			return max( 1, int64( ceil( byteSize / 1000.0 ) ) );
		}

		int64 GetMemoryForCameraKB(
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const uint cameraIx,
			__in const std::unordered_map< uint, uint >* pPointIndexTable )
		{
			const uint projectionCount = (uint)pProvider->GetCameraProjectionCount( cameraIx );
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

		int64 GetMemoryForPointKB(
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

		void GetProjection(
			__in const size_t projectionIx,
			__out_ecount( 2 ) DScalar< CameraModel::totalParamCount >* pResiduals ) const
		{
			const Projection* pProjection = m_projections.Data() + projectionIx;

			const uint cameraIndex = pProjection->cameraIndex;
			const uint pointIndex = pProjection->pointIndex;

			m_cameraModels[cameraIndex].ProjectPoint( m_points[pointIndex].Elements( ), pProjection->projectedPoint.Elements( ), pResiduals );
		}

		inline void ExtractCameraParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const
		{
			ShallowCopy< Scalar >(
				pResidual->GetDiff( ).Elements( ),
				CameraModel::cameraParameterCount,
				pDestination );
		}

		inline void ExtractPointParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const
		{
			ShallowCopy< Scalar >(
				pResidual->GetDiff( ).Elements( ) + CameraModel::pointParamStartIx,
				POINT_PARAM_COUNT,
				pDestination );
		}

	protected:
		
		Containers::Buffer< Camera > m_cameras;
		Containers::Buffer< CameraModel > m_cameraModels;

		Containers::Buffer< Vector3 > m_points;
		
		Containers::Buffer< Projection > m_projections;

		Containers::Buffer< Structure::LocalBundleStructureMapping > m_mapping;
		
		Containers::Buffer< uint > m_cameraGlobalMapping;
		Containers::Buffer< uint > m_pointGlobalMapping;
	};

}