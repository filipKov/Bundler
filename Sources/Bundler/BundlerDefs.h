#pragma once

namespace Bundler {
	
	struct CameraCalibration {
		ScalarType fScale;
		ScalarType k1;
		ScalarType k2;
	};

	struct Camera {
		CameraCalibration k;

		Matrix3x3 r;
		Vector3 t;
	};

	struct Track {
		uint cameraIndex;
		uint pointIndex;

		Vector2 projectedPoint;
	};

	struct Point {
		Vector3 position;
		uint color;
	};

	static const uint POINT_PARAM_COUNT = 3;

	struct Bundle {
		Containers::Buffer< Camera > cameras;
		Containers::Buffer< Point > points;
		Containers::Buffer< Track > tracks;
	};

}