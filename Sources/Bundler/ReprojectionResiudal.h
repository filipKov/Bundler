#pragma once

namespace Bundler {

	Vector2Diff reprojectionResidual( __in const Bundle& bundle, __in const size_t trackIndex );

	Vector2Diff reprojectionResidual(
		__in const Bundle& bundle,
		__in const size_t trackIndex,
		__in_ecount( BUNDLE_PARAM_CAM_CNT ) const ScalarType* cameraDeltas,
		__in_ecount( BUNDLE_PARAM_PTS_CNT ) const ScalarType* pointDeltas );

	// Vector2 reprojectionResidualNoDiff( __in const Bundle& bundle, __in const size_t trackIndex );

}
