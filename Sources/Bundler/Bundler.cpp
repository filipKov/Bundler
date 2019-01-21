#include "stdafx.h"
#include "ReprojectionResiudal.h"
#include "Bundler.h"

#define LM_LAMBDA_INIT 0.001
#define LM_LAMBDA_UP 10
#define LM_LAMBDA_DOWN 0.1

namespace Bundler {

	Bundler::Bundler()
	{
	}

	void Bundler::Optimize( __inout Bundle& bundle ) {
		LevMarSettings defaultSettings;
		defaultSettings.errorTolerance = 10e-5f;
		defaultSettings.lambdaDown = 0.1f;
		defaultSettings.lambdaUp = 10.0f;
		defaultSettings.lambdaInitial = 0.001f;
		defaultSettings.maxIterations = 100;

		Iterate( bundle, defaultSettings );
	}

	void Bundler::Iterate( __inout Bundle& bundle, __in const LevMarSettings& settings ) {
		uint residualCnt = (uint)( bundle.tracks.Length() * 2 );
		uint parameterCnt = (uint)( bundle.cameras.Length() * BUNDLE_PARAM_CAM_CNT + bundle.points.Length() * BUNDLE_PARAM_PTS_CNT );

		OptimizationData data( residualCnt, parameterCnt );
		
		data.error = ComputeError( bundle, data.jacobian, data.residualVector );
		uint iter = 0;

		while ( ( iter < settings.maxIterations ) && ( data.error > settings.errorTolerance ) ) {
			if ( !data.hessianValid ) {
				MultiplyATB( data.jacobian, data.jacobian, data.hessian );
				MultiplyATB( data.jacobian, -data.residualVector, data.jtfNeg );
				data.hessianValid = true;
			}

			data.hessian.MultiplyDiag( 1 + data.lambda );

			LinearAlgebra::Vector< float > delta( parameterCnt );
			m_linearSolver->Solve( data.hessian, data.jtfNeg, delta );
			
			float newError = ComputeError( bundle, delta, data.jacobian, data.residualVector );
			if ( data.error < newError ) {
				data.lambda *= settings.lambdaDown;
				data.error = newError;
				data.hessianValid = false;
				
				SaveDelta( delta, bundle );
			} else {
				data.lambda *= settings.lambdaUp;
			}

			iter++;
		}
	}

	ScalarType Bundler::ComputeError( 
		__in const Bundle& bundle, 
		__out LinearAlgebra::Matrix< ScalarType >& jacobian, 
		__out LinearAlgebra::Vector< ScalarType >& residualVector ) 
	{
		for ( size_t trackI = 0; trackI < bundle.tracks.Length(); trackI++ ) {
			Vector2Diff residual = reprojectionResidual( bundle, trackI );

			const Track& track = bundle.tracks[trackI];
			const uint cameraIx = track.cameraIndex;
			const uint pointIx = track.pointIndex;

			const uint rowX = (uint)( 2 * trackI );
			const uint rowY = (uint)( 2 * trackI + 1 );

			residualVector[rowX] = residual[0].GetFx();
			residualVector[rowY] = residual[1].GetFx();

			const uint cameraParamsIx = (uint)( cameraIx * BUNDLE_PARAM_CAM_CNT );
			const uint pointParamsIx = (uint)( bundle.cameras.Length() * BUNDLE_PARAM_CAM_CNT + pointIx * BUNDLE_PARAM_PTS_CNT );

			ShallowCopy( residual[0].GetDiff().Elements() + BUNDLE_PARAM_CAM_START, BUNDLE_PARAM_CAM_CNT, &jacobian[rowX][cameraParamsIx] );
			ShallowCopy( residual[1].GetDiff().Elements() + BUNDLE_PARAM_CAM_START, BUNDLE_PARAM_CAM_CNT, &jacobian[rowY][cameraParamsIx] );

			ShallowCopy( residual[0].GetDiff().Elements() + BUNDLE_PARAM_PTS_START, BUNDLE_PARAM_PTS_CNT, &jacobian[rowX][pointParamsIx] );
			ShallowCopy( residual[1].GetDiff().Elements() + BUNDLE_PARAM_PTS_START, BUNDLE_PARAM_PTS_CNT, &jacobian[rowY][pointParamsIx] );
		}

		return residualVector.Dot( residualVector );
	}

