#pragma once

namespace SceneGenerator {

#define NOISE_POINT_POS 1
#define NOISE_CAMERA_POS 2
#define NOISE_CAMERA_ROT 4

	struct SceneGeneratorParams {
		uint noiseMask;
		double noiseStrength;
		uint rngSeed;
	};

	struct AxisAlignedBBox {
		Vector3f ptMin;
		Vector3f ptMax;

		void GetFrom( __in const uint pointCount, __in const Bundler::Point* pPoints );
	};

	class SceneGenerator
	{
	public:

		SceneGenerator();

		void Initialize( __in const SceneGeneratorParams& params );

		void SetGroundTruth( __in const Bundler::Bundle& groundTruth );

		void GetScene( __out Bundler::Bundle& noisedScene );

		void Release();

	protected:

		void CopyGroundTruth();

		void AddNoiseToPoints();

		void AddNoiseToPoint( __in const Bundler::ScalarType noiseBase, __in const Bundler::ScalarType noiseModifier, __inout Bundler::Point& point );

		void AddNoiseToCameraPosition();

		void AddNoiseToCameraRotation();

	protected:

		SceneGeneratorParams m_params;

		const Bundler::Bundle* m_pGroundTruth;
		AxisAlignedBBox m_groundTruthBBox;
		
		Bundler::Bundle* m_pOutBundle;

	};

}
