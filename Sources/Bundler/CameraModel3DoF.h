#pragma once

namespace Bundler { namespace CameraModels {

	class CameraModel3DoF : public BasicCameraModel< 3 >
	{
	public:
		
		void UpdateCamera( __in_ecount( 3 ) const Scalar* pDeltaParams ) override;

	protected:

		void RotatePoint(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pRotatedPoint ) const override;

		void TranslatePoint( __inout_ecount( 3 ) DScalar< totalParamCount >* pPoint ) const override;

		void MultiplyByCalibration(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pCalibratedPoint ) const override;

	};

} }

