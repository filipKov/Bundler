#pragma once

namespace Bundler {

	template < class CameraModel >
	class HessianBlockProvider
	{
	public:

		size_t GetCameraCount();

		size_t GetPointCount();

		size_t GetPointCount( __in const size_t cameraIx );

		size_t GetPointIndex( __in const size_t cameraIx, __in const size_t ix );

		void GetCameraBlock(
			__in const size_t cameraIx,
			__out_ecount( CameraModel::cameraParameterCount * CameraModel::cameraParameterCount ) Scalar* pBlock );

		void GetCameraPointBlock(
			__in const size_t cameraIx,
			__in const size_t pointIx,
			__out_ecount( CameraModel::cameraParameterCount * POINT_PARAM_COUNT ) Scalar* pBlock );

		void GetPointBlock(
			__in const size_t pointIx,
			__out_ecount( POINT_PARAM_COUNT * POINT_PARAM_COUNT ) Scalar* pBlock );

	};

}