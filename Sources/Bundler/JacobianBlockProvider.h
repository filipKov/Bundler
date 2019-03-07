#pragma once

namespace Bundler {

	template < class CameraModel >
    class JacobianBlockProvider 
	{
	public:

		size_t GetCameraCount();

		size_t GetCameraCount( __in const size_t pointIx );

		size_t GetPointCount();

		size_t GetPointCount( __in const size_t cameraIx );

		size_t GetPointIndex( __in const size_t cameraIx, __in const size_t ix );

		size_t GetCameraIndex( __in const size_t pointIx, __in const size_t ix );

		void GetCameraBlockDimensions( __out uint* pRows, __out uint* pColumns );

		void GetCameraBlock(
			__in const size_t cameraIx
			__in const size_t rows,
			__in const size_t columns,
			__out_ecount( rows * columns ) Scalar* pBlock
			__out_ecount_opt( rows ) Scalar* pResiduals );

		void GetPointBlock(
			__in const size_t cameraIx,
			__in const size_t pointIx,
			__out_ecount( 2 * POINT_PARAM_COUNT ) Scalar* pBlock );

	};

}