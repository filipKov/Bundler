#pragma once

namespace Bundler { namespace LinearSolver {

#define PRECONDITIONER_NO_CHECK false

	template < class CameraModel >
	class JacobiPreconditioner : public Preconditioner< CameraModel >
	{
	public:

		void Apply(
			__in const HessianBlockProvider< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pPreconditionedX ) const override
		{
			const size_t cameraCount = pHessian->GetCameraCount();
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			Scalar cameraBlock[ cameraParamCount * cameraParamCount ];
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				pHessian->GetCameraBlock( cameraIx, cameraBlock );

				ApplyPreconditionerBlock< cameraParamCount >( 
					cameraBlock,
					Utils::GetCameraParamPtr< CameraModel >( cameraIx, pX ),
					Utils::GetCameraParamPtr< CameraModel >( cameraIx, pPreconditionedX )
				);
			}

			const size_t pointCount = pHessian->GetPointCount();

			Scalar pointBlock[ POINT_PARAM_COUNT * POINT_PARAM_COUNT ];
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
			__in_ecount( blockSize * blockSize ) const Scalar* pBlock,
			__in_ecount( blockSize ) const Scalar* pVector,
			__out_ecount( blockSize ) Scalar* pPreconditionedVector ) const
		{
			for ( uint i = 0; i < blockSize; i++ )
			{
			#if PRECONDITIONER_NO_CHECK
				ELEMENT( pPreconditionedVector, i ) = ( Scalar( 1 ) / ELEMENT( pBlock, i * blockSize + i ) ) * ELEMENT( pVector, i );
			#else
				ELEMENT( pPreconditionedVector, i ) = ELEMENT( pVector, i );
				Scalar diagElement = ELEMENT( pBlock, i * blockSize + i );
				if ( diagElement != Scalar( 0 ) )
				{
					ELEMENT( pPreconditionedVector, i ) *= Scalar( 1 ) / diagElement;
				}
			#endif // PRECONDITIONER_NO_CHECK

				// Or should we add some eps to diagonal element??
			}
		}

	};

} }