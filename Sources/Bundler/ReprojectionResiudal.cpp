#include "stdafx.h"
#include "ReprojectionResiudal.h"

namespace Bundler {

	void SetCalibration( __in const Camera& camera, __out Matrix3x3Diff& K ) {
		K[0][0] = BundleDiffNum( camera.k.fScale, BUNDLE_PARAM_CAM_CALIB_START + 0 );
		K[0][2] = BundleDiffNum( camera.k.k1, BUNDLE_PARAM_CAM_CALIB_START + 0 );
		K[1][1] = BundleDiffNum( camera.k.fScale, BUNDLE_PARAM_CAM_CALIB_START + 1 );
		K[1][2] = BundleDiffNum( camera.k.k2, BUNDLE_PARAM_CAM_CALIB_START + 2 );
		K[2][2] = BundleDiffNum( 1 );
	}

	void SetRotationTranslation( __in const Camera& camera, __out Matrix3x4Diff& Rt ) {
		const ScalarType* pR = camera.r.Elements();
		const ScalarType* pT = camera.t.Elements();
		BundleDiffNum* pRt = Rt.Elements();

		*( pRt + 0 ) = BundleDiffNum( *( pR + 0 ), BUNDLE_PARAM_CAM_ROT_START + 0 );
		*( pRt + 1 ) = BundleDiffNum( *( pR + 1 ), BUNDLE_PARAM_CAM_ROT_START + 1 );
		*( pRt + 2 ) = BundleDiffNum( *( pR + 2 ), BUNDLE_PARAM_CAM_ROT_START + 2 );
		*( pRt + 3 ) = BundleDiffNum( *( pT + 0 ), BUNDLE_PARAM_CAM_TRANSL_START + 0 );

		*( pRt + 4 ) = BundleDiffNum( *( pR + 3 ), BUNDLE_PARAM_CAM_ROT_START + 3 );
		*( pRt + 5 ) = BundleDiffNum( *( pR + 4 ), BUNDLE_PARAM_CAM_ROT_START + 4 );
		*( pRt + 6 ) = BundleDiffNum( *( pR + 5 ), BUNDLE_PARAM_CAM_ROT_START + 5 );
		*( pRt + 7 ) = BundleDiffNum( *( pT + 1 ), BUNDLE_PARAM_CAM_TRANSL_START + 1 );

		*( pRt + 8 ) = BundleDiffNum( *( pR + 6 ), BUNDLE_PARAM_CAM_ROT_START + 6 );
		*( pRt + 9 ) = BundleDiffNum( *( pR + 7 ), BUNDLE_PARAM_CAM_ROT_START + 7 );
		*( pRt + 10 ) = BundleDiffNum( *( pR + 8 ), BUNDLE_PARAM_CAM_ROT_START + 8 );
		*( pRt + 11 ) = BundleDiffNum( *( pT + 2 ), BUNDLE_PARAM_CAM_TRANSL_START + 2 );
	}

	void SetPoint( __in const Point& point, __out Vector4Diff& x ) {
		const ScalarType* pPt = point.position.Elements();
		BundleDiffNum* pX = x.Elements();

		*( pX + 0 ) = BundleDiffNum( *( pPt + 0 ), BUNDLE_PARAM_PTS_START + 0 );
		*( pX + 1 ) = BundleDiffNum( *( pPt + 1 ), BUNDLE_PARAM_PTS_START + 1 );
		*( pX + 2 ) = BundleDiffNum( *( pPt + 2 ), BUNDLE_PARAM_PTS_START + 2 );
		*( pX + 3 ) = BundleDiffNum( 1 );
	}

	void SetCalibration( __in const Camera& camera, __out Matrix3x3Diff& K, __in_ecount( BUNDLE_PARAM_CAM_CNT ) const ScalarType* cameraDeltas ) {
		K[0][0] = BundleDiffNum( camera.k.fScale + cameraDeltas[BUNDLE_PARAM_CAM_CALIB_START + 0], BUNDLE_PARAM_CAM_CALIB_START + 0 );
		K[0][2] = BundleDiffNum( camera.k.k1 + cameraDeltas[BUNDLE_PARAM_CAM_CALIB_START + 1], BUNDLE_PARAM_CAM_CALIB_START + 0 );
		K[1][1] = BundleDiffNum( camera.k.fScale + cameraDeltas[BUNDLE_PARAM_CAM_CALIB_START + 0], BUNDLE_PARAM_CAM_CALIB_START + 1 );
		K[1][2] = BundleDiffNum( camera.k.k2 + cameraDeltas[BUNDLE_PARAM_CAM_CALIB_START + 2], BUNDLE_PARAM_CAM_CALIB_START + 2 );
		K[2][2] = BundleDiffNum( 1 );
	}

