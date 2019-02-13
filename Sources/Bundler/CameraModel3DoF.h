#pragma once

namespace Bundler { namespace CameraModels {

	class CameraModel3DoF : public ICameraModel< 3 >
	{
	public:
		
		void Initialize( __in Camera* pCamera );

		virtual void UpdateCamera( __in_ecount( 3 ) const Scalar* pDeltaParams ) override;

		virtual void ProjectPoint(
			__in_ecount( 3 ) const Scalar* pPointCoords,
			__in_ecount( 2 ) const Scalar* pFeatureCoords,
			__out_ecount( 2 ) DScalar< 3 + POINT_PARAM_COUNT >* pResiduals ) const override;

	protected:

		void RotatePoint(
			__in_ecount( 3 ) const DScalar< 3 + POINT_PARAM_COUNT >* pPoint,
			__out_ecount( 3 ) DScalar< 3 + POINT_PARAM_COUNT >* pRotatedPoint ) const;

		void TranslatePoint( __inout_ecount( 3 ) DScalar< 3 + POINT_PARAM_COUNT >* pPoint ) const;

		void MultiplyByCalibration(
			__in_ecount( 3 ) const DScalar< 3 + POINT_PARAM_COUNT >* pPoint,
			__out_ecount( 3 ) DScalar< 3 + POINT_PARAM_COUNT >* pCalibratedPoint ) const;

	protected:

		Camera* m_pCamera;

	};

} }

