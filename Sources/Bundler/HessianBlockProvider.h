#pragma once

namespace Bundler {

	

	template < class CameraModel >
	class HessianBlockProvider
	{
	public:

		void Initialize( __in const ProjectionProvider< CameraModel >* pProjectionProvider )
		{
			m_pJacobian = pProjectionProvider;
		}

		inline size_t GetCameraCount() const
		{
			return m_pJacobian->GetCameraCount();
		}
		
		inline size_t GetCameraProjectionCount( __in const size_t cameraIx ) const
		{
			return m_pJacobian->GetCameraProjectionCount( cameraIx );
		}

		inline size_t GetPointCount() const
		{
			return m_pJacobian->GetPointCount();
		}

		inline size_t GetPointProjectionCount( __in const size_t pointIx ) const
		{
			return m_pJacobian->GetPointProjectionCount( pointIx );
		}

		void GetCameraBlock(
			__in const size_t cameraIx,
			__out_ecount( CameraModel::cameraParameterCount * CameraModel::cameraParameterCount ) Scalar* pBlock ) const
		{
			const uint projectionCount = (uint)GetCameraProjectionCount( cameraIx );
			const uint rows = 2 * projectionCount;
			constexpr const uint columns = CameraModel::cameraParameterCount;

			Matrix< Scalar > jacobianCameraBlock( rows, columns );

			Scalar* pBlockDst = jacobianCameraBlock.Elements();
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const size_t projectionIx = m_pJacobian->GetCameraProjectionIndex( cameraIx, projI );

				m_pJacobian->GetProjectionBlock< true, false, false >(
					projectionIx, pBlockDst, NULL, NULL );

				pBlockDst += columns * 2;
			}

			MatrixMultiplyAtB(
				rows, columns, columns,
				jacobianCameraBlock.Elements(),
				jacobianCameraBlock.Elements(),
				pBlock );
		}

		void GetCameraPointBlockCam(
			__in const size_t cameraIx,
			__in const size_t ix,
			__out size_t* pPointIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const
		{
			size_t projectionIx = m_pJacobian->GetCameraProjectionIndex( cameraIx, ix );
			
			GetCameraPointBlock( projectionIx, pBlock );
			*pPointIx = m_pJacobian->GetPointIndex( projectionIx );
		}

		void GetCameraPointBlockPt(
			__in const size_t pointIx,
			__in const size_t ix,
			__out size_t* pCameraIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const
		{
			size_t projectionIx = m_pJacobian->GetPointProjectionIndex( pointIx, ix );

			GetCameraPointBlock( projectionIx, pBlock );
			*pCameraIx = m_pJacobian->GetCameraIndex( projectionIx );
		}

		void GetPointBlock(
			__in const size_t pointIx,
			__out_ecount( POINT_PARAM_COUNT * POINT_PARAM_COUNT ) Scalar* pBlock ) const
		{
			const size_t projectionCount = m_pJacobian->GetPointProjectionCount( pointIx );

			Matrix< Scalar > pointBlocks( (uint)( projectionCount * 2 ), POINT_PARAM_COUNT );
			Scalar* pBlockDst = pointBlocks.Elements();

			for ( size_t projI = 0; projI < projectionCount; projI++ )
			{
				const size_t projectionIx = m_pJacobian->GetPointProjectionIndex( pointIx, projI );

				m_pJacobian->GetProjectionBlock< false, true, false >(
					projectionIx, NULL, pBlockDst, NULL );

				pBlockDst += POINT_PARAM_COUNT * 2;
			}

			MatrixMultiplyAtB< Scalar >( 
				(uint)( projectionCount * 2 ), POINT_PARAM_COUNT, POINT_PARAM_COUNT,
				pointBlocks.Elements(), pointBlocks.Elements(), pBlock );
		}

	protected:

		void GetCameraPointBlock(
			__in const size_t projectionIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock ) const
		{
			Scalar cameraBlock[ 2 * CameraModel::cameraParameterCount ];
			Scalar pointBlock[ 2 * POINT_PARAM_COUNT ];

			m_pJacobian->GetProjectionBlock< true, true, false >( projectionIx, cameraBlock, pointBlock, NULL );

			MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, POINT_PARAM_COUNT >(
				cameraBlock, pointBlock, pBlock );
		}


	protected:

		const ProjectionProvider< CameraModel >* m_pJacobian;

	};

}