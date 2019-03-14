#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class CachedJacobiPreconditioner : public Preconditioner< CameraModel >
	{
	public:

		CachedJacobiPreconditioner()
		{
			m_cacheValid = false;
		}

		void Invalidate()
		{
			m_cacheValid = false;
		}

		void Apply(
			__in const HessianBlockProvider< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pPreconditionedX ) const override
		{
			if ( !m_cacheValid )
			{
				GetPreconditioningVector( pHessian, vectorSize );
				m_cacheValid = true;
			}

			MatrixElementWiseMultiply( vectorSize, 1, m_cache.Elements(), pX, pPreconditionedX );
		}

	protected:

		void GetPreconditioningVector(
			__in const HessianBlockProvider< CameraModel >* pHessian,
			__in const size_t vectorSize )
		{
			m_cache.ResizeTo( vectorSize, 1 );

			Scalar* pCacheDst = m_cache.Elements();

			const size_t cameraCount = pHessian->GetCameraCount();
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			Scalar cameraBlock[ cameraParamCount * cameraParamCount ];
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				pHessian->GetCameraBlock( cameraIx, cameraBlock );
				pCacheDst = ExtractInvertedDiagonal< cameraParamCount >( cameraBlock, pCacheDst );
			}

			const size_t pointCount = pHessian->GetPointCount();

			Scalar pointBlock[ POINT_PARAM_COUNT * POINT_PARAM_COUNT ];
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				pHessian->GetPointBlock( pointIx, pointBlock );
				pCacheDst = ExtractInvertedDiagonal< POINT_PARAM_COUNT >( pointBlock, pCacheDst );
			}
		}

		template < uint blockSize >
		inline Scalar* ExtractInvertedDiagonal(
			__in_ecount( blockSize * blockSize ) const Scalar* pBlock,
			__out_ecount( blockSize ) Scalar* pDst )
		{
			for ( uint i = 0; i < blockSize; i++ )
			{
#if PRECONDITIONER_SAFE
				ELEMENT( pDst, i ) = Scalar( 1 );
				Scalar diagElement = ELEMENT( pBlock, i * blockSize + i );
				if ( diagElement != Scalar( 0 ) )
				{
					ELEMENT( pDst, i ) /= diagElement;
				}
#else
				ELEMENT( pDst, i ) = Scalar( 1 ) / ELEMENT( pBlock, i * blockSize + i );
#endif // PRECONDITIONER_SAFE
			}

			return pDst += blockSize;
		}

	protected:

		Vector< Scalar > m_cache;
		bool m_cacheValid;

	};

} }
