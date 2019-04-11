#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class LocalBlockJacobiPreconditioner
	{
	public:

		static void ApplyToCamera(
			__in const LocalHessianBlockProviderCPU< CameraModel >* pHessian,
			__in const uint localCameraIx,
			__in_ecount( CameraModel::cameraParameterCount ) const Scalar* pX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pPreconditionedX ) restrict( cpu )
		{
			constexpr const uint blockSize = CameraModel::cameraParameterCount;

			Scalar cameraBlock[blockSize * blockSize];
			pHessian->GetCameraBlock( localCameraIx, cameraBlock );

			ApplyPreconditionerBlock< blockSize >( cameraBlock, pX, pPreconditionedX );
		}

		static void ApplyToCamera(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in const LocalHessianBlockProviderAMP< CameraModel >* pHessian,
			__in const uint localCameraIx,
			__in_ecount( CameraModel::cameraParameterCount ) const Scalar* pX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pPreconditionedX ) restrict( amp )
		{
			constexpr const uint blockSize = CameraModel::cameraParameterCount;

			Scalar cameraBlock[blockSize * blockSize];
			pHessian->GetCameraBlock( pJacobian, localCameraIx, cameraBlock );

			ApplyPreconditionerBlock< blockSize >( cameraBlock, pX, pPreconditionedX );
		}

		static void ApplyToPoint(
			__in const LocalHessianBlockProviderCPU< CameraModel >* pHessian,
			__in const uint localPointIx,
			__in_ecount( POINT_PARAM_COUNT ) const Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pPreconditionedX ) restrict( cpu )
		{
			constexpr const uint blockSize = POINT_PARAM_COUNT;

			Scalar pointBlock[blockSize * blockSize];
			pHessian->GetPointBlock( localPointIx, pointBlock );

			ApplyPreconditionerBlock< blockSize >( pointBlock, pX, pPreconditionedX );
		}

		static void ApplyToPoint(
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__in const LocalHessianBlockProviderAMP< CameraModel >* pHessian,
			__in const uint localPointIx,
			__in_ecount( POINT_PARAM_COUNT ) const Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pPreconditionedX ) restrict( amp )
		{
			constexpr const uint blockSize = POINT_PARAM_COUNT;

			Scalar pointBlock[blockSize * blockSize];
			pHessian->GetPointBlock( pJacobian, localPointIx, pointBlock );

			// ApplyPreconditionerBlock< blockSize >( pointBlock, pX, pPreconditionedX ); // -> results in Error C3560 for some reason...

			Scalar invertedBlock[ blockSize * blockSize ];
			if ( blockSize == 3 )
			{
				M33Inverse( pointBlock, invertedBlock );
				M33MulV3( invertedBlock, pX, pPreconditionedX );
			}
			else
			{
				MatrixInvertGJ< Scalar, blockSize >( pointBlock, invertedBlock );
				MatrixMultiply< Scalar, blockSize, blockSize, 1 >( invertedBlock, pX, pPreconditionedX );
			}
			// hopefully, the if statement will be removed by compiler
		}

	protected:

		template < uint blockSize >
		static __forceinline void ApplyPreconditionerBlock(
			__inout_ecount( blockSize * blockSize ) Scalar* pBlock,
			__in_ecount( blockSize ) const Scalar* pVector,
			__out_ecount( blockSize ) Scalar* pPreconditionedVector ) restrict( amp, cpu )
		{
			Scalar invertedBlock[blockSize * blockSize];

			MatrixInvertGJ< Scalar, blockSize >( pBlock, invertedBlock );
			MatrixMultiply< Scalar, blockSize, blockSize, 1 >( invertedBlock, pVector, pPreconditionedVector );
		}

		template <>
		static __forceinline void ApplyPreconditionerBlock< 3 >(
			__inout_ecount( blockSize * blockSize ) Scalar* pBlock,
			__in_ecount( blockSize ) const Scalar* pVector,
			__out_ecount( blockSize ) Scalar* pPreconditionedVector ) restrict( amp, cpu )
		{
			Scalar invertedBlock[9];

			M33Inverse( pBlock, invertedBlock );
			M33MulV3( invertedBlock, pVector, pPreconditionedVector );
		}

	};

} }
