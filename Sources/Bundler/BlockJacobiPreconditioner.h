#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class BlockJacobiPreconditioner : public Preconditioner< CameraModel >
	{
	public:

		void Apply(
			__in const HessianBlockProvider< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pPreconditionedX ) const override
		{
			const size_t cameraCount = pHessian->GetCameraCount( );
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			Scalar cameraBlock[cameraParamCount * cameraParamCount];
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				pHessian->GetCameraBlock( cameraIx, cameraBlock );

				ApplyPreconditionerBlock< cameraParamCount >(
					cameraBlock,
					Utils::GetCameraParamPtr< CameraModel >( cameraIx, pX ),
					Utils::GetCameraParamPtr< CameraModel >( cameraIx, pPreconditionedX )
					);
			}

			const size_t pointCount = pHessian->GetPointCount( );

			Scalar pointBlock[POINT_PARAM_COUNT * POINT_PARAM_COUNT];
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				pHessian->GetPointBlock( pointIx, pointBlock );

				ApplyPreconditionerBlock< POINT_PARAM_COUNT >(
					pointBlock,
					Utils::GetPointParamPtr< CameraModel >( pointIx, cameraCount, pX ),
					Utils::GetPointParamPtr< CameraModel >( pointIx, cameraCount, pPreconditionedX )
					);
			}
		}
	
	protected:

		template < uint blockSize >
		inline void ApplyPreconditionerBlock(
			__inout_ecount( blockSize * blockSize ) Scalar* pBlock,
			__in_ecount( blockSize ) const Scalar* pVector,
			__out_ecount( blockSize ) Scalar* pPreconditionedVector ) const
		{
			Scalar invertedBlock[blockSize * blockSize];

			MatrixInvertGJ< Scalar, blockSize >( pBlock, invertedBlock );
			MatrixMultiply< Scalar, blockSize, blockSize, 1 >( invertedBlock, pVector, pPreconditionedVector );
		}

		template <>
		inline void ApplyPreconditionerBlock< 3 >(
			__inout_ecount( blockSize * blockSize ) Scalar* pBlock,
			__in_ecount( blockSize ) const Scalar* pVector,
			__out_ecount( blockSize ) Scalar* pPreconditionedVector ) const
		{
			Scalar invertedBlock[9];

			M33Inverse( pBlock, invertedBlock );
			M33MulV3( invertedBlock, pVector, pPreconditionedVector );
		}

	};

} }