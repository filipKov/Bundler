#pragma once

namespace Bundler {

	struct Camera 
	{
		Scalar focalLength;
		Matrix3x3 r;
		Vector3 t;
	};

	struct CameraInfo
	{
		uint width;
		uint height;
	};

	struct Projection 
	{
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
		Containers::Buffer< CameraInfo > cameraInformation;
	};

}