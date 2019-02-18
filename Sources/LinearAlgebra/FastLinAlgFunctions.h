#pragma once

namespace LinearAlgebra {

	template < typename T >
	inline void IdentityM33( __out_ecount( 9 ) T* M ) {
		ByteFill< T >( 0, 9, M );
		ELEMENT( M, 0 ) = T( 1 );
		ELEMENT( M, 4 ) = T( 1 );
		ELEMENT( M, 8 ) = T( 1 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void AddV3V3( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2, __out_ecount( 3 ) T3* ov )
	{
		ELEMENT( ov, 0 ) = ELEMENT( v1, 0 ) + ELEMENT( v2, 0 );
		ELEMENT( ov, 1 ) = ELEMENT( v1, 1 ) + ELEMENT( v2, 1 );
		ELEMENT( ov, 2 ) = ELEMENT( v1, 2 ) + ELEMENT( v2, 2 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void AddM33M33( __in_ecount( 9 ) const T1* M, __in_ecount( 9 ) const T2* K, __out_ecount( 3 ) T3* oM )
	{
		ELEMENT( oM, 0 ) = ELEMENT( M, 0 ) + ELEMENT( K, 0 );
		ELEMENT( oM, 1 ) = ELEMENT( M, 1 ) + ELEMENT( K, 1 );
		ELEMENT( oM, 2 ) = ELEMENT( M, 2 ) + ELEMENT( K, 2 );
		ELEMENT( oM, 3 ) = ELEMENT( M, 3 ) + ELEMENT( K, 3 );
		ELEMENT( oM, 4 ) = ELEMENT( M, 4 ) + ELEMENT( K, 4 );
		ELEMENT( oM, 5 ) = ELEMENT( M, 5 ) + ELEMENT( K, 5 );
		ELEMENT( oM, 6 ) = ELEMENT( M, 6 ) + ELEMENT( K, 6 );
		ELEMENT( oM, 7 ) = ELEMENT( M, 7 ) + ELEMENT( K, 7 );
		ELEMENT( oM, 8 ) = ELEMENT( M, 8 ) + ELEMENT( K, 8 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void MulM33C( __in_ecount( 9 ) const T1* M, __in const T2 c, __out_ecount( 9 ) T3* oM ) {
		ELEMENT( oM, 0 ) = ELEMENT( M, 0 ) * c;
		ELEMENT( oM, 1 ) = ELEMENT( M, 1 ) * c;
		ELEMENT( oM, 2 ) = ELEMENT( M, 2 ) * c;
		ELEMENT( oM, 3 ) = ELEMENT( M, 3 ) * c;
		ELEMENT( oM, 4 ) = ELEMENT( M, 4 ) * c;
		ELEMENT( oM, 5 ) = ELEMENT( M, 5 ) * c;
		ELEMENT( oM, 6 ) = ELEMENT( M, 6 ) * c;
		ELEMENT( oM, 7 ) = ELEMENT( M, 7 ) * c;
		ELEMENT( oM, 8 ) = ELEMENT( M, 8 ) * c;
	}

	template < typename T1, typename T2, typename T3 >
	inline void MulM33V3( __in_ecount( 9 ) const T1* M, __in_ecount( 3 ) const T2* iv, __out_ecount( 3 ) T3* ov ) 
	{
		_ASSERT_EXPR( iv != ov, "Input and output vectors must be different!" );

		ELEMENT( ov, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( iv, 2 ) );
		ELEMENT( ov, 1 ) = ( ELEMENT( M, 3 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 4 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( iv, 2 ) );
		ELEMENT( ov, 2 ) = ( ELEMENT( M, 6 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 8 ) * ELEMENT( iv, 2 ) );
	}
	
	template < typename T1, typename T2, typename T3 >
	inline void MulM33M33( __in_ecount( 9 ) const T1* M, __in_ecount( 9 ) const T2* K, __out_ecount( 3 ) T3* oM )
	{
		_ASSERT_EXPR( ( M != oM ) && ( K != oM ), "Input and output matrices must be different!" );

		ELEMENT( oM, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( K, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( K, 3 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( K, 6 ) );
		ELEMENT( oM, 1 ) = ( ELEMENT( M, 0 ) * ELEMENT( K, 1 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( K, 4 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( K, 7 ) );
		ELEMENT( oM, 2 ) = ( ELEMENT( M, 0 ) * ELEMENT( K, 2 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( K, 5 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( K, 8 ) );

		ELEMENT( oM, 3 ) = ( ELEMENT( M, 3 ) * ELEMENT( K, 0 ) ) + ( ELEMENT( M, 4 ) * ELEMENT( K, 3 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( K, 6 ) );
		ELEMENT( oM, 4 ) = ( ELEMENT( M, 3 ) * ELEMENT( K, 1 ) ) + ( ELEMENT( M, 4 ) * ELEMENT( K, 4 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( K, 7 ) );
		ELEMENT( oM, 5 ) = ( ELEMENT( M, 3 ) * ELEMENT( K, 2 ) ) + ( ELEMENT( M, 4 ) * ELEMENT( K, 5 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( K, 8 ) );

		ELEMENT( oM, 6 ) = ( ELEMENT( M, 6 ) * ELEMENT( K, 0 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( K, 3 ) ) + ( ELEMENT( M, 8 ) * ELEMENT( K, 6 ) );
		ELEMENT( oM, 7 ) = ( ELEMENT( M, 6 ) * ELEMENT( K, 1 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( K, 4 ) ) + ( ELEMENT( M, 8 ) * ELEMENT( K, 7 ) );
		ELEMENT( oM, 8 ) = ( ELEMENT( M, 6 ) * ELEMENT( K, 2 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( K, 5 ) ) + ( ELEMENT( M, 8 ) * ELEMENT( K, 8 ) );
	}

}
