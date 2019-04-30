#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class BlockJacobiPreconditionerOpenMP
	{
	public:
	
		static void Apply(
			__in const HessianBlockProvider< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pPreconditionedX )
		{
			UNREFERENCED_PARAMETER( vectorSize );
	
			const int64 cameraCount = pHessian->GetCameraCount( );
			const int64 pointCount = pHessian->GetPointCount( );
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
	
			#pragma omp parallel 
			{
				Scalar cameraBlock[cameraParamCount * cameraParamCount];
				Scalar pointBlock[POINT_PARAM_COUNT * POINT_PARAM_COUNT];
	
				#pragma omp for nowait
				for ( int64 cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
				{
					pHessian->GetCameraBlock( cameraIx, cameraBlock );
	
					ApplyPreconditionerBlock< cameraParamCount >(
						cameraBlock,
						Utils::GetCameraParamPtr< CameraModel >( cameraIx, pX ),
						Utils::GetCameraParamPtr< CameraModel >( cameraIx, pPreconditionedX )
						);
				}
	
				#pragma omp for
				for ( int64 pointIx = 0; pointIx < pointCount; pointIx++ )
				{
					pHessian->GetPointBlock( pointIx, pointBlock );
	
					ApplyPreconditionerBlock< POINT_PARAM_COUNT >(
						pointBlock,
						Utils::GetPointParamPtr< CameraModel >( pointIx, cameraCount, pX ),
						Utils::GetPointParamPtr< CameraModel >( pointIx, cameraCount, pPreconditionedX )
						);
				}
			}
		}
	
	protected:
	
		template < uint blockSize >
		inline static void ApplyPreconditionerBlock(
			__inout_ecount( blockSize * blockSize ) Scalar* pBlock,
			__in_ecount( blockSize ) const Scalar* pVector,
			__out_ecount( blockSize ) Scalar* pPreconditionedVector )
		{
			Scalar invertedBlock[blockSize * blockSize];
	
			MatrixInvertGJ< Scalar, blockSize >( pBlock, invertedBlock );
			MatrixMultiply< Scalar, blockSize, blockSize, 1 >( invertedBlock, pVector, pPreconditionedVector );
		}
	
		template <>
		inline static void ApplyPreconditionerBlock< 3 >(
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
