#pragma once

namespace Bundler {

	template < class CameraModel >
	class LocalHessianBlockProviderAMP
	{
	public:
	
		LocalHessianBlockProviderAMP( __in const int tempElementCount, __in const int tempElementProjections, __in const int tempElementSize ) __CPU_ONLY 
			: m_temp( extent<2>( tempElementCount, 2 * tempElementProjections * tempElementSize ) )
		{
			m_temp.discard_data( );
		}
	
		void GetCameraBlock(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in const uint localCameraIx,
			__out_ecount( CameraModel::cameraParameterCount * CameraModel::cameraParameterCount ) Scalar* pBlock ) const __GPU
		{
			const uint projectionCount = pJacobian->GetCameraProjectionCount( localCameraIx );
			const uint rows = 2 * projectionCount;
			constexpr const uint columns = CameraModel::cameraParameterCount;
	
			Scalar* pJacobianCameraBlock = m_temp[localCameraIx].data( );
			Scalar* pBlockDst = pJacobianCameraBlock;
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = pJacobian->GetCameraProjectionIndex( localCameraIx, projI );
	
				pJacobian->GetProjectionBlock< true, false, false >( projectionIx, pBlockDst, NULL, NULL );
	
				pBlockDst += columns * 2;
			}
	
			MatrixMultiplyAtB(
				rows, columns, columns,
				pJacobianCameraBlock,
				pJacobianCameraBlock,
				pBlock );
		}
	
		void GetCameraPointBlockCam(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in const uint localCameraIx,
			__in const uint ix,
			__out uint* pLocalPointIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const __GPU
		{
			uint projectionIx = pJacobian->GetCameraProjectionIndex( localCameraIx, ix );
	
			GetCameraPointBlock( pJacobian, projectionIx, pBlock );
			*pLocalPointIx = pJacobian->GetPointIndex( projectionIx );
		}
	
		void GetCameraPointBlockPt(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in const uint localPointIx,
			__in const uint ix,
			__out uint* pLocalCameraIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const __GPU
		{
			uint projectionIx = pJacobian->GetPointProjectionIndex( localPointIx, ix );
	
			GetCameraPointBlock( pJacobian, projectionIx, pBlock );
			*pLocalCameraIx = pJacobian->GetCameraIndex( projectionIx );
		}
	
		void GetPointBlock(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in const uint localPointIx,
			__out_ecount( POINT_PARAM_COUNT * POINT_PARAM_COUNT ) Scalar* pBlock ) const __GPU
		{
			const uint projectionCount = pJacobian->GetPointProjectionCount( localPointIx );
	
			Scalar* pJacobianPointBlock = m_temp[localPointIx].data( );
			Scalar* pBlockDst = pJacobianPointBlock;
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = pJacobian->GetPointProjectionIndex( localPointIx, projI );
	
				pJacobian->GetProjectionBlock< false, true, false >( projectionIx, NULL, pBlockDst, NULL );
	
				pBlockDst += POINT_PARAM_COUNT * 2;
			}
	
			MatrixMultiplyAtB< Scalar >(
				( uint )( 2 * projectionCount ), 
				POINT_PARAM_COUNT, 
				POINT_PARAM_COUNT,
				pJacobianPointBlock, 
				pJacobianPointBlock, 
				pBlock );
		}
	
	protected:
	
		void GetCameraPointBlock(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in const uint projectionIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const __GPU
		{
			Scalar cameraBlock[2 * CameraModel::cameraParameterCount];
			Scalar pointBlock[2 * POINT_PARAM_COUNT];
	
			pJacobian->GetProjectionBlock< true, true, false >( projectionIx, cameraBlock, pointBlock, NULL );
	
			MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, POINT_PARAM_COUNT >(
				cameraBlock, pointBlock, pBlock );
		}
	
	protected:
	
		array_view< Scalar, 2 > m_temp;
	
	};

}