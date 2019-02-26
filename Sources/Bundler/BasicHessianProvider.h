#pragma once

namespace Bundler {

	template < class CameraModel >
	class BasicHessianProvider : public IHessianProvider< CameraModel >
	{
		void Initialize( __in IJacobianProvider< CameraModel >* pJacobianProvider ) 
		{
			m_pJacobianProvider = pJacobianProvider;
		}

		void GetCameraCount() const override {
			return m_pJacobianProvider->GetCameraCount();
		}

		void GetPointCount() const override {
			return m_pJacobianProvider->GetPointCount();
		}


		void GetCameraBlocks(
			__in const size_t cameraIx,
			__deref_out HessianCameraBlock** ppCamBlock,
			__out size_t* pCamPointBlockCount,
			__deref_out size_t** ppPointIndices,
			__deref_out HessianCameraPointBlock** ppCamPtBlocks ) override 
		{
			const size_t projectionCount = m_pJacobianProvider->GetCameraProjectionCount( cameraIx );
			*pCamPointBlockCount = projectionCount;

			m_cameraPtBlocks.EnsureLength( projectionCount );

			for ( size_t i = 0; i < projectionCount; i++ ) 
			{
				size_t camIx;
				size_t ptIx;
				DScalar< CameraModel::totalParamCount > residuals[ 2 ];

				m_pJacobianProvider->GetProjection( m_pJacobianProvider->GetCameraProjectionIndex( cameraIx i ), &camIx, &ptIx, residuals );

				// accumulate camera block


				// calculate camera-point block
				MatrixMultiply< Scalar, CameraModel::totalParamCount, 2, POINT_PARAM_COUNT >( ..., ..., m_cameraPtBlocks[ i ].Elements() );
			}

			// calculate camera block: Nx|C| x |C|xN matrix multiplication
		}

		void GetPointBlock(
			__in const size_t pointIx,
			__deref_out HessianPointBlock** ppPtBlock ) override
		{
			const size_t projectionCount m_pJacobianProvider->GetPointProjectionCount( pointIx );

			for ( size_t i = 0; i < projectionCount; i++ ) {
				// accumulate 
			}

			// calculate block: Nx3 x 3xN matrix multiplication
		}

	protected:

		IJacobianProvider< CameraModel >* m_pJacobianProvider;

		HessianCameraBlock m_cameraBlock;
		PagedVector< HessianCameraPointBlock, 13 > m_cameraPtBlocks;
		HessianPointBlock m_pointBlock;

	};

}