#pragma once

namespace Bundler {

	template < class CameraModel >
	using HessianCameraBlock = Matrix< Scalar, CameraModel::totalParamCount, CameraModel::totalParamCount >;

	template < class CameraModel >
	using HessianCameraPointBlock = Matrix< Scalar, CameraModel::totalParamCount, (int)POINT_PARAM_COUNT >;

	using HessianPointBlock = Matrix< Scalar, (int)POINT_PARAM_COUNT, (int)POINT_PARAM_COUNT >;

	template < class CameraModel >
	class IHessianProvider {
	public:

		virtual size_t GetCameraCount() const = 0;

		virtual size_t GetCameraPointBlockCount( __in const size_t cameraIx ) const = 0;

		virtual size_t GetPointCount() const = 0;

		virtual void GetCameraBlocks(
			__in const size_t cameraIx,
			__out HessianCameraBlock< CameraModel >* ppCamBlock,
			__in const size_t camPointBlockCount,
			__out size_t* pPointIndices,
			__out HessianCameraPointBlock< CameraModel >* pCamPtBlocks ) const = 0;

		virtual void GetPointBlock(
			__in const size_t pointIx,
			__out HessianPointBlock* pPointBlock ) const = 0;


	};

}