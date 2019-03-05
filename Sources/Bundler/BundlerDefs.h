#pragma once

namespace Bundler {
	
	struct CameraCalibration {
		Scalar fScale;
		Scalar k1;
		Scalar k2;
	};

	struct Camera {
		CameraCalibration k;

		Matrix3x3 r;
		Vector3 t;
	};

	struct Projection {
		uint cameraIndex;
		uint pointIndex;

		Vector2 projectedPoint;
	};

	static constexpr const uint POINT_PARAM_COUNT = 3;

	struct Bundle 
	{
		Containers::Buffer< Camera > cameras;
		Containers::Buffer< Vector3 > points;
		Containers::Buffer< Projection > projections;
	};

	struct BundleAdditionalPayload 
	{
		Containers::Buffer< uint > pointColors;
	};

}