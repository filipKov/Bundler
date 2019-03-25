#pragma once

namespace Bundler { namespace SceneGenerator {

	struct SceneGenRange
	{
		Scalar minDelta;
		Scalar maxDelta;
	};
	
	struct SceneGenCameraNoiseSettings
	{
		SceneGenRange translationNoise;
		SceneGenRange focalLengthNoise;
		SceneGenRange rotationNoise;
	};
	
	struct SceneGenNoiseSettings
	{
		SceneGenRange pointSettings;
		SceneGenCameraNoiseSettings cameraSettings;
		uint rngSeed;
	};
	
	class SceneGen
	{
	public:
	
		static void AddNoise(
			__in const SceneGenNoiseSettings* pNoiseParameters,
			__in const Bundle* pGroundTruth,
			__out Bundle* pNoisedScene );
	
	protected:
	
		static void CopyGroundTruth(
			__in const Bundle* pGroundTruth,
			__out Bundle* pNoisedScene );
	
		static void AddNoise(
			__in const SceneGenRange* pSettings,
			__in const uint n,
			__inout_ecount( n ) Scalar* pValues );
	
		static void Clamp(
			__in const Scalar minVal,
			__in const Scalar maxVal,
			__in const uint n,
			__inout_ecount( n ) Scalar* pValues );
	
	};

} }
