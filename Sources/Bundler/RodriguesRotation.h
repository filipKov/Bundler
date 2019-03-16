#pragma once

#include "stdafx.h"

namespace Bundler { namespace CameraModels {

	class RodriguesRotation {
	public:

		static constexpr const uint rotationParameterCount = 4;

		template < typename T >
		static inline void GetFromRotationMatrix( __in_ecount( 9 ) const T* pMatrix, __out T* pAngle, __out_ecount( 3 ) T* pAxis ) {
			ELEMENT( pAxis, 0 ) = ( ELEMENT( pMatrix, 7 ) - ELEMENT( pMatrix, 5 ) );
			ELEMENT( pAxis, 1 ) = ( ELEMENT( pMatrix, 2 ) - ELEMENT( pMatrix, 6 ) );
			ELEMENT( pAxis, 2 ) = ( ELEMENT( pMatrix, 3 ) - ELEMENT( pMatrix, 1 ) );

			T s = V3Length( pAxis );
			if ( s >= T( 1e-5 ) )
			{
				T c = ( M33Trace( pMatrix ) - T( 1 ) ) * T( 0.5 );
				c = max( T( -1.0 ), min( c, T( 1.0 ) ) );
				
				*pAngle = acos( c );
				V3MulC( pAxis, T( 1 ) / s, pAxis );
			}
			else
			{
				*pAngle = T( 0 );
			}
		}

		template < uint cameraModelParameters, uint rotationParamStartIx = 0, bool useDiff = false >
		static inline void GetRotation( __in_ecount( 4 ) const Scalar* pParams, __out_ecount( 9 ) DScalar< cameraModelParameters >* pRotationMatrix ) 
		{
			GetRotation< cameraModelParameters, rotationParamStartIx, useDiff >( *pParams, ( pParams + 1 ) );
		}

		template < uint cameraModelParameters, uint rotationParamStartIx, bool useDiff >
		static inline void GetRotation( __in const Scalar angle, __in_ecount( 3 ) const Scalar* pAxis, __out_ecount( 9 ) DScalar< cameraModelParameters >* pRotationMatrix )
		{
			DScalar< cameraModelParameters > dAngle( angle );

			DScalar< cameraModelParameters > dAxis[ 3 ] = {
				DScalar< cameraModelParameters >( ELEMENT( pAxis, 0 ) ),
				DScalar< cameraModelParameters >( ELEMENT( pAxis, 1 ) ),
				DScalar< cameraModelParameters >( ELEMENT( pAxis, 2 ) ),
			};

			if ( useDiff ) {
				dAngle.SetDiff( rotationParamStartIx + 0 );
				ELEMENT( dAxis, 0 ).SetDiff( rotationParamStartIx + 1 );
				ELEMENT( dAxis, 1 ).SetDiff( rotationParamStartIx + 2 );
				ELEMENT( dAxis, 2 ).SetDiff( rotationParamStartIx + 3 );
			}

			GetRotation< DScalar< cameraModelParameters > >( dAngle, dAxis, pRotationMatrix );
		}

		template < typename T >
		static inline void GetRotation( __in const T angle, __in_ecount( 3 ) const T* pAxis, __out_ecount( 9 ) T* pRotationMatrix ) 
		{
			T K[ 9 ] = {
				T( 0 ),					-ELEMENT( pAxis, 2 ),	ELEMENT( pAxis, 1 ),
				ELEMENT( pAxis, 2 ),	T( 0 ),					-ELEMENT( pAxis, 0 ),
				-ELEMENT( pAxis, 1 ),	ELEMENT( pAxis, 0 ),	T( 0 )
			};

			T K2[ 9 ];
			T angleCosine = cos( angle );

			M33MulC( K, sin( angle ), K );

			V3OuterProduct( pAxis, pAxis, K2 );
			M33MulC( K2, ( T( 1 ) - angleCosine ), K2 );

			M33Identity( pRotationMatrix );
			M33MulC( pRotationMatrix, angleCosine, pRotationMatrix );

			M33AddM33( K, K2, K );
			M33AddM33( pRotationMatrix, K, pRotationMatrix );
		}

	};

} }