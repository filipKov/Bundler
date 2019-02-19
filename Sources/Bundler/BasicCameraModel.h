#pragma once

namespace Bundler { namespace CameraModels {

	template < uint cameraParamCount >
	class BasicCameraModel : public ICameraModel< cameraParamCount > 
	{
	public:

		virtual void Initialize( __in Camera* pCamera ) {
			m_pCamera = pCamera;
		}

		void ProjectPoint(
			__in_ecount( 3 ) const Scalar* pPointCoords,
			__in_ecount( 2 ) const Scalar* pFeatureCoords,
			__out_ecount( 2 ) DScalar< totalParamCount >* pResiduals ) const override 
		{
			DScalar< totalParamCount > pt1[ 3 ] =
			{
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 0 ), cameraParamCount + 0 ),
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 1 ), cameraParamCount + 1 ),
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 2 ), cameraParamCount + 2 ),
			};

			DScalar< totalParamCount > pt2[ 3 ];
			RotatePoint( pt1, pt2 );
			TranslatePoint( pt2 );
			MultiplyByCalibration( pt2, pt1 );

			ELEMENT( pResiduals, 0 ) = ( ELEMENT( pt1, 0 ) / ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 0 );
			ELEMENT( pResiduals, 0 ) = ( ELEMENT( pt1, 1 ) / ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 1 );
		}

	protected:

		virtual void RotatePoint(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pRotatedPoint ) const = 0;

		virtual void TranslatePoint( __inout_ecount( 3 ) DScalar< totalParamCount >* pPoint ) const = 0;

		virtual void MultiplyByCalibration(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pCalibratedPoint ) const = 0;

	protected:

		Camera* m_pCamera;

	};

} }