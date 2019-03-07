#pragma once

namespace Bundler {

	template < class CameraModel >
	using HessianCameraBlock = Matrix< Scalar, CameraModel::cameraParameterCount, CameraModel::cameraParameterCount >;

	template < class CameraModel >
	using HessianCameraPointBlock = Matrix< Scalar, CameraModel::cameraParameterCount, (int)POINT_PARAM_COUNT >;

	using HessianPointBlock = Matrix< Scalar, (int)POINT_PARAM_COUNT, (int)POINT_PARAM_COUNT >;

	template < class CameraModel >
	class IHessianProvider {
	public:

		virtual size_t GetCameraCount() const = 0;

		virtual size_t GetCameraPointBlockCount( __in const size_t cameraIx ) const = 0;

		virtual size_t GetPointCount() const = 0;

		virtual void GetCameraBlocks(
			__in const size_t cameraIx,
			__out HessianCameraBlock< CameraModel >* pCamBlock,
			__in const size_t camPointBlockCount,
			__out size_t* pPointIndices,
			__out HessianCameraPointBlock< CameraModel >* pCamPtBlocks,
			__out_ecount_opt( CameraModel::cameraParameterCount ) Scalar* pJtfBlock ) const = 0;

		virtual void GetPointBlock(
			__in const size_t pointIx,
			__out HessianPointBlock* pPointBlock,
			__out_ecount_opt( POINT_PARAM_COUNT ) Scalar* pJtfBlock ) const = 0;



	};

}