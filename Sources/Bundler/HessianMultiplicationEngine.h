#pragma once

namespace Bundler {

	template < class CameraModel >
	class HessianMultiplicationEngine
	{
	public:

		size_t GetCameraCount();

		size_t GetPointCount();

		void MultiplyCameraRow(
			__in const size_t cameraIx,
			__in_ecount( CameraModel::cameraParameterCount ) Scalar* pX,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pY );

		void MultiplyCameraRow(
			__in const size_t pointIx,
			__in_ecount( POINT_PARAM_COUNT ) Scalar* pX,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pY );

	};

}