#pragma once

namespace Bundler {

	template < class CameraModel >
	class LocalHessianMultiplicationEngineCPU
	{
	public:
	
		void Initialize( __in const LocalProjectionProviderCPU< CameraModel >* pProjectionProvider, __in const Scalar dampeningFactor )
		{
			m_hessian.Initialize( pProjectionProvider );
			m_diagonalFactor = 1 + dampeningFactor;
		}
	
		__forceinline uint GetCameraCount( ) const
		{
			return m_hessian.GetCameraCount( );
		}
	
		__forceinline uint GetPointCount( ) const
		{
			return m_hessian.GetPointCount( );
		}
	
		void MultiplyCameraRow(
			__in const uint localCameraIx,
			__in const uint totalCameraParameterCount,
			__in_ecount( totalCameraParameterCount ) const Scalar* pCameraX,
			__in const uint totalPointParameterCount,
			__in_ecount( totalPointParameterCount ) const Scalar* pPointX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pY ) const
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			UNREFERENCED_PARAMETER( totalCameraParameterCount );

			const uint globalCameraIx = m_hessian.GetGlobalCameraIndex( localCameraIx );
			pCameraX = pCameraX + globalCameraIx * cameraParamCount;
			MultiplyByCameraBlock( localCameraIx, pCameraX, pY );
	
			Scalar pointBlockAccumulator[cameraParamCount];
			MultiplyByCameraPointBlocksCam( localCameraIx, totalPointParameterCount, pPointX, pointBlockAccumulator );
	
			MatrixAdd< Scalar, CameraModel::cameraParameterCount, 1 >( pY, pointBlockAccumulator, pY );
		}
	
		void MultiplyPointRow(
			__in const uint localPointIx,
			__in const uint totalCameraParameterCount,
			__in_ecount( totalCameraParameterCount ) const Scalar* pCameraX,
			__in const uint totalPointParameterCount,
			__in_ecount( totalPointParameterCount ) const Scalar* pPointX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pY ) const
		{
			UNREFERENCED_PARAMETER( totalPointParameterCount );

			const uint globalPointIndex = m_hessian.GetGlobalPointIndex( localPointIx );
			pPointX = pPointX + globalPointIndex * POINT_PARAM_COUNT;
			MultiplyByPointBlock( localPointIx, pPointX, pY );
	
			Scalar cameraBlockAccumulator[POINT_PARAM_COUNT];
			MultiplyByCameraPointBlocksPt( localPointIx, totalCameraParameterCount, pCameraX, cameraBlockAccumulator );
	
			MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pY, cameraBlockAccumulator, pY );
		}
		
	protected:
	
		void MultiplyByCameraBlock(
			__in const uint localCameraIx,
			__in_ecount( CameraModel::cameraParameterCount ) const Scalar* pX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pY ) const
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
	
			Scalar cameraBlock[cameraParamCount * cameraParamCount];
			m_hessian.GetCameraBlock( localCameraIx, cameraBlock );
	
			MatrixMultiplyDiagonal< Scalar, cameraParamCount >( m_diagonalFactor, cameraBlock );
			MatrixMultiply< Scalar, cameraParamCount, cameraParamCount, 1 >( cameraBlock, pX, pY );
		}
	
		void MultiplyByCameraPointBlocksCam(
			__in const uint localCameraIx,
			__in const uint totalPointParameterCount,
			__in_ecount( totalPointParameterCount ) const Scalar* pPointX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pAccumulator ) const
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
			const uint projectionCount = m_hessian.GetCameraProjectionCount( localCameraIx );
	
			ByteFill< Scalar >( 0, cameraParamCount, pAccumulator );
			Scalar cameraPointBlock[cameraParamCount * POINT_PARAM_COUNT];
			Scalar tempAccumulator[cameraParamCount];
	
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				uint localPointIndex = 0;
				m_hessian.GetCameraPointBlockCam( localCameraIx, projI, &localPointIndex, cameraPointBlock );
	
				const uint globalPointIndex = m_hessian.GetGlobalPointIndex( localPointIndex );
				const Scalar* pXSource = pPointX + globalPointIndex * POINT_PARAM_COUNT;
	
				MatrixMultiply< Scalar, cameraParamCount, POINT_PARAM_COUNT, 1 >( cameraPointBlock, pXSource, tempAccumulator );
				MatrixAdd< Scalar, cameraParamCount, 1 >( pAccumulator, tempAccumulator, pAccumulator );
			}
		}
	
		void MultiplyByPointBlock(
			__in const uint pointIx,
			__in_ecount( POINT_PARAM_COUNT ) const Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pY ) const
		{
			Scalar pointBlock[POINT_PARAM_COUNT * POINT_PARAM_COUNT];
			m_hessian.GetPointBlock( pointIx, pointBlock );
	
			MatrixMultiplyDiagonal< Scalar, POINT_PARAM_COUNT >( m_diagonalFactor, pointBlock );
			MatrixMultiply< Scalar, POINT_PARAM_COUNT, POINT_PARAM_COUNT, 1 >( pointBlock, pX, pY );
		}
	
		void MultiplyByCameraPointBlocksPt(
			__in const uint localPointIx,
			__in const uint totalCameraParameterCount,
			__in_ecount( totalCameraParameterCount ) const Scalar* pCameraX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pAccumulator ) const
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
			const uint projectionCount = m_hessian.GetPointProjectionCount( localPointIx );
	
			ByteFill< Scalar >( 0, POINT_PARAM_COUNT, pAccumulator );
			Scalar cameraPointBlock[cameraParamCount * POINT_PARAM_COUNT];
			Scalar tempAccumulator[POINT_PARAM_COUNT];
	
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				uint localCameraIndex = 0;
				m_hessian.GetCameraPointBlockPt( localPointIx, projI, &localCameraIndex, cameraPointBlock );
	
				const uint globalCameraIndex = m_hessian.GetGlobalCameraIndex( localCameraIndex );
				const Scalar* pXSource = pCameraX + globalCameraIndex * cameraParamCount;
	
				MatrixMultiplyAtB< Scalar, cameraParamCount, POINT_PARAM_COUNT, 1 >( cameraPointBlock, pXSource, tempAccumulator );
				MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pAccumulator, tempAccumulator, pAccumulator );
			}
		}
	
	protected:
	
		LocalHessianBlockProviderCPU< CameraModel > m_hessian;
	
		Scalar m_diagonalFactor;
	
	};

}