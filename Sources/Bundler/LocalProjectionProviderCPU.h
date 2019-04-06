#pragma once

namespace Bundler {
	
	template < class CameraModel >
	class LocalProjectionProviderCPU : public ProjectionProvider< CameraModel >
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
			ProjectionProvider< CameraModel >::Initialize( cameraCount, pCameras, pointCount, pPoints, projectionCount, pProjections );
			
			m_localCameraStartIx = 0;
			m_localCameraCount = (uint)cameraCount;

			m_localPointStartIx = 0;
			m_loclaPointCount = (uint)pointCount;
		}

		void InitializeForCameras(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const uint cameraStartIx,
			__in const uint count )
		{
			pGlobalProvider->CopyTo( this );
			m_localCameraStartIx = cameraStartIx;
			m_localCameraCount = count;
			m_localPointStartIx = 0;
			m_loclaPointCount = m_pointCount;
		}

		void InitializeForPoints(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const uint pointStartIx,
			__in const uint count )
		{
			pGlobalProvider->CopyTo( this );
			m_localCameraStartIx = 0;
			m_localCameraCount = m_cameraCount;
			m_localPointStartIx = pointStartIx;
			m_loclaPointCount = count;
		}

		__forceinline uint GetCameraCount( ) const
		{
			return m_localCameraCount;
		}

		__forceinline uint GetCameraProjectionCount( __in const uint localCameraIx ) const
		{
			const uint globalCameraIx = GetGlobalCameraIndex( localCameraIx );
			return (uint)m_mapping.GetCameraProjectionCount( globalCameraIx );
		}

		__forceinline uint GetCameraProjectionIndex( __in const uint localCameraIx, __in const uint ix ) const
		{
			const uint globalCameraIx = GetGlobalCameraIndex( localCameraIx );
			return (uint)m_mapping.GetCameraProjectionIndex( globalCameraIx, ix );
		}

		__forceinline uint GetCameraIndex( __in const uint projectionIx ) const
		{
			return GetLocalCameraIndex( ELEMENT( m_pProjections, projectionIx ).cameraIndex );
		}

		__forceinline uint GetGlobalCameraIndex( __in const uint localCameraIx ) const
		{
			return m_localCameraStartIx + localCameraIx;
		}

		__forceinline uint GetPointCount( ) const
		{
			return m_loclaPointCount;
		}

		__forceinline uint GetPointProjectionCount( __in const uint localPointIx ) const
		{
			const uint globalPointIx = GetGlobalPointIndex ( localPointIx );
			return (uint)m_mapping.GetPointProjectionCount( globalPointIx );
		}

		__forceinline uint GetPointProjectionIndex( __in const uint localPointIx, __in const uint ix ) const
		{
			const uint globalPointIx = GetGlobalPointIndex ( localPointIx );
			return (uint)m_mapping.GetPointProjectionIndex( globalPointIx, ix );
		}

		__forceinline uint GetPointIndex( __in const uint projectionIx ) const
		{
			return GetLocalPointIndex( ELEMENT( m_pProjections, projectionIx ).pointIndex );
		}

		__forceinline uint GetGlobalPointIndex( __in const uint localPointIx ) const
		{
			return m_localPointStartIx + localPointIx;
		}

	protected:

		__forceinline uint GetLocalCameraIndex( __in const uint globalCameraIx ) const
		{
			return globalCameraIx - m_localCameraStartIx;
		}

		__forceinline uint GetLocalPointIndex( __in const uint globalPointIx ) const
		{
			return globalPointIx - m_localPointStartIx;
		}

	protected:

		uint m_localCameraStartIx;
		uint m_localCameraCount;

		uint m_localPointStartIx;
		uint m_loclaPointCount;

	};


}