#pragma once

namespace Bundler { namespace CameraModels {

	template < uint totalParameterCount >
	class RodriguesRotation : public IRotationFormula< 4, totalParameterCount > {
	public:

		void Initialize( __in_ecount( 4 ) const Scalar* pParams ) override 
		{
			Initialize( *pParams, ( pParams + 1 ) );
		}

		inline void Initialize( __in const Scalar angle, __in_ecount( 3 ) const Scalar* pAxis ) 
		{
			Scalar K[9] = {
				0,						-ELEMENT( pAxis, 2 ),	ELEMENT( pAxis, 1 ),
			 	ELEMENT( pAxis, 2 ),	0,						-ELEMENT( pAxis, 0 ),
			 	-ELEMENT( pAxis, 1 ),	ELEMENT( pAxis, 0 ),	0
			};
			Scalar K2[ 9 ];

			M33MulM33( K, K, K2 );
			M33MulC( K2, ( Scalar( 1 ) - cos( angle ) ), K2 );

			M33MulC( K, sin( angle ), K );

			M33AddM33( K, K2, K );

			M33Identity( mR );
			M33AddM33( mR, K, mR );
		}

		inline void RotatePoint(
			__in_ecount( 3 ) DScalar< totalParameterCount >* pInputPoint,
			__out_ecount( 3 ) DScalar< totalParameterCount >* pOutputPoint ) const override 
		{
			M33MulV3( mR, pInputPoint, pOutputPoint );
		}

	protected:

		Scalar mR[9];

	};

} }