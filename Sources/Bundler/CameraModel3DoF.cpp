#include "stdafx.h"
#include "ICameraModel.h"
#include "BasicCameraModel.h"
#include "CameraModel3DoF.h"

namespace Bundler { namespace CameraModels {

	void CameraModel3DoF::UpdateCamera( __in_ecount( 3 ) const Scalar* pDeltaParams ) 
	{
		Scalar* pTranslation = m_pCamera->t.Elements( );
		V3AddV3( pTranslation, pDeltaParams, pTranslation );
	}

	void CameraModel3DoF::ResetLastUpdate( __in_ecount( 3 ) const Scalar* pDeltaParams )
	{
		Scalar* pTranslation = m_pCamera->t.Elements( );
		V3SubV3( pTranslation, pDeltaParams, pTranslation );
	}

	void CameraModel3DoF::RotatePoint(
		__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
		__out_ecount( 3 ) DScalar< totalParamCount >* pRotatedPoint ) const
	{
		M33MulV3( m_pCamera->r.Elements(), pPoint, pRotatedPoint );
	}

	void CameraModel3DoF::TranslatePoint( __inout_ecount( 3 ) DScalar< totalParamCount >* pPoint ) const
	{
		const Scalar* pT = m_pCamera->t.Elements();

		DScalar< totalParamCount > translation[ 3 ] =
		{
			DScalar< totalParamCount >( ELEMENT( pT, 0 ), 0 ),
			DScalar< totalParamCount >( ELEMENT( pT, 1 ), 1 ),
			DScalar< totalParamCount >( ELEMENT( pT, 2 ), 2 ),
		};

		V3AddV3( pPoint, translation, pPoint );
	}

	void CameraModel3DoF::MultiplyByCalibration(
		__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
		__out_ecount( 3 ) DScalar< totalParamCount >* pCalibratedPoint ) const
	{
		const Scalar focalLength = m_pCamera->focalLength;
		
		ELEMENT( pCalibratedPoint, 0 ) = ELEMENT( pPoint, 0 ) * focalLength;
		ELEMENT( pCalibratedPoint, 1 ) = ELEMENT( pPoint, 1 ) * focalLength;
		ELEMENT( pCalibratedPoint, 2 ) = ELEMENT( pPoint, 2 );
	}

} }
