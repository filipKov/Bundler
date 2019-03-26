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
	
	enum SceneGenAutoNoiseMask
	{
		POINTS = 1,
		CAMAERA_TRANSLATION = 2,
		CAMAERA_ROTATION = 4,
		CAMAERA_CALIBRATION = 8,
	};

	class SceneGen
	{
	public:

		static void GetAutoNoise(
			__in const Bundle* pBundle,
			__in const uint noiseMask,
			__out SceneGenNoiseSettings* pNoise );
	
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

		static void GetAutoNoisePoints(
			__in const uint count,
			__in_ecount( count ) const Vector3* pPoints,
			__out SceneGenRange* pNoise );

		static void GetAutoNoiseCameraTranslation(
			__in const uint count,
			__in_ecount( count ) const Camera* pCameras,
			__out SceneGenRange* pNoise );

		static void GetAutoNoiseCameraCalibration(
			__in const uint count,
			__in_ecount( count ) const Camera* pCameras,
			__out SceneGenRange* pNoise );


	
	};

} }
