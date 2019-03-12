#pragma once

namespace Bundler { namespace Utils {

	template < class CameraModel >
	inline Scalar* GetCameraParamPtr( __in const size_t cameraIndex, __in Scalar* pParameterVector )
	{
		return pParameterVector + cameraIndex * CameraModel::cameraParameterCount;
	}

	template < class CameraModel >
	inline Scalar* GetPointParamPtr( __in const size_t pointIndex, __in const size_t cameraCount, __in Scalar* pParameterVector )
	{
		constexpr const size_t pointParamCount = CameraModel::totalParamCount - CameraModel::cameraParameterCount;
		return pParameterVector + cameraCount * CameraModel::cameraParameterCount * pointIndex * pointParamCount;
	}

} }