#pragma once

namespace Bundler {

	// TODO: add Levenberg-Marquardt dampening factor

	template < class CameraModel >
	class HessianMultiplicationEngine
	{
	public:

		void Initialize( __in const ProjectionProvider< CameraModel >* pProjectionProvider )
		{
			m_pHessian->Initialize( pProjectionProvider );
		}

		inline size_t GetCameraCount() const
		{
			return m_pHessian->GetCameraCount();
		}

		inline size_t GetPointCount() const
		{
			return m_pHessian->GetPointCount();
		}

		void MultiplyCameraRow(
			__in const size_t cameraIx,
			__in const size_t totalParameterCount,
			__in_ecount( totalParameterCount ) const Scalar* pX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pY )
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			MultiplyByCameraBlock( cameraIx, pX + cameraIx * cameraParamCount, pY );

			Scalar pointBlockAccumulator[ cameraParamCount ];
			MultiplyByCameraPointBlocksCam( cameraIx, totalParameterCount, pX, pointBlockAccumulator );
			
			MatrixAdd< Scalar, CameraModel::cameraParameterCount, 1 >( pY, pointBlockAccumulator, pY );
		}

		void MultiplyPointRow(
			__in const size_t pointIx,
			__in const size_t totalParameterCount,
			__in_ecount( totalParameterCount ) const Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pY )
		{
			const Scalar* pPointXSource = pX + GetCameraCount() * CameraModel::cameraParameterCount + pointIx * POINT_PARAM_COUNT;
			MultiplyByPointBlock( pointIx, pPointXSource, pY );

			Scalar cameraBlockAccumulator[ POINT_PARAM_COUNT ];
			MultiplyByCameraPointBlockPt( pointIx, totalParameterCount, pX, cameraBlockAccumulator );

			MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pY, cameraBlockAccumulator, pY );
		}

	protected:

		void MultiplyByCameraBlock(
			__in const size_t cameraIx,
			__in_ecount( CameraModel::cameraParameterCount ) const Scalar* pX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pY ) 
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			Scalar cameraBlock[ cameraParamCount * cameraParamCount ];
			m_pHessian->GetCameraBlock( cameraIx, cameraBlock );

			MatrixMultiply< Scalar, cameraParamCount, cameraParamCount, 1 >( cameraBlock, pX, pY );
		}

		void MultiplyByCameraPointBlocksCam(
			__in const size_t cameraIx,
			__in const size_t totalParameterCount,
			__in_ecount( totalParameterCount ) const Scalar* pX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pAccumulator )
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
			const size_t projectionCount = m_pHessian->GetCameraProjectionCount( cameraIx );

			ByteFill< Scalar >( 0, cameraParamCount, pAccumulator );
			Scalar cameraPointBlock[ cameraParamCount * POINT_PARAM_COUNT ];
			Scalar tempAccumulator[ cameraParamCount ];

			for ( size_t projI = 0; projI < projectionCount; projI++ )
			{
				size_t pointIndex = 0;
				m_pHessian->GetCameraPointBlockCam( cameraix, projI, &pointIndex, cameraPointBlock );

				const Scalar* pXSource = pX + GetCameraCount() * cameraParamCount + pointIndex * POINT_PARAM_COUNT;

				MatrixMultiply< Scalar, cameraParamCount, POINT_PARAM_COUNT, 1 >( cameraPointBlock, pXSource, tempAccumulator );
				MatrixAdd< Scalar, cameraParamCount, 1 >( pAccumulator, tempAccumulator, pAccumulator );
			}
		}

		void MultiplyByPointBlock(
			__in const size_t pointIx,
			__in_ecount( POINT_PARAM_COUNT ) const Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pY )
		{
			Scalar pointBlock[ POINT_PARAM_COUNT * POINT_PARAM_COUNT ];
			m_pHessian->GetPointBlock( pointIx, pointBlock );

			MatrixMultiply< Scalar, POINT_PARAM_COUNT, POINT_PARAM_COUNT, 1 >( pointBlock, pX, pY );
		}

		void MultiplyByCameraPointBlocksPt(
			__in const size_t pointIx,
			__in const size_t totalParameterCount,
			__in_ecount( totalParameterCount ) const Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pAccumulator )
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
			const size_t projectionCount = m_pHessian->GetPointProjectionCount( pointIx );

			ByteFill< Scalar >( 0, POINT_PARAM_COUNT, pAccumulator );
			Scalar cameraPointBlock[ cameraParamCount * POINT_PARAM_COUNT ];
			Scalar tempAccumulator[ POINT_PARAM_COUNT ];

			for ( size_t projI = 0; projI < projectionCount; projI++ )
			{
				size_t cameraIndex = 0;
				m_pHessian->GetCameraPointBlockPt( pointIx, projI, &cameraIndex, cameraPointBlock );

				const Scalar* pXSource = pX + cameraIndex * cameraParamCount;
			
				MatrixMultiplyAtB< Scalar, cameraParamCount, POINT_PARAM_COUNT, 1 >( cameraPointBlock, pXSource, tempAccumulator );
				MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pAccumulator, tempAccumulator, pAccumulator );
			}
		}

	protected:

		const HessianBlockProvider< CameraModel >* m_pHessian;

	};

}