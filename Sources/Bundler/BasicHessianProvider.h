#pragma once

namespace Bundler {

	template < class CameraModel >
	class BasicHessianProvider : public IHessianProvider< CameraModel >
	{
		void Initialize( __in IJacobianProvider< CameraModel >* pJacobianProvider ) 
		{
			m_pJacobianProvider = pJacobianProvider;
		}

		size_t GetCameraCount() const override
		{
			return m_pJacobianProvider->GetCameraCount();
		}

		size_t GetCameraPointBlockCount( __in const size_t cameraIx ) const override
		{
			return m_pJacobianProvider->GetCameraProjectionCount( cameraIx );
		}

		size_t GetPointCount() const override
		{
			return m_pJacobianProvider->GetPointCount();
		}

		void GetCameraBlocks(
			__in const size_t cameraIx,
			__out HessianCameraBlock< CameraModel >* pCamBlock,
			__in const size_t camPointBlockCount,
			__out size_t* pPointIndices,
			__out HessianCameraPointBlock< CameraModel >* pCamPtBlocks,
			__out_ecount_opt( CameraModel::cameraParameterCount ) Scalar* pJtfBlock ) const override
		{
			const size_t projectionCount = m_pJacobianProvider->GetCameraProjectionCount( cameraIx );
			_ASSERT_EXPR( camPointBlockCount == projectionCount, "requested number of camera-point blocks is different than jacobian provides" );

			Matrix< Scalar > cameraBlockAccumulator( (uint)( 2 * projectionCount ), CameraModel::cameraParameterCount );
			Vector< Scalar > residualAccumulator( (uint)( 2 * projectionCount ) );

			for ( size_t i = 0; i < projectionCount; i++ ) 
			{
				size_t camIx = 0;
				size_t pointIx = 0;
				DScalar< CameraModel::totalParamCount > residuals[ 2 ];

				m_pJacobianProvider->GetProjection( m_pJacobianProvider->GetCameraProjectionIndex( cameraIx, i ), &camIx, &pointIx, residuals );

				JacobianPointBlock pointBlock;
				Scalar* pCameraBlock = cameraBlockAccumulator[ 2 * (uint)i ];

				m_pJacobianProvider->ConvertToBlocks( residuals, pCameraBlock, pointBlock );
				residualAccumulator[ 2 * (uint)i + 0 ] = -residuals[ 0 ].GetFx();
				residualAccumulator[ 2 * (uint)i + 1 ] = -residuals[ 1 ].GetFx();


				ELEMENT( pPointIndices, i ) = pointIx;
				MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, POINT_PARAM_COUNT >( pCameraBlock, pointBlock, ELEMENT( pCamPtBlocks, i ).Elements() );
			}

			MatrixMultiplyAtB( 
				(uint)( 2 * projectionCount ), 
				CameraModel::cameraParameterCount,
				CameraModel::cameraParameterCount,
				cameraBlockAccumulator.Elements(), 
				cameraBlockAccumulator.Elements(), 
				pCamBlock->Elements() 
			);

			if ( pJtfBlock )
			{
				MatrixMultiplyAtB(
					(uint)( 2*projectionCount ),
					CameraModel::cameraParameterCount
					1,
					cameraBlockAccumulator.Elements(),
					residualAccumulator.Elements(),
					pJtfBlock
				);
			}
		}

		void GetPointBlock(
			__in const size_t pointIx,
			__out HessianPointBlock* pPointBlock,
			__out_ecount_opt( POINT_PARAM_COUNT ) Scalar* pJtfBlock ) const override
		{
			const size_t projectionCount = m_pJacobianProvider->GetPointProjectionCount( pointIx );

			Matrix< Scalar > pointBlockAccumulator( (uint)( 2 * projectionCount ), POINT_PARAM_COUNT );
			Vector< Scalar > residualAccumulator( (uint)( 2 * projectionCount ) );

			for ( size_t i = 0; i < projectionCount; i++ )
			{
				size_t camIx = 0;
				size_t pointIx = 0;
				DScalar< CameraModel::totalParamCount > residuals[ 2 ];

				m_pJacobianProvider->GetProjection( m_pJacobianProvider->GetPointProjectionIndex( pointIx, i ), &camIx, &pointIx, residuals );

				Scalar* pAccumulator = pointBlockAccumulator[ 2 * (uint)i ];
				ShallowCopy< Scalar >( residuals[ 0 ].GetDiff().Elements() + CameraModel::pointParamStartIx, POINT_PARAM_COUNT, pAccumulator + 0 );
				ShallowCopy< Scalar >( residuals[ 1 ].GetDiff().Elements() + CameraModel::pointParamStartIx, POINT_PARAM_COUNT, pAccumulator + POINT_PARAM_COUNT );

				residualAccumulator[ 2 * (uint)i + 0 ] = -residuals[ 0 ].GetFx();
				residualAccumulator[ 2 * (uint)i + 1 ] = -residuals[ 1 ].GetFx();
			}

			MatrixMultiplyAtB( 
				(uint)( 2 * projectionCount ),
				POINT_PARAM_COUNT,
				POINT_PARAM_COUNT,
				pointBlockAccumulator.Elements(), 
				pointBlockAccumulator.Elements(), 
				pPointBlock->Elements() 
			);
			
			if ( pJtfBlock )
			{
				MatrixMultiplyAtB(
					(uint)( 2 * projectionCount ),
					POINT_PARAM_COUNT,
					1,
					pointBlockAccumulator.Elements(),
					residualAccumulator.Elements(),
					pJtfBlock
				);
			}
		}


	protected:

		IJacobianProvider< CameraModel >* m_pJacobianProvider;

	};

}