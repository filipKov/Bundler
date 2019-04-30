#pragma once

namespace Bundler {

	namespace Internal 
	{
		struct HessianBlockEntry
		{
			Scalar* pData;
		};

	}

	template < class CameraModel >
	class CachingHessianBlockProvider
	{
	public:
	
		void Initialize( __in const ProjectionProvider< CameraModel >* pProjectionProvider )
		{
			m_pJacobian = pProjectionProvider;

			InitializeBlocks( );
			CalculateBlocks( );
		}
	
		inline size_t GetCameraCount( ) const
		{
			return m_pJacobian->GetCameraCount( );
		}
	
		inline size_t GetCameraProjectionCount( __in const size_t cameraIx ) const
		{
			return m_pJacobian->GetCameraProjectionCount( cameraIx );
		}
	
		inline size_t GetPointCount( ) const
		{
			return m_pJacobian->GetPointCount( );
		}
	
		inline size_t GetPointProjectionCount( __in const size_t pointIx ) const
		{
			return m_pJacobian->GetPointProjectionCount( pointIx );
		}
	
		void GetCameraBlock(
			__in const size_t cameraIx,
			__deref_out_ecount( CameraModel::cameraParameterCount * CameraModel::cameraParameterCount ) Scalar** ppBlock ) const
		{
			*ppBlock = m_cameraBlocks[cameraIx];
		}
	
		void GetCameraPointBlockCam(
			__in const size_t cameraIx,
			__in const size_t ix,
			__out size_t* pPointIx,
			__deref_out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar** ppBlock ) const
		{
			size_t projectionIx = m_pJacobian->GetCameraProjectionIndex( cameraIx, ix );
	
			GetCameraPointBlock( projectionIx, ppBlock );
			*pPointIx = m_pJacobian->GetPointIndex( projectionIx );
		}
	
		void GetCameraPointBlockPt(
			__in const size_t pointIx,
			__in const size_t ix,
			__out size_t* pCameraIx,
			__deref_out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar** ppBlock ) const
		{
			size_t projectionIx = m_pJacobian->GetPointProjectionIndex( pointIx, ix );
	
			GetCameraPointBlock( projectionIx, ppBlock );
			*pCameraIx = m_pJacobian->GetCameraIndex( projectionIx );
		}
	
		void GetPointBlock(
			__in const size_t pointIx,
			__deref_out_ecount( POINT_PARAM_COUNT * POINT_PARAM_COUNT ) Scalar** ppBlock ) const
		{
			*ppBlock = m_pointBlocks[pointIx];
		}
	
	protected:
	
		void GetCameraPointBlock(
			__in const size_t projectionIx,
			__deref_out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar** ppBlock ) const
		{
			*ppBlock = m_projectionBlocks[projectionIx];
		}

		void InitializeBlocks( )
		{
			const size_t cameraCount = GetCameraCount( );
			m_cameraBlocks.Allocate( cameraCount );
			for ( size_t i = 0; i < cameraCount; i++ )
			{
				m_cameraBlocks[i] = m_blockMemory.AllocateArray< Scalar >( CameraModel::cameraParameterCount * CameraModel::cameraParameterCount );
			}

			const size_t pointCount = GetPointCount( );
			m_pointBlocks.Allocate( pointCount );
			for ( size_t i = 0; i < pointCount; i++ )
			{
				m_pointBlocks[i] = m_blockMemory.AllocateArray< Scalar >( POINT_PARAM_COUNT * POINT_PARAM_COUNT );
			}

			const size_t projectionCount = m_pJacobian->GetProjectionCount( );
			m_projectionBlocks.Allocate( projectionCount );
			for ( size_t i = 0; i < projectionCount; i++ )
			{
				m_projectionBlocks[i] = m_blockMemory.AllocateArray< Scalar >( CameraModel::cameraParameterCount * POINT_PARAM_COUNT );
			}
		}

		void CalculateBlocks( )
		{
			const size_t cameraCount = GetCameraCount( );
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				CalculateCameraBlock( cameraIx );
			}

			const size_t pointCount = GetPointCount( );
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				CalculatePointAndProjectionBlocks( pointIx );
			}
		}

		void CalculateCameraBlock( __in const size_t cameraIx )
		{
			const uint projectionCount = ( uint )GetCameraProjectionCount( cameraIx );
			const uint rows = 2 * projectionCount;
			constexpr const uint columns = CameraModel::cameraParameterCount;

			Matrix< Scalar > jacobianCameraBlock( rows, columns );

			Scalar* pBlockDst = jacobianCameraBlock.Elements( );
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const size_t projectionIx = m_pJacobian->GetCameraProjectionIndex( cameraIx, projI );

				m_pJacobian->GetProjectionBlock< true, false, false >(
					projectionIx, pBlockDst, NULL, NULL );

				pBlockDst += columns * 2;
			}

			MatrixMultiplyAtB(
				rows, columns, columns,
				jacobianCameraBlock.Elements( ),
				jacobianCameraBlock.Elements( ),
				m_cameraBlocks[ cameraIx ] );
		}

		void CalculatePointAndProjectionBlocks( __in const size_t pointIx )
		{
			const size_t projectionCount = m_pJacobian->GetPointProjectionCount( pointIx );

			Matrix< Scalar > pointBlocks( ( uint )( projectionCount * 2 ), POINT_PARAM_COUNT );
			Scalar* pPointBlock = pointBlocks.Elements( );

			Scalar cameraBlock[2 * CameraModel::cameraParameterCount];

			for ( size_t projI = 0; projI < projectionCount; projI++ )
			{
				const size_t projectionIx = m_pJacobian->GetPointProjectionIndex( pointIx, projI );

				m_pJacobian->GetProjectionBlock< true, true, false >(
					projectionIx, cameraBlock, pPointBlock, NULL );

				MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, POINT_PARAM_COUNT >(
					cameraBlock, pPointBlock, m_projectionBlocks[projectionIx] );

				pPointBlock += POINT_PARAM_COUNT * 2;
			}

			MatrixMultiplyAtB< Scalar >(
				( uint )( projectionCount * 2 ), 
				POINT_PARAM_COUNT, 
				POINT_PARAM_COUNT,
				pointBlocks.Elements( ), 
				pointBlocks.Elements( ), 
				m_pointBlocks[pointIx] );
		}
	
	protected:
	
		const ProjectionProvider< CameraModel >* m_pJacobian;
	
		Containers::MemoryHeap< 20 > m_blockMemory;

		Containers::Buffer< Scalar* > m_cameraBlocks;
		Containers::Buffer< Scalar* > m_pointBlocks;
		Containers::Buffer< Scalar* > m_projectionBlocks;
	};

}
