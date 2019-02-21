#pragma once

namespace Bundler { namespace CameraModels {

	template < uint cameraParamCount >
	class ICameraModel {
	public:

		static constexpr const uint cameraParameterCount = cameraParamCount;

		static constexpr const uint totalParamCount = cameraParamCount + POINT_PARAM_COUNT;

		virtual void UpdateCamera( __in_ecount( cameraParamCount ) const Scalar* pDeltaParams ) = 0;

		virtual void ProjectPoint( 
			__in_ecount( 3 ) const Scalar* pPointCoords, 
			__in_ecount( 2 ) const Scalar* pFeatureCoords,
			__out_ecount( 2 ) DScalar< totalParamCount >* pResiduals ) const = 0;

	};

} }