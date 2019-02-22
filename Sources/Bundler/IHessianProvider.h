#pragma once

namespace Bundler {
	
	template < class CameraModel >
	struct HessianCameraPointBlock {
		uint pointIndex;

		Matrix< Scalar, CameraModel::totalParamCount, POINT_PARAM_COUNT > diffs;
	};

	template < class CameraModel >
	class IHessianProvider {
	public:
		using HessianCameraBlock = Matrix< Scalar, CameraModel::totalParamCount, CameraModel::totalParamCount >;
		using HessianPointBlock = Matrix< Scalar, POINT_PARAM_COUNT, POINT_PARAM_COUNT >;

		virtual void GetCameraCount() const = 0;

		virtual void GetPointCount() const = 0;


		virtual void GetCameraBlocks( 
			__in const uint cameraIx,
			__deref_out HessianCameraBlock** ppCamBlock,
			__out uint* pCamPointBlockCount,
			__deref_out HessianCameraPointBlock< CameraModel >** ppCamPtBlocks ) = 0;

		virtual void GetPointBlock(
			__in const uint pointIx,
			__deref_out HessianPointBlock** ppPtBlock ) = 0;

	};

}