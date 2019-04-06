#pragma once

namespace Bundler {

	template < class CameraModel >
	class LocalHessianBlockProviderCPU
	{
	public:
	
		void Initialize( __in const LocalProjectionProviderCPU< CameraModel >* pProjectionProvider )
		{
			m_pJacobian = pProjectionProvider;
		}
	
		__forceinline uint GetCameraCount( ) const
		{
			return m_pJacobian->GetCameraCount( );
		}
	
		__forceinline uint GetCameraProjectionCount( __in const uint localCameraIx ) const
		{
			return m_pJacobian->GetCameraProjectionCount( localCameraIx );
		}

		__forceinline uint GetGlobalCameraIndex( __in const uint localCameraIx ) const
		{
			return m_pJacobian->GetGlobalCameraIndex( localCameraIx );
		}
	
		__forceinline uint GetPointCount( ) const
		{
			return m_pJacobian->GetPointCount( );
		}
	
		__forceinline uint GetPointProjectionCount( __in const uint localPointIx ) const
		{
			return m_pJacobian->GetPointProjectionCount( localPointIx );
		}

		__forceinline uint GetGlobalPointIndex( __in const uint localPointIx ) const
		{
			return m_pJacobian->GetGlobalPointIndex( localPointIx );
		}
	
		void GetCameraBlock(
			__in const uint localCameraIx,
			__out_ecount( CameraModel::cameraParameterCount * CameraModel::cameraParameterCount ) Scalar* pBlock ) const
		{
			const uint projectionCount = ( uint )GetCameraProjectionCount( localCameraIx );
			const uint rows = 2 * projectionCount;
			constexpr const uint columns = CameraModel::cameraParameterCount;
	
			Matrix< Scalar > jacobianCameraBlock( rows, columns );
	
			Scalar* pBlockDst = jacobianCameraBlock.Elements( );
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = m_pJacobian->GetCameraProjectionIndex( localCameraIx, projI );
	
				m_pJacobian->GetProjectionBlock< true, false, false >(
					projectionIx, pBlockDst, NULL, NULL );
	
				pBlockDst += columns * 2;
			}
	
			MatrixMultiplyAtB(
				rows, columns, columns,
				jacobianCameraBlock.Elements( ),
				jacobianCameraBlock.Elements( ),
				pBlock );
		}
	
		void GetCameraPointBlockCam(
			__in const uint localCameraIx,
			__in const uint ix,
			__out uint* pLocalPointIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const
		{
			uint projectionIx = m_pJacobian->GetCameraProjectionIndex( localCameraIx, ix );
	
			GetCameraPointBlock( projectionIx, pBlock );
			*pLocalPointIx = m_pJacobian->GetPointIndex( projectionIx );
		}
	
		void GetCameraPointBlockPt(
			__in const uint localPointIx,
			__in const uint ix,
			__out uint* pLocalCameraIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const
		{
			uint projectionIx = m_pJacobian->GetPointProjectionIndex( localPointIx, ix );
	
			GetCameraPointBlock( projectionIx, pBlock );
			*pLocalCameraIx = m_pJacobian->GetCameraIndex( projectionIx );
		}
	
		void GetPointBlock(
			__in const uint localPointIx,
			__out_ecount( POINT_PARAM_COUNT * POINT_PARAM_COUNT ) Scalar* pBlock ) const
		{
			const uint projectionCount = m_pJacobian->GetPointProjectionCount( localPointIx );
	
			Matrix< Scalar > pointBlocks( ( uint )( projectionCount * 2 ), POINT_PARAM_COUNT );
			Scalar* pBlockDst = pointBlocks.Elements( );
	
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = m_pJacobian->GetPointProjectionIndex( localPointIx, projI );
	
				m_pJacobian->GetProjectionBlock< false, true, false >(
					projectionIx, NULL, pBlockDst, NULL );
	
				pBlockDst += POINT_PARAM_COUNT * 2;
			}
	
			MatrixMultiplyAtB< Scalar >(
				( uint )( projectionCount * 2 ), POINT_PARAM_COUNT, POINT_PARAM_COUNT,
				pointBlocks.Elements( ), pointBlocks.Elements( ), pBlock );
		}
	
	protected:
	
		void GetCameraPointBlock(
			__in const uint projectionIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const
		{
			Scalar cameraBlock[2 * CameraModel::cameraParameterCount];
			Scalar pointBlock[2 * POINT_PARAM_COUNT];
	
			m_pJacobian->GetProjectionBlock< true, true, false >( projectionIx, cameraBlock, pointBlock, NULL );
	
			MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, POINT_PARAM_COUNT >(
				cameraBlock, pointBlock, pBlock );
		}
	
	protected:
	
		const LocalProjectionProviderCPU< CameraModel >* m_pJacobian;
	
	};

}