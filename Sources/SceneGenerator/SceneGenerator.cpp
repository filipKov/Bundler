#include "stdafx.h"

#include <random>

#include "AlgebraLib.h"

#include "SceneGenerator.h"

namespace SceneGenerator {

#define NOISE_MAX 0.05

	void AxisAlignedBBox::GetFrom( __in const uint pointCount, __in const Bundler::Point* pPoints ) {
		ptMin = pPoints->position;
		ptMax = pPoints->position;
		pPoints++;

		for ( uint i = 1; i < pointCount; i++ ) {
			const Bundler::ScalarType x = pPoints->position[0];
			const Bundler::ScalarType y = pPoints->position[1];
			const Bundler::ScalarType z = pPoints->position[2];

			if ( x < ptMin[0] ) {
				ptMin[0] = x;
			} else if ( x > ptMax[0] ) {
				ptMax[0] = x;
			}

			if ( y < ptMin[1] ) {
				ptMin[1] = y;
			} else if ( y > ptMax[1] ) {
				ptMax[1] = y;
			}

			if ( z < ptMin[2] ) {
				ptMin[2] = z;
			} else if ( x > ptMax[2] ) {
				ptMax[2] = z;
			}
		}
	}

	SceneGenerator::SceneGenerator() {
		Release();
		m_params.noiseMask = 0;
		m_params.noiseStrength = 0;
		m_params.rngSeed = 1;
		srand( m_params.rngSeed );
	}

	void SceneGenerator::Initialize( __in const SceneGeneratorParams& params ) {
		m_params = params;
		srand( m_params.rngSeed );
	}

	void SceneGenerator::SetGroundTruth( __in const Bundler::Bundle& groundTruth ) {
		m_pGroundTruth = &groundTruth;
		m_groundTruthBBox.GetFrom( (uint)m_pGroundTruth->points.Length(), m_pGroundTruth->points.Data() );
	}

	void SceneGenerator::GetScene( __out Bundler::Bundle& noisedScene ) {
		m_pOutBundle = &noisedScene;
		CopyGroundTruth();

		const uint noiseMask = m_params.noiseMask;
		if ( noiseMask & NOISE_POINT_POS ) {
			AddNoiseToPoints();
		}

		if ( noiseMask & NOISE_CAMERA_POS ) {
			AddNoiseToCameraPosition();
		}

		if ( noiseMask & NOISE_CAMERA_ROT ) {
			AddNoiseToCameraRotation();
		}
	}

	void SceneGenerator::Release() {
		m_pGroundTruth = nullptr;
	}

	void SceneGenerator::CopyGroundTruth() {
		m_pOutBundle->points.SetCopy( m_pGroundTruth->points.Length(), m_pGroundTruth->points.Data() );
		m_pOutBundle->cameras.SetCopy( m_pGroundTruth->cameras.Length(), m_pGroundTruth->cameras.Data() );
		m_pOutBundle->projections.SetCopy( m_pGroundTruth->projections.Length(), m_pGroundTruth->projections.Data() );
	}

	void SceneGenerator::AddNoiseToPoints() {
		Bundler::ScalarType noiseStrengthModifier = Bundler::ScalarType( m_params.noiseStrength );
		Bundler::ScalarType noiseBase = Bundler::ScalarType( m_groundTruthBBox.ptMin.Distance( m_groundTruthBBox.ptMax ) * NOISE_MAX );

		uint pointCount = (uint)m_pOutBundle->points.Length();
		for ( uint i = 0; i < pointCount; i++ ) {
			AddNoiseToPoint( noiseBase, noiseStrengthModifier, m_pOutBundle->points[i] );
		}
	}

	void SceneGenerator::AddNoiseToPoint( __in const Bundler::ScalarType noiseBase, __in const Bundler::ScalarType noiseModifier, __inout Bundler::Point& point ) {
		Vector3f positionNoise;
		positionNoise[0] = noiseModifier * noiseBase * ( ( 2 * (Bundler::ScalarType)rand() / RAND_MAX ) - 1 );
		positionNoise[1] = noiseModifier * noiseBase * ( ( 2 * (Bundler::ScalarType)rand() / RAND_MAX ) - 1 );
		positionNoise[2] = noiseModifier * noiseBase * ( ( 2 * (Bundler::ScalarType)rand() / RAND_MAX ) - 1 );

		point.position += positionNoise;
	}

	void SceneGenerator::AddNoiseToCameraPosition() {
		// TODO later
	}

	void SceneGenerator::AddNoiseToCameraRotation() {
		// TODO later
	}

}

