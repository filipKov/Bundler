#pragma once

namespace Bundler {

	namespace Structure {
		struct LocalBundleStructureMapping
		{
			uint startIndex;
			uint projectionCount;
		};
	}

	template < class CameraModel >
	class LocalProjectionProviderAMP
	{
	public:

		static void CreateForCameras(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const int cameraCount,
			__in_ecount( cameraCount ) const uint* pCamerasToCopy,
			__in const int pointCount,
			__in_ecount( pointCount ) const uint* pPointsToCopy,
			__in const int projectionCount,
			__deref_out LocalProjectionProviderAMP< CameraModel >** ppProvider ) __CPU_ONLY
		{
			_ASSERT_EXPR( ppProvider != NULL, "Deref to provider cannot be NULL" );

			*ppProvider = new LocalProjectionProviderAMP< CameraModel >( 
				cameraCount,  pointCount, projectionCount,  cameraCount,
				pCamerasToCopy, pPointsToCopy );

			( *ppProvider )->InitializeForCameras( pGlobalProvider );
		}

		static void CreateForPoints(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const int cameraCount,
			__in_ecount( cameraCount ) const uint* pCamerasToCopy,
			__in const int pointCount,
			__in_ecount( pointCount ) const uint* pPointsToCopy,
			__in const int projectionCount,
			__deref_out LocalProjectionProviderAMP< CameraModel >** ppProvider ) __CPU_ONLY
		{
			_ASSERT_EXPR( ppProvider != NULL, "Deref to provider cannot be NULL" );

			*ppProvider = new LocalProjectionProviderAMP< CameraModel >(
				cameraCount, pointCount, projectionCount, pointCount,
				pCamerasToCopy, pPointsToCopy );

			( *ppProvider )->InitializeForPoints( pGlobalProvider );
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
			__in const size_t projectionIx,
			__out_ecount_opt( 2 * CameraModel::cameraParameterCount ) Scalar* pCameraBlock,
			__out_ecount_opt( 2 * POINT_PARAM_COUNT ) Scalar* pPointBlock,
			__out_ecount_opt( 2 ) Scalar* pResiduals ) const __GPU
		{
			DScalar< CameraModel::totalParamCount > residuals[2];
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
				ELEMENT( pResiduals, 0 ) = ELEMENT( residuals, 0 ).GetFx( );
				ELEMENT( pResiduals, 1 ) = ELEMENT( residuals, 1 ).GetFx( );
			}
		}

	protected:

		LocalProjectionProviderAMP (
			__in const int cameraCount,
			__in const int pointCount,
			__in const int projectionCount,
			__in const int projectionMappingSize,
			__in_ecount( cameraCount ) const uint* pGlobalCameraMapping,
			__in_ecount( pointCount ) const uint* pGlobalPointMapping ) __CPU_ONLY
			: m_cameras( cameraCount ),
			  m_points( pointCount ),
			  m_projection( projectionCount ),
			  m_globalMappingCameras( cameraCount, pGlobalCameraMapping ),
			  m_globalMappingPoints( pointCount, pGlobalPointMapping ),
			  m_projectionMapping( projectionMappingSize )
		{
		}

		void InitializeForCameras( __in const ProjectionProvider< CameraModel >* pProvider ) __CPU_ONLY
		{
			// TODO
		}

		void InitializeForPoints( __in const ProjectionProvider< CameraModel >* pProvider ) __CPU_ONLY
		{
			// TODO
		}

		void GetProjection(
			__in const size_t projectionIx,
			__out_ecount( 2 ) DScalar< CameraModel::totalParamCount >* pResiduals ) const __GPU
		{
			const Projection* pProjection = m_projections.data( ) + projectionIx;

			const uint cameraIndex = pProjection->cameraIndex;
			const uint pointIndex = pProjection->pointIndex;

			m_cameraModels[cameraIndex].ProjectPoint( m_points[pointIndex].Elements( ), pProjection->projectedPoint.Elements( ), pResiduals );
		}

		inline void ExtractCameraParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const __GPU
		{
			Async::AmpArrayUtils< Scalar >::Copy< CameraModel::cameraParameterCount >(
				pResidual->GetDiff( ).Elements( ),
				pDestination );
		}

		inline void ExtractPointParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const __GPU
		{
			Async::AmpArrayUtils< Scalar >::Copy< POINT_PARAM_COUNT >(
				pResidual->GetDiff( ).Elements( ) + CameraModel::pointParamStartIx,
				pDestination );
		}

	protected:

		// Bundle
		array_view< const CameraModel, 1 > m_cameras;
		array_view< const Vector3, 1 > m_points;
		array_view< const Projection, 1 > m_projections;

		// Mapping
		array_view< const uint, 1 > m_globalMappingCameras;
		array_view< const uint, 1 > m_globalMappingPoints;

		array_view< const Structure::LocalBundleStructureMapping, 1 > m_projectionMapping;
	};

}
