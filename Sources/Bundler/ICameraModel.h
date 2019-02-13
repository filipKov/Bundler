#pragma once

namespace Bundler { namespace CameraModels {

	template < uint parameterCount >
	class ICameraModel {
	public:

		virtual void UpdateCamera( __in_ecount( parameterCount ) const Scalar* pDeltaParams ) = 0;

		virtual void ProjectPoint( 
			__in_ecount( 3 ) const Scalar* pPointCoords, 
			__in_ecount( 2 ) const Scalar* pFeatureCoords,
			__out_ecount( 2 ) DScalar< parameterCount + POINT_PARAM_COUNT >* pResiduals ) const = 0;

	};

} }