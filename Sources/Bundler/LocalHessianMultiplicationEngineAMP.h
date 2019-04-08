#pragma once

namespace Bundler {

	template < class CameraModel >
	class LocalHessianMultiplicationEngineAMP
	{
	public:
	
		void Initialize( __in const Scalar dampeningFactor ) __GPU
		{
			m_diagonalFactor = 1 + dampeningFactor;
		}
	
		void MultiplyCameraRow(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in LocalHessianBlockProviderAMP< CameraModel >* pHessian,
			__in const uint localCameraIx,
			__in const uint totalCameraParameterCount,
			__in_ecount( totalCameraParameterCount ) const Scalar* pCameraX,
			__in const uint totalPointParameterCount,
			__in_ecount( totalPointParameterCount ) const Scalar* pPointX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pY ) const __GPU
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
	
			UNREFERENCED_PARAMETER( totalCameraParameterCount );
	
			const uint globalCameraIx = pJacobian->GetGlobalCameraIndex( localCameraIx );
			pCameraX = pCameraX + globalCameraIx * cameraParamCount;
			MultiplyByCameraBlock( pJacobian, pHessian, localCameraIx, pCameraX, pY );
	
			Scalar pointBlockAccumulator[cameraParamCount];
			MultiplyByCameraPointBlocksCam( pJacobian, pHessian localCameraIx, totalPointParameterCount, pPointX, pointBlockAccumulator );
	
			MatrixAdd< Scalar, CameraModel::cameraParameterCount, 1 >( pY, pointBlockAccumulator, pY );
		}
	
		void MultiplyPointRow(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in LocalHessianBlockProviderAMP< CameraModel >* pHessian,
			__in const uint localPointIx,
			__in const uint totalCameraParameterCount,
			__in_ecount( totalCameraParameterCount ) const Scalar* pCameraX,
			__in const uint totalPointParameterCount,
			__in_ecount( totalPointParameterCount ) const Scalar* pPointX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pY ) const __GPU
		{
			UNREFERENCED_PARAMETER( totalPointParameterCount );
	
			const uint globalPointIndex = pJacobian->GetGlobalPointIndex( localPointIx );
			pPointX = pPointX + globalPointIndex * POINT_PARAM_COUNT;
			MultiplyByPointBlock( localPointIx, pPointX, pY );
	
			Scalar cameraBlockAccumulator[POINT_PARAM_COUNT];
			MultiplyByCameraPointBlocksPt( localPointIx, totalCameraParameterCount, pCameraX, cameraBlockAccumulator );
	
			MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pY, cameraBlockAccumulator, pY );
		}
	
	protected:
	
		void MultiplyByCameraBlock(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in LocalHessianBlockProviderAMP< CameraModel >* pHessian,
			__in const uint localCameraIx,
			__in_ecount( CameraModel::cameraParameterCount ) const Scalar* pX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pY ) const __GPU
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
	
			Scalar cameraBlock[cameraParamCount * cameraParamCount];
			pHessian->GetCameraBlock( pJacobian, localCameraIx, cameraBlock );
	
			MatrixMultiplyDiagonal< Scalar, cameraParamCount >( m_diagonalFactor, cameraBlock );
			MatrixMultiply< Scalar, cameraParamCount, cameraParamCount, 1 >( cameraBlock, pX, pY );
		}
	
		void MultiplyByCameraPointBlocksCam(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in LocalHessianBlockProviderAMP< CameraModel >* pHessian,
			__in const uint localCameraIx,
			__in const uint totalPointParameterCount,
			__in_ecount( totalPointParameterCount ) const Scalar* pPointX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pAccumulator ) const __GPU
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
			const uint projectionCount = pJacobian->GetCameraProjectionCount( localCameraIx );
	
			ByteFill< Scalar >( 0, cameraParamCount, pAccumulator );
			Scalar cameraPointBlock[cameraParamCount * POINT_PARAM_COUNT];
			Scalar tempAccumulator[cameraParamCount];
	
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				uint localPointIndex = 0;
				pHessian->GetCameraPointBlockCam( pJacobian, localCameraIx, projI, &localPointIndex, cameraPointBlock );
	
				const uint globalPointIndex = pJacobian->GetGlobalPointIndex( localPointIndex );
				const Scalar* pXSource = pPointX + globalPointIndex * POINT_PARAM_COUNT;
	
				MatrixMultiply< Scalar, cameraParamCount, POINT_PARAM_COUNT, 1 >( cameraPointBlock, pXSource, tempAccumulator );
				MatrixAdd< Scalar, cameraParamCount, 1 >( pAccumulator, tempAccumulator, pAccumulator );
			}
		}
	
		void MultiplyByPointBlock(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in LocalHessianBlockProviderAMP< CameraModel >* pHessian,
			__in const uint pointIx,
			__in_ecount( POINT_PARAM_COUNT ) const Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pY ) const __GPU
		{
			Scalar pointBlock[POINT_PARAM_COUNT * POINT_PARAM_COUNT];
			pHessian->GetPointBlock( pJacobian, pointIx, pointBlock );
	
			MatrixMultiplyDiagonal< Scalar, POINT_PARAM_COUNT >( m_diagonalFactor, pointBlock );
			MatrixMultiply< Scalar, POINT_PARAM_COUNT, POINT_PARAM_COUNT, 1 >( pointBlock, pX, pY );
		}
	
		void MultiplyByCameraPointBlocksPt(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in LocalHessianBlockProviderAMP< CameraModel >* pHessian,
			__in const uint localPointIx,
			__in const uint totalCameraParameterCount,
			__in_ecount( totalCameraParameterCount ) const Scalar* pCameraX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pAccumulator ) const __GPU
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
			const uint projectionCount = pJacobian->GetPointProjectionCount( localPointIx );
	
			ByteFill< Scalar >( 0, POINT_PARAM_COUNT, pAccumulator );
			Scalar cameraPointBlock[cameraParamCount * POINT_PARAM_COUNT];
			Scalar tempAccumulator[POINT_PARAM_COUNT];
	
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				uint localCameraIndex = 0;
				pHessian->GetCameraPointBlockPt( pJacobian, localPointIx, projI, &localCameraIndex, cameraPointBlock );
	
				const uint globalCameraIndex = pJacobian->GetGlobalCameraIndex( localCameraIndex );
				const Scalar* pXSource = pCameraX + globalCameraIndex * cameraParamCount;
	
				MatrixMultiplyAtB< Scalar, cameraParamCount, POINT_PARAM_COUNT, 1 >( cameraPointBlock, pXSource, tempAccumulator );
				MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pAccumulator, tempAccumulator, pAccumulator );
			}
		}
	
	protected:
	
		Scalar m_diagonalFactor;
	
	};

}