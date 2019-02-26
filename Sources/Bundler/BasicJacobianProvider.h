#pragma once

namespace Bundler {

	template < class CameraModel >
	class BasicJacobianProvider : public IJacobianProvider< CameraModel > {
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

			InitializeMappings();
		}

		size_t GetCameraCount() const override {
			return m_cameraCount;
		}

		size_t GetCameraProjectionCount( __in const size_t cameraIx ) const override {
			return m_cameraToProjectionMapping[ cameraIx ].Length();
		}

		size_t GetCameraProjectionIndex( __in const size_t cameraIx, __in const size_t ix ) const override {
			return m_cameraToProjectionMapping[ cameraIx ][ ix ];
		}


		size_t GetPointCount() const override {
			return m_pointCount;
		}

		size_t GetPointProjectionCount( __in const size_t pointIx ) const override {
			return m_pointToProjectionMapping[ pointIx ].Length();
		}

		size_t GetPointProjectionIndex( __in const size_t pointIx, __in const size_t ix ) const override {
			return m_pointToProjectionMapping[ pointIx ][ ix ];
		}


		void GetProjection(
			__in const size_t projectionIx,
			__out size_t* pCameraIndex,
			__out size_t* pPointIndex,
			__out_ecount( 2 ) DScalar< CameraModel::totalParamCount >* pResiduals ) const override
		{
			Projection* pProjection = m_pProjections + projectionIx;

			*pCameraIndex = pProjection->cameraIndex;
			*pPointIndex = pProjection->pointIndex;

			ELEMENT( m_pCameras, *pCameraIndex ).ProjectPoint( ELEMENT( m_pPoints, *pPointIndex ).Elements(), pProjection->projectedPoint.Elements(), pResiduals );
		}

		void ConvertToBlocks(
			__in_ecount( 2 ) const DScalar< CameraModel::totalParamCount >* pResiduals,
			__out JacobianCameraBlock< CameraModel::totalParamCount >* pCameraBlock,
			__out JacobianPointBlock* pPointBlock ) const override 
		{
			UNREFERENCED_PARAMETER( pResiduals );
			UNREFERENCED_PARAMETER( pCameraBlock );
			UNREFERENCED_PARAMETER( pPointBlock );
		}

	protected:

		void InitializeMappings() 
		{
			m_cameraToProjectionMapping.Allocate( m_cameraCount );
			m_pointToProjectionMapping.Allocate( m_pointCount );

			const Projection* pProjection = m_pProjections;
			for ( size_t projI = 0; projI < m_projectionCount; projI++ )
			{
				m_cameraToProjectionMapping[ pProjection->cameraIndex ].Add( projI );
				m_pointToProjectionMapping[ pProjection->pointIndex ].Add( projI );

				pProjection++;
			}
		}

	protected:

		size_t m_cameraCount;
		size_t m_pointCount;
		size_t m_projectionCount;

		CameraModel* m_pCameras;
		Vector3* m_pPoints;
		Projection* m_pProjections;

		Containers::Buffer< Containers::PagedVector< size_t, 13 > > m_cameraToProjectionMapping;
		Containers::Buffer< Containers::PagedVector< size_t, 5 > > m_pointToProjectionMapping;
		

	};

}