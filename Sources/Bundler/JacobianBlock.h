#pragma once

namespace Bundler {

	template < uint cameraParamCount >
	struct JacobianCameraBlock {
		Vector< Scalar, cameraParamCount > diffX;
		Vector< Scalar, cameraParamCount > diffY;
	};

	struct JacobianPointBlock {
		Vector< Scalar, POINT_PARAM_COUNT > diffX;
		Vector< Scalar, POINT_PARAM_COUNT > diffY;
	};

	template < uint cameraModelParamCount >
	struct JacobianRow {
		JacobianCameraBlock< cameraModelParamCount - POINT_PARAM_COUNT > cameraBlock;
		JacobianPointBlock pointBlock;
	};



}