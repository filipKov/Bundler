#pragma once

namespace Bundler {

	template < class CameraModel >
	class LocalProjectionProvider;

	template < class CameraModel >
	class ProjectionProvider
	{
	public:

		void Initialize(
			__in const size_t cameraCount,
			__in const CameraModel* pCameras,
			__in const size_t pointCount,
			__in const Vector3* pPoints,
			__in const size_t projectionCount,
			__in const Projection* pProjections )
		{
			m_cameraCount = cameraCount;
			m_pCameras = pCameras;

			m_pointCount = pointCount;
			m_pPoints = pPoints;

			m_projectionCount = projectionCount;
			m_pProjections = pProjections;

			m_mapping.Initialize( cameraCount, pointCount, projectionCount, pProjections );
		}

		size_t GetCameraCount() const
		{
			return m_cameraCount;
		}

		size_t GetCameraProjectionCount( __in const size_t cameraIx ) const
		{
			return m_mapping.GetCameraProjectionCount( cameraIx );
		}

		size_t GetCameraProjectionIndex( __in const size_t cameraIx, __in const size_t ix ) const
		{
			return m_mapping.GetCameraProjectionIndex( cameraIx, ix );
		}

		size_t GetCameraIndex( __in const size_t projectionIx ) const
		{
			return ELEMENT( m_pProjections, projectionIx ).cameraIndex;
		}


		size_t GetPointCount() const
		{
			return m_pointCount;
		}

		size_t GetPointProjectionCount( __in const size_t pointIx ) const
		{
			return m_mapping.GetPointProjectionCount( pointIx );
		}

		size_t GetPointProjectionIndex( __in const size_t pointIx, __in const size_t ix ) const
		{
			return m_mapping.GetPointProjectionIndex( pointIx, ix );
		}

		size_t GetPointIndex( __in const size_t projectionIx ) const
		{
			return ELEMENT( m_pProjections, projectionIx ).pointIndex;
		}

		size_t GetProjectionCount( ) const
		{
			return m_projectionCount;
		}


		template < bool getCameraBlock, bool getPointBlock, bool getResiduals >
		void GetProjectionBlock(
			__in const size_t projectionIx,
			__out_ecount_opt( 2 * CameraModel::cameraParameterCount ) Scalar* pCameraBlock,
			__out_ecount_opt( 2 * POINT_PARAM_COUNT ) Scalar* pPointBlock,
			__out_ecount_opt( 2 ) Scalar* pResiduals ) const
		{
			DScalar< CameraModel::totalParamCount > residuals[ 2 ];
			GetProjection( projectionIx, residuals );

			if ( getCameraBlock )
			{
				_ASSERT_EXPR( pCameraBlock != NULL, "Camera block must not be null" );
				ExtractCameraParameters( residuals + 0, pCameraBlock + 0 );
				ExtractCameraParameters( residuals + 1, pCameraBlock + CameraModel::cameraParameterCount );
			}

			if ( getPointBlock )
			{
				_ASSERT_EXPR( pPointBlock != NULL, "Point block must not be null" );
				ExtractPointParameters( residuals + 0, pPointBlock + 0 );
				ExtractPointParameters( residuals + 1, pPointBlock + POINT_PARAM_COUNT );
			}

			if ( getResiduals )
			{
				_ASSERT_EXPR( pResiduals != NULL, "Residual block must not be null" );
				ELEMENT( pResiduals, 0 ) = ELEMENT( residuals, 0 ).GetFx();
				ELEMENT( pResiduals, 1 ) = ELEMENT( residuals, 1 ).GetFx();
			}
		}

	protected:

		void GetProjection(
			__in const size_t projectionIx,
			__out_ecount( 2 ) DScalar< CameraModel::totalParamCount >* pResiduals ) const
		{
			const Projection* pProjection = m_pProjections + projectionIx;

			const uint cameraIndex = pProjection->cameraIndex;
			const uint pointIndex = pProjection->pointIndex;

			ELEMENT( m_pCameras, cameraIndex ).ProjectPoint( ELEMENT( m_pPoints, pointIndex ).Elements(), pProjection->projectedPoint.Elements(), pResiduals );
		}

		inline void ExtractCameraParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const
		{
			ShallowCopy< Scalar >(
				pResidual->GetDiff().Elements(),
				CameraModel::cameraParameterCount,
				pDestination );
		}

		inline void ExtractPointParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const
		{
			ShallowCopy< Scalar >(
				pResidual->GetDiff().Elements() + CameraModel::pointParamStartIx,
				POINT_PARAM_COUNT,
				pDestination );
		}

	protected:

		size_t m_cameraCount;
		size_t m_pointCount;
		size_t m_projectionCount;

		const CameraModel* m_pCameras;
		const Vector3* m_pPoints;
		const Projection* m_pProjections;

		Structure::BundleStructureMapping m_mapping;

		friend class LocalProjectionProvider< CameraModel >;
	};

}