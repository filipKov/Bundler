#pragma once

namespace SceneGenerator {

	struct SceneGenRange
	{
		Bundler::Scalar minDelta;
		Bundler::Scalar maxDelta;
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

		void AddNoise( 
			__in const SceneGenNoiseSettings* pNoiseParameters,
			__in const Bundler::Bundle* pGroundTruth, 
			__out Bundler::Bundle* pNoisedScene );

	protected:

		void CopyGroundTruth(
			__in const Bundler::Bundle* pGroundTruth,
			__out Bundler::Bundle* pNoisedScene );

		void AddNoise(
			__in const SceneGenRange* pSettings,
			__in const uint n,
			__inout_ecount( n ) Bundler::Scalar* pValues );

		void Clamp(
			__in const Bundler::Scalar minVal,
			__in const Bundler::Scalar maxVal,
			__in const uint n,
			__inout_ecount( n ) Bundler::Scalar* pValues );

	};

}
