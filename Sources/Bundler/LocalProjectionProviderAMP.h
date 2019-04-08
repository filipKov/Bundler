#pragma once

namespace Bundler {

	template < class CameraModel >
	class LocalProjectionProviderAMP
	{
	public:

		LocalProjectionProviderAMP (
			__in const ProjectionProvider< CameraModel >* pProvider,
			__in const int cameraCount,
			__in_ecount( cameraCount ) const uint* pCamerasToCopy,
			__in const int pointCount,
			__in_ecount( pointCount ) const uint* pPointsToCopy,
			__in const int projectionCount ) __CPU_ONLY
			: m_cameras( cameraCount ), m_points( pointCount ), m_projections( projectionCount ),
			  m_globalMappingCameras( cameraCount, pCamerasToCopy ), m_globalMappingPoints( pointCount, pPointsToCopy )
		{
			CameraModel* pCameras = m_cameras.data( );
			Projection* pProjections = m_projections.data( );
			for ( int i = 0; i < cameraCount; i++ )
			{
				// ELEMENT( pCameras, i ) = pProvider->...;
			}

			Vector3* pPoints = m_points.data( );
			for ( int i = 0; i < pointCount; i++ )
			{
				// ELEMENT( pPoints, i ) = pProvider->...;
			}

			
			for ( int i = 0; i < projectionCount; i++ )
			{
				// ELEMENT( pProjections, i ) = pProvider->...;
			}

			
		}

		__forceinline uint GetCameraCount( ) const __GPU
		{
			return m_cameras.get_extent( )[0];
		}

		__forceinline uint GetCameraProjectionCount( __in const uint localCameraIx ) const __GPU;

		__forceinline uint GetCameraProjectionIndex( __in const uint localCameraIx, __in const uint ix ) const __GPU;

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

		__forceinline uint GetPointProjectionCount( __in const uint localPointIx ) const __GPU;

		__forceinline uint GetPointProjectionIndex( __in const uint localPointIx, __in const uint ix ) const __GPU;

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
			__out_ecount_opt( 2 ) Scalar* pResiduals ) const __GPU;

	protected:

		// Bundle
		array_view< CameraModel, 1 > m_cameras;
		array_view< Vector3, 1 > m_points;
		array_view< Projection, 1 > m_projections;

		// Mapping
		array_view< uint, 1 > m_globalMappingCameras;
		array_view< uint, 1 > m_globalMappingPoints;


	};

}
