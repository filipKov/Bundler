#pragma once

namespace Bundler {

	template < class CameraModel >
	class IHessianProvider {
	public:
		using HessianCameraBlock = Matrix< Scalar, CameraModel::totalParamCount, CameraModel::totalParamCount >;
		using HessianCameraPointBlock = Matrix< Scalar, CameraModel::totalParamCount, POINT_PARAM_COUNT >;
		using HessianPointBlock = Matrix< Scalar, POINT_PARAM_COUNT, POINT_PARAM_COUNT >;

		virtual void GetCameraCount() const = 0;

		virtual void GetPointCount() const = 0;


		virtual void GetCameraBlocks(
			__in const size_t cameraIx,
			__deref_out HessianCameraBlock** ppCamBlock,
			__out size_t* pCamPointBlockCount,
			__deref_out size_t** ppPointIndices,
			__deref_out HessianCameraPointBlock** ppCamPtBlocks ) = 0;

		virtual void GetPointBlock(
			__in const size_t pointIx,
			__deref_out HessianPointBlock** ppPtBlock ) = 0;

	};

}