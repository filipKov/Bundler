#pragma once

namespace Bundler {

	template < class CameraModel >
	class LocalProjectionProviderAMP
	{
	public:

		LocalProjectionProviderAMP ( __in LocalProjectionProviderDataAMP< CameraModel >* pData ) __CPU_ONLY
			: m_cameras( pData->m_cameras ),
			m_points( pData->m_points ),
			m_projections( pData->m_projections ),
			m_globalMappingCameras( pData->m_globalMappingCameras ),
			m_globalMappingPoints( pData->m_globalMappingPoints ),
			m_projectionMapping( pData->m_projectionMapping )
		{
		}

		__forceinline uint GetCameraCount( ) const __GPU
		{
			return m_cameras.get_extent( )[0];
		}

		__forceinline uint GetCameraProjectionCount( __in const uint localCameraIx ) const __GPU
		{
			return m_projectionMapping[localCameraIx].projectionCount;
		}

		__forceinline uint GetCameraProjectionIndex( __in const uint localCameraIx, __in const uint ix ) const __GPU
		{
			return m_projectionMapping[localCameraIx].startIndex + ix;
		}

		__forceinline uint GetCameraIndex( __in const uint projectionIx ) const __GPU
		{
			return m_projections[projectionIx].cameraIndex;
		}

		__forceinline uint GetGlobalCameraIndex( __in const uint localCameraIx ) const __GPU
		{
			return m_globalMappingCameras[localCameraIx];
		}


		__forceinline uint GetPointCount( ) const __GPU
		{
			return m_points.get_extent( )[0];
		}

		__forceinline uint GetPointProjectionCount( __in const uint localPointIx ) const __GPU
		{
			return m_projectionMapping[localPointIx].projectionCount;
		}

		__forceinline uint GetPointProjectionIndex( __in const uint localPointIx, __in const uint ix ) const __GPU
		{
			return m_projectionMapping[localPointIx].startIndex + ix;
		}

		__forceinline uint GetPointIndex( __in const uint projectionIx ) const __GPU
		{
			return m_projections[projectionIx].pointIndex;
		}

		__forceinline uint GetGlobalPointIndex( __in const uint localPointIx ) const __GPU
		{
			return m_globalMappingPoints[localPointIx];
		}


		template < bool getCameraBlock, bool getPointBlock, bool getResiduals >
		__forceinline void GetProjectionBlock(
			__in const uint projectionIx,
			__out_ecount_opt( 2 * CameraModel::cameraParameterCount ) Scalar* pCameraBlock,
			__out_ecount_opt( 2 * POINT_PARAM_COUNT ) Scalar* pPointBlock,
			__out_ecount_opt( 2 ) Scalar* pResiduals ) const __GPU
		{
			DScalar< CameraModel::totalParamCount > residuals[2];
			GetProjection( projectionIx, residuals );

			if ( getCameraBlock )
			{
				// _ASSERT_EXPR( pCameraBlock != NULL, "Camera block must not be null" );
				ExtractCameraParameters( residuals + 0, pCameraBlock + 0 );
				ExtractCameraParameters( residuals + 1, pCameraBlock + CameraModel::cameraParameterCount );
			}

			if ( getPointBlock )
			{
				// _ASSERT_EXPR( pPointBlock != NULL, "Point block must not be null" );
				ExtractPointParameters( residuals + 0, pPointBlock + 0 );
				ExtractPointParameters( residuals + 1, pPointBlock + POINT_PARAM_COUNT );
			}

			if ( getResiduals )
			{
				// _ASSERT_EXPR( pResiduals != NULL, "Residual block must not be null" );
				ELEMENT( pResiduals, 0 ) = ELEMENT( residuals, 0 ).GetFx( );
				ELEMENT( pResiduals, 1 ) = ELEMENT( residuals, 1 ).GetFx( );
			}
		}

	protected:

		void GetProjection(
			__in const uint projectionIx,
			__out_ecount( 2 ) DScalar< CameraModel::totalParamCount >* pResiduals ) const __GPU
		{
			const Projection* pProjection = m_projections.data( ) + projectionIx;

			const uint cameraIndex = pProjection->cameraIndex;
			const uint pointIndex = pProjection->pointIndex;

			m_cameras[cameraIndex].ProjectPoint( m_points[pointIndex].Elements( ), pProjection->projectedPoint.Elements( ), pResiduals );
		}

		inline void ExtractCameraParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const __GPU
		{
			Containers::ArrayUtils< Scalar >::Copy< CameraModel::cameraParameterCount >(
				pResidual->GetDiff( ).Elements( ),
				pDestination );
		}

		inline void ExtractPointParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const __GPU
		{
			Containers::ArrayUtils< Scalar >::Copy< POINT_PARAM_COUNT >(
				pResidual->GetDiff( ).Elements( ) + CameraModel::pointParamStartIx,
				pDestination );
		}

	protected:

		// Bundle
		array< CameraModel, 1 >& m_cameras;
		array< Vector3, 1 >& m_points;
		array< Projection, 1 >& m_projections;

		// Mapping
		array< uint, 1 >& m_globalMappingCameras;
		array< uint, 1 >& m_globalMappingPoints;

		array< Structure::LocalBundleStructureMapping, 1 >& m_projectionMapping;
	};

}
