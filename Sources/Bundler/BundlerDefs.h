#pragma once

namespace Bundler {
	
	using ScalarType = float;

	using Vector2 = LinearAlgebra::Vector< ScalarType, 2 >;
	using Vector3 = LinearAlgebra::Vector< ScalarType, 3 >;
	using Vector4 = LinearAlgebra::Vector< ScalarType, 4 >;

	using Matrix3x3 = LinearAlgebra::Matrix< ScalarType, 3, 3 >;
	using Matrix3x4 = LinearAlgebra::Matrix< ScalarType, 3, 4 >;
	using Matrix4x4 = LinearAlgebra::Matrix< ScalarType, 4, 4 >;

	
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

	struct Bundle {
		Containers::Buffer< Camera > cameras;
		Containers::Buffer< Point > points;
		Containers::Buffer< Track > tracks;
	};


	#define BUNDLE_PARAM_CAM_CNT 15
	#define BUNDLE_PARAM_PTS_CNT 3
	#define BUNDLE_PARAM_CNT BUNDLE_PARAM_CAM_CNT + BUNDLE_PARAM_PTS_CNT
	
	#define BUNDLE_PARAM_CAM_START 0
	#define BUNDLE_PARAM_CAM_CALIB_START 0
	#define BUNDLE_PARAM_CAM_CALIB_CNT 3
	#define BUNDLE_PARAM_CAM_ROT_START BUNDLE_PARAM_CAM_CALIB_START + BUNDLE_PARAM_CAM_CALIB_CNT
	#define BUNDLE_PARAM_CAM_ROT_CNT 9
	#define BUNDLE_PARAM_CAM_TRANSL_START BUNDLE_PARAM_CAM_ROT_START + BUNDLE_PARAM_CAM_ROT_CNT
	#define BUNDLE_PARAM_CAM_TRANSL_CNT 3

	#define BUNDLE_PARAM_PTS_START BUNDLE_PARAM_CAM_START + BUNDLE_PARAM_CAM_CNT

	using BundleDiffNum = NumericOptimization::AutomaticDifferentiation::DiffNum< ScalarType, BUNDLE_PARAM_CNT >;

	using Vector2Diff = LinearAlgebra::Vector< BundleDiffNum, 2 >;
	using Vector3Diff = LinearAlgebra::Vector< BundleDiffNum, 3 >;
	using Vector4Diff = LinearAlgebra::Vector< BundleDiffNum, 4 >;

	using Matrix3x3Diff = LinearAlgebra::Matrix< BundleDiffNum, 3, 3 >;
	using Matrix3x4Diff = LinearAlgebra::Matrix< BundleDiffNum, 3, 4 >;
	using Matrix4x4Diff = LinearAlgebra::Matrix< BundleDiffNum, 4, 4 >;
}