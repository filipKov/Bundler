#pragma once

#define ELEMENT( arrayPtr, index ) (*( arrayPtr + index ))

namespace LinearAlgebra {

	template < typename T1, typename T2, typename T3 >
	inline void MulM33V3( __in_ecount( 9 ) const T1* M, __in_ecount( 3 ) const T2* iv, __out_ecount( 3 ) T3* ov ) 
	{
		ELEMENT( ov, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( iv, 2 ) );
		ELEMENT( ov, 1 ) = ( ELEMENT( M, 3 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 4 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( iv, 2 ) );
		ELEMENT( ov, 2 ) = ( ELEMENT( M, 6 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 8 ) * ELEMENT( iv, 2 ) );
	}

	template < typename T1, typename T2, typename T3 >
	inline void AddV3V3( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2, __out_ecount( 3 ) T3* ov ) 
	{
		ELEMENT( ov, 0 ) = ELEMENT( v1, 0 ) + ELEMENT( v2, 0 );
		ELEMENT( ov, 1 ) = ELEMENT( v1, 1 ) + ELEMENT( v2, 1 );
		ELEMENT( ov, 2 ) = ELEMENT( v1, 2 ) + ELEMENT( v2, 2 );
	}

}