	void SetRotationTranslation( __in const Camera& camera, __out Matrix3x4Diff& Rt, __in_ecount( BUNDLE_PARAM_CAM_CNT ) const ScalarType* cameraDeltas ) {
		const ScalarType* pR = camera.r.Elements();
		const ScalarType* pT = camera.t.Elements();
		BundleDiffNum* pRt = Rt.Elements();

		*( pRt + 0 ) = BundleDiffNum( *( pR + 0 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 0], BUNDLE_PARAM_CAM_ROT_START + 0 );
		*( pRt + 1 ) = BundleDiffNum( *( pR + 1 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 1], BUNDLE_PARAM_CAM_ROT_START + 1 );
		*( pRt + 2 ) = BundleDiffNum( *( pR + 2 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 2], BUNDLE_PARAM_CAM_ROT_START + 2 );
		*( pRt + 3 ) = BundleDiffNum( *( pT + 0 ) + cameraDeltas[BUNDLE_PARAM_CAM_TRANSL_START + 0], BUNDLE_PARAM_CAM_TRANSL_START + 0 );

		*( pRt + 4 ) = BundleDiffNum( *( pR + 3 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 3], BUNDLE_PARAM_CAM_ROT_START + 3 );
		*( pRt + 5 ) = BundleDiffNum( *( pR + 4 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 4], BUNDLE_PARAM_CAM_ROT_START + 4 );
		*( pRt + 6 ) = BundleDiffNum( *( pR + 5 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 5], BUNDLE_PARAM_CAM_ROT_START + 5 );
		*( pRt + 7 ) = BundleDiffNum( *( pT + 1 ) + cameraDeltas[BUNDLE_PARAM_CAM_TRANSL_START + 1], BUNDLE_PARAM_CAM_TRANSL_START + 1 );

		*( pRt + 8 ) = BundleDiffNum( *( pR + 6 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 6], BUNDLE_PARAM_CAM_ROT_START + 6 );
		*( pRt + 9 ) = BundleDiffNum( *( pR + 7 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 7], BUNDLE_PARAM_CAM_ROT_START + 7 );
		*( pRt + 10 ) = BundleDiffNum( *( pR + 8 ) + cameraDeltas[BUNDLE_PARAM_CAM_ROT_START + 8], BUNDLE_PARAM_CAM_ROT_START + 8 );
		*( pRt + 11 ) = BundleDiffNum( *( pT + 2 ) + cameraDeltas[BUNDLE_PARAM_CAM_TRANSL_START + 0], BUNDLE_PARAM_CAM_TRANSL_START + 2 );
	}

	void SetPoint( __in const Point& point, __out Vector4Diff& x, __in_ecount( BUNDLE_PARAM_PTS_CNT ) const ScalarType* pointDeltas ) {
		const ScalarType* pPt = point.position.Elements();
		BundleDiffNum* pX = x.Elements();

		*( pX + 0 ) = BundleDiffNum( *( pPt + 0 ) + pointDeltas[0], BUNDLE_PARAM_PTS_START + 0 );
		*( pX + 1 ) = BundleDiffNum( *( pPt + 1 ) + pointDeltas[1], BUNDLE_PARAM_PTS_START + 1 );
		*( pX + 2 ) = BundleDiffNum( *( pPt + 2 ) + pointDeltas[2], BUNDLE_PARAM_PTS_START + 2 );
		*( pX + 3 ) = BundleDiffNum( 1 );
	}

	inline Vector2Diff reprojectionResidual( __in const Matrix3x3Diff K, __in const Matrix3x4Diff Rt, __in const Vector4Diff x, __in const Vector2& originalPoint ) {
		Vector3Diff projectedPoint = K * Rt * x;
		Vector2Diff originalPt( { BundleDiffNum( originalPoint[0] ), BundleDiffNum( originalPoint[1] ) } );

		return originalPt - Vector2Diff( { projectedPoint[0] / projectedPoint[2], projectedPoint[1] / projectedPoint[2] } );
	}

