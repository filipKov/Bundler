#pragma once

namespace Bundler { namespace Utils {

	template < class CameraModel >
	inline const Scalar* GetCameraParamPtr( __in const size_t cameraIndex, __in const Scalar* pParameterVector )
	{
		return pParameterVector + cameraIndex * CameraModel::cameraParameterCount;
	}

	template < class CameraModel >
	inline Scalar* GetCameraParamPtr( __in const size_t cameraIndex, __in Scalar* pParameterVector ) 
	{
		return const_cast<Scalar*>( GetCameraParamPtr< CameraModel >( cameraIndex, (const Scalar*)pParameterVector ) );
	}


	template < class CameraModel >
	inline const Scalar* GetPointParamPtr( __in const size_t pointIndex, __in const size_t cameraCount, __in const Scalar* pParameterVector )
	{
		constexpr const size_t pointParamCount = CameraModel::totalParamCount - CameraModel::cameraParameterCount;
		return pParameterVector + cameraCount * CameraModel::cameraParameterCount + pointIndex * pointParamCount;
	}

	template < class CameraModel >
	inline Scalar* GetPointParamPtr( __in const size_t pointIndex, __in const size_t cameraCount, __in Scalar* pParameterVector )
	{
		return const_cast<Scalar*>( GetPointParamPtr< CameraModel >( pointIndex, cameraCount, (const Scalar*)pParameterVector ) );
	}


	template < class CameraModel >
	inline const Scalar* GetCameraParamPtr( __in const uint cameraIndex, __in const Scalar* pParameterVector ) __GPU_ONLY
	{
		return pParameterVector + cameraIndex * CameraModel::cameraParameterCount;
	}

	template < class CameraModel >
	inline Scalar* GetCameraParamPtr( __in const uint cameraIndex, __in Scalar* pParameterVector ) __GPU_ONLY
	{
		return const_cast<Scalar*>( GetCameraParamPtr< CameraModel >( cameraIndex, ( const Scalar* )pParameterVector ) );
	}


	template < class CameraModel >
	inline const Scalar* GetPointParamPtr( __in const uint pointIndex, __in const uint cameraCount, __in const Scalar* pParameterVector ) __GPU_ONLY
	{
		constexpr const uint pointParamCount = CameraModel::totalParamCount - CameraModel::cameraParameterCount;
		return pParameterVector + cameraCount * CameraModel::cameraParameterCount + pointIndex * pointParamCount;
	}

	template < class CameraModel >
	inline Scalar* GetPointParamPtr( __in const uint pointIndex, __in const uint cameraCount, __in Scalar* pParameterVector ) __GPU_ONLY
	{
		return const_cast<Scalar*>( GetPointParamPtr< CameraModel >( pointIndex, cameraCount, ( const Scalar* )pParameterVector ) );
	}


	void CopyBundle( __in const Bundle* pInBundle, __out Bundle* pOutBundle );

} }