	ScalarType ComputeError(
		__in const Bundle& bundle,
		__in const LinearAlgebra::Vector< ScalarType >& paramDelta,
		__out LinearAlgebra::Matrix< ScalarType >& jacobian,
		__out LinearAlgebra::Vector< ScalarType >& residualVector )
	{
		for ( size_t trackI = 0; trackI < bundle.tracks.Length(); trackI++ ) {
			const Track& track = bundle.tracks[trackI];
			const uint cameraIx = track.cameraIndex;
			const uint pointIx = track.pointIndex;

			Vector2Diff residual = reprojectionResidual( 
				bundle, 
				trackI, 
				&paramDelta[ (uint)( cameraIx * BUNDLE_PARAM_CAM_CNT ) ], 
				&paramDelta[ (uint)( BUNDLE_PARAM_CAM_CNT * bundle.cameras.Length() + BUNDLE_PARAM_PTS_CNT * pointIx ) ] 
			);

			const uint rowX = (uint)( 2 * trackI );
			const uint rowY = (uint)( 2 * trackI + 1 );

			residualVector[rowX] = residual[0].GetFx();
			residualVector[rowY] = residual[1].GetFx();

			const uint cameraParamsIx = (uint)( cameraIx * BUNDLE_PARAM_CAM_CNT );
			const uint pointParamsIx = (uint)( bundle.cameras.Length() * BUNDLE_PARAM_CAM_CNT + pointIx * BUNDLE_PARAM_PTS_CNT );

			ShallowCopy( residual[0].GetDiff().Elements() + BUNDLE_PARAM_CAM_START, BUNDLE_PARAM_CAM_CNT, &jacobian[rowX][cameraParamsIx] );
			ShallowCopy( residual[1].GetDiff().Elements() + BUNDLE_PARAM_CAM_START, BUNDLE_PARAM_CAM_CNT, &jacobian[rowY][cameraParamsIx] );

			ShallowCopy( residual[0].GetDiff().Elements() + BUNDLE_PARAM_PTS_START, BUNDLE_PARAM_PTS_CNT, &jacobian[rowX][pointParamsIx] );
			ShallowCopy( residual[1].GetDiff().Elements() + BUNDLE_PARAM_PTS_START, BUNDLE_PARAM_PTS_CNT, &jacobian[rowY][pointParamsIx] );
		}

		return residualVector.Dot( residualVector );
	}

	void Bundler::SaveDelta( __in const LinearAlgebra::Vector< ScalarType > delta, __out Bundle& bundle )
	{
		for ( uint cameraIx = 0; cameraIx < bundle.cameras.Length(); cameraIx++ ) {
			const ScalarType* pCameraParams = delta.Elements() + BUNDLE_PARAM_CAM_CNT * cameraIx;

			Camera& camera = bundle.cameras[cameraIx];

			camera.k.fScale += pCameraParams[BUNDLE_PARAM_CAM_CALIB_START + 0];
			camera.k.k1 += pCameraParams[BUNDLE_PARAM_CAM_CALIB_START + 1];
			camera.k.k2 += pCameraParams[BUNDLE_PARAM_CAM_CALIB_START + 2];

			camera.r += Matrix3x3( { 
				{ pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 0], pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 1], pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 2] },
				{ pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 3], pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 4], pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 5] },
				{ pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 6], pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 7], pCameraParams[BUNDLE_PARAM_CAM_ROT_START + 8] } 
			} );

			camera.t += Vector3( { pCameraParams[BUNDLE_PARAM_CAM_TRANSL_START + 0], pCameraParams[BUNDLE_PARAM_CAM_TRANSL_START + 1], pCameraParams[BUNDLE_PARAM_CAM_TRANSL_START + 2] } );
		}

		const size_t pointParamsStart = bundle.cameras.Length() * BUNDLE_PARAM_CAM_CNT;
		for ( uint pointIx = 0; pointIx < bundle.points.Length(); pointIx++ ) {
			const ScalarType* pPointParams = delta.Elements() + pointParamsStart + BUNDLE_PARAM_PTS_CNT * pointIx;

			Point& point = bundle.points[pointIx];

			point.position += Vector3( { pPointParams[0], pPointParams[1], pPointParams[2] } );
		}
	}

}