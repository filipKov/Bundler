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
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pPreconditionedX )
		{
			constexpr const uint blockSize = CameraModel::cameraParameterCount;

			Scalar cameraBlock[blockSize * blockSize];
			pHessian->GetCameraBlock( localCameraIx, cameraBlock );

			ApplyPreconditionerBlock< blockSize >( cameraBlock, pX, pPreconditionedX );
		}

		static void ApplyToPoint(
			__in const LocalHessianBlockProviderCPU< CameraModel >* pHessian,
			__in const uint localPointIx,
			__in_ecount( POINT_PARAM_COUNT ) const Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pPreconditionedX )
		{
			constexpr const uint blockSize = POINT_PARAM_COUNT;

			Scalar pointBlock[blockSize * blockSize];
			pHessian->GetPointBlock( localPointIx, pointBlock );

			ApplyPreconditionerBlock< blockSize >( pointBlock, pX, pPreconditionedX );
		}

	protected:

		template < uint blockSize >
		static __forceinline void ApplyPreconditionerBlock(
			__inout_ecount( blockSize * blockSize ) Scalar* pBlock,
			__in_ecount( blockSize ) const Scalar* pVector,
			__out_ecount( blockSize ) Scalar* pPreconditionedVector )
		{
			Scalar invertedBlock[blockSize * blockSize];

			MatrixInvertGJ< Scalar, blockSize >( pBlock, invertedBlock );
			MatrixMultiply< Scalar, blockSize, blockSize, 1 >( invertedBlock, pVector, pPreconditionedVector );
		}

		template <>
		static __forceinline void ApplyPreconditionerBlock< 3 >(
			__inout_ecount( blockSize * blockSize ) Scalar* pBlock,
			__in_ecount( blockSize ) const Scalar* pVector,
			__out_ecount( blockSize ) Scalar* pPreconditionedVector )
		{
			Scalar invertedBlock[9];

			M33Inverse( pBlock, invertedBlock );
			M33MulV3( invertedBlock, pVector, pPreconditionedVector );
		}

	};

} }
