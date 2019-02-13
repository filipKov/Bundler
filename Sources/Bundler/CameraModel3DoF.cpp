#include "stdafx.h"
#include "ICameraModel.h"
#include "CameraModel3DoF.h"

#define CAMERA_PARAM_CNT 3
#define PARAM_CNT CAMERA_PARAM_CNT + POINT_PARAM_COUNT

namespace Bundler { namespace CameraModels {

	void CameraModel3DoF::Initialize( __in Camera* pCamera )
	{
		m_pCamera = pCamera;
	}

	void CameraModel3DoF::UpdateCamera( __in_ecount( CAMERA_PARAM_CNT ) const Scalar* pDeltaParams ) 
	{
		m_pCamera->t += pDeltaParams;
	}

	void CameraModel3DoF::ProjectPoint(
		__in_ecount( 3 ) const Scalar* pPointCoords,
		__in_ecount( 2 ) const Scalar* pFeatureCoords,
		__out_ecount( 2 ) DScalar< PARAM_CNT >* pResiduals ) const
	{
		DScalar< PARAM_CNT > pt1[ 3 ] = 
		{
			DScalar< PARAM_CNT >( pPointCoords[ 0 ], CAMERA_PARAM_CNT + 0 ),
			DScalar< PARAM_CNT >( pPointCoords[ 1 ], CAMERA_PARAM_CNT + 1 ),
			DScalar< PARAM_CNT >( pPointCoords[ 2 ], CAMERA_PARAM_CNT + 2 ),
		};

		DScalar< PARAM_CNT > pt2[ 3 ];
		RotatePoint( pt1, pt2 );
		TranslatePoint( pt2 );
		MultiplyByCalibration( pt2, pt1 );

		ELEMENT( pResiduals, 0 ) = ( ELEMENT( pt1, 0 ) / ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 0 );
		ELEMENT( pResiduals, 0 ) = ( ELEMENT( pt1, 1 ) / ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 1 );
	}

	void CameraModel3DoF::RotatePoint(
		__in_ecount( 3 ) const DScalar< PARAM_CNT >* pPoint,
		__out_ecount( 3 ) DScalar< PARAM_CNT >* pRotatedPoint ) const
	{
		MulM33V3( m_pCamera->r.Elements(), pPoint, pRotatedPoint );
	}

	void CameraModel3DoF::TranslatePoint( __inout_ecount( 3 ) DScalar< PARAM_CNT >* pPoint ) const
	{
		const Scalar* pT = m_pCamera->t.Elements();

		DScalar< PARAM_CNT > translation[ 3 ] =
		{
			DScalar< PARAM_CNT >( ELEMENT( pT, 0 ), 0 ),
			DScalar< PARAM_CNT >( ELEMENT( pT, 1 ), 1 ),
			DScalar< PARAM_CNT >( ELEMENT( pT, 2 ), 2 ),
		};

		AddV3V3( pPoint, translation, pPoint );
	}

	void CameraModel3DoF::MultiplyByCalibration(
		__in_ecount( 3 ) const DScalar< PARAM_CNT >* pPoint,
		__out_ecount( 3 ) DScalar< PARAM_CNT >* pCalibratedPoint ) const
	{
		const CameraCalibration& calib = m_pCamera->k;
		Scalar K[9] = 
		{
			calib.fScale,	0,				calib.k1,
			0,				calib.fScale,	calib.k2,
			0,				0,				1
		};

		MulM33V3( K, pPoint, pCalibratedPoint );
	}

} }
