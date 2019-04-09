#pragma once

namespace Bundler {
	
	template < class CameraModel >
	class LocalProjectionProviderCPU 
	{
	public:

		void InitializeForCameras(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const uint cameraStartIx,
			__in const uint count )
		{
			m_localCameraStartIx = cameraStartIx;
			m_localCameraCount = count;

			m_localPointStartIx = 0;
			m_localPointCount = ( uint )pGlobalProvider->GetPointCount( );

			m_pProvider = pGlobalProvider;
		}

		void InitializeForPoints(
			__in const ProjectionProvider< CameraModel >* pGlobalProvider,
			__in const uint pointStartIx,
			__in const uint count )
		{
			m_localCameraStartIx = 0;
			m_localCameraCount = ( uint )pGlobalProvider->GetCameraCount( );

			m_localPointStartIx = pointStartIx;
			m_localPointCount = count;

			m_pProvider = pGlobalProvider;
		}

		__forceinline uint GetCameraCount( ) const
		{
			return m_localCameraCount;
		}

		__forceinline uint GetCameraProjectionCount( __in const uint localCameraIx ) const
		{
			const uint globalCameraIx = GetGlobalCameraIndex( localCameraIx );
			return ( uint )m_pProvider->GetCameraProjectionCount( globalCameraIx );
		}

		__forceinline uint GetCameraProjectionIndex( __in const uint localCameraIx, __in const uint ix ) const
		{
			const uint globalCameraIx = GetGlobalCameraIndex( localCameraIx );
			return ( uint )m_pProvider->GetCameraProjectionIndex( globalCameraIx, ix );
		}

		__forceinline uint GetCameraIndex( __in const uint projectionIx ) const
		{
			const uint globalCameraIndex = ( uint )m_pProvider->GetCameraIndex( projectionIx );
			return GetLocalCameraIndex( globalCameraIndex );
		}

		__forceinline uint GetGlobalCameraIndex( __in const uint localCameraIx ) const
		{
			return m_localCameraStartIx + localCameraIx;
		}


		__forceinline uint GetPointCount( ) const
		{
			return m_localPointCount;
		}

		__forceinline uint GetPointProjectionCount( __in const uint localPointIx ) const
		{
			const uint globalPointIx = GetGlobalPointIndex ( localPointIx );
			return ( uint )m_pProvider->GetPointProjectionCount( globalPointIx );
		}

		__forceinline uint GetPointProjectionIndex( __in const uint localPointIx, __in const uint ix ) const
		{
			const uint globalPointIx = GetGlobalPointIndex ( localPointIx );
			return (uint)m_pProvider->GetPointProjectionIndex( globalPointIx, ix );
		}

		__forceinline uint GetPointIndex( __in const uint projectionIx ) const
		{
			const uint globalPointIndex = (uint)m_pProvider->GetPointIndex( projectionIx );
			return GetLocalPointIndex( globalPointIndex );
		}

		__forceinline uint GetGlobalPointIndex( __in const uint localPointIx ) const
		{
			return m_localPointStartIx + localPointIx;
		}

		template < bool getCameraBlock, bool getPointBlock, bool getResiduals >
		__forceinline void GetProjectionBlock(
			__in const size_t projectionIx,
			__out_ecount_opt( 2 * CameraModel::cameraParameterCount ) Scalar* pCameraBlock,
			__out_ecount_opt( 2 * POINT_PARAM_COUNT ) Scalar* pPointBlock,
			__out_ecount_opt( 2 ) Scalar* pResiduals ) const
		{
			m_pProvider->GetProjectionBlock< getCameraBlock, getPointBlock, getResiduals >( projectionIx, pCameraBlock, pPointBlock, pResiduals );
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
		uint m_localPointCount;

		const ProjectionProvider< CameraModel >* m_pProvider;

	};


}