#pragma once

namespace Bundler {

	template < uint cameraParamCount >
	struct JacobianCameraBlock {
		uint cameraIndex;

		Scalar diffX[ cameraParamCount ];
		Scalar diffY[ cameraParamCount ];
	};

	struct JacobianPointBlock {
		uint pointIndex;

		Scalar diffX[ POINT_PARAM_COUNT ];
		Scalar diffY[ POINT_PARAM_COUNT ];
	};

	template < uint cameraModelParamCount >
	struct JacobianRow {
		JacobianCameraBlock< cameraModelParamCount - POINT_PARAM_COUNT > cameraBlock;
		JacobianPointBlock pointBlock;
	};



}