	Vector2Diff reprojectionResidual( __in const Bundle& bundle, __in const size_t trackIndex ) {
		const Track& track = bundle.tracks[trackIndex];
		const Camera& camera = bundle.cameras[track.cameraIndex];
		const Point& point = bundle.points[track.pointIndex];

		Matrix3x3Diff K;
		Matrix3x4Diff Rt;
		Vector4Diff x;

		SetCalibration( camera, K );
		SetRotationTranslation( camera, Rt );
		SetPoint( point, x );

		return reprojectionResidual( K, Rt, x, track.projectedPoint );
	}

	Vector2Diff reprojectionResidual(
		__in const Bundle& bundle,
		__in const size_t trackIndex,
		__in_ecount( BUNDLE_PARAM_CAM_CNT ) const ScalarType* cameraDeltas,
		__in_ecount( BUNDLE_PARAM_PTS_CNT ) const ScalarType* pointDeltas )
	{
		const Track& track = bundle.tracks[trackIndex];
		const Camera& camera = bundle.cameras[track.cameraIndex];
		const Point& point = bundle.points[track.pointIndex];

		Matrix3x3Diff K;
		Matrix3x4Diff Rt;
		Vector4Diff x;

		SetCalibration( camera, K, cameraDeltas );
		SetRotationTranslation( camera, Rt, cameraDeltas );
		SetPoint( point, x, pointDeltas );

		return reprojectionResidual( K, Rt, x, track.projectedPoint );
	}

	// void SetCalibration( __in const Camera& camera, __out Matrix3x3& K ) {
	// 	K[0][0] = camera.k.fScale;
	// 	K[0][2] = camera.k.k1;
	// 	K[1][1] = camera.k.fScale;
	// 	K[1][2] = camera.k.k2;
	// 	K[2][2] = ScalarType( 1 );
	// }
	// 
	// void SetRotationTranslation( __in const Camera& camera, __out Matrix3x4& Rt ) {
	// 	const ScalarType* pR = camera.r.Elements();
	// 	const ScalarType* pT = camera.t.Elements();
	// 	ScalarType* pRt = Rt.Elements();
	// 
	// 	*( pRt + 0 ) = *( pR + 0 );
	// 	*( pRt + 1 ) = *( pR + 1 );
	// 	*( pRt + 2 ) = *( pR + 2 );
	// 	*( pRt + 3 ) = *( pT + 0 );
	// 
	// 	*( pRt + 4 ) = *( pR + 3 );
	// 	*( pRt + 5 ) = *( pR + 4 );
	// 	*( pRt + 6 ) = *( pR + 5 );
	// 	*( pRt + 7 ) = *( pT + 1 );
	// 
	// 	*( pRt + 8 ) = *( pR + 6 );
	// 	*( pRt + 9 ) = *( pR + 7 );
	// 	*( pRt + 10 ) = *( pR + 8 );
	// 	*( pRt + 11 ) = *( pT + 2 );
	// }
	// 
	// void SetPoint( __in const Point& point, __out Vector4& x ) {
	// 	const ScalarType* pPt = point.position.Elements();
	// 	ScalarType* pX = x.Elements();
	// 
	// 	*( pX + 0 ) = *( pPt + 0 );
	// 	*( pX + 1 ) = *( pPt + 1 );
	// 	*( pX + 2 ) = *( pPt + 2 );
	// 	*( pX + 3 ) = ScalarType( 1 );
	// }
	// 
	// Vector2 reprojectionResidualNoDiff( __in const Bundle& bundle, __in const size_t trackIndex ) {
	// 	const Track& track = bundle.tracks[trackIndex];
	// 	const Camera& camera = bundle.cameras[track.cameraIndex];
	// 	const Point& point = bundle.points[track.pointIndex];
	// 
	// 	Matrix3x3 K;
	// 	Matrix3x4 Rt;
	// 	Vector4 x;
	// 
	// 	SetCalibration( camera, K );
	// 	SetRotationTranslation( camera, Rt );
	// 	SetPoint( point, x );
	// 
	// 	Vector3 projectedPoint = K * Rt * x;
	// 	return track.projectedPoint - Vector2( { projectedPoint[0] / projectedPoint[2], projectedPoint[1] / projectedPoint[2] } );
	// }

}
