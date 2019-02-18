#pragma once

namespace LinearAlgebra {

	template < typename T >
	inline void M33Identity( __out_ecount( 9 ) T* M ) {
		ByteFill< T >( 0, 9, M );
		ELEMENT( M, 0 ) = T( 1 );
		ELEMENT( M, 4 ) = T( 1 );
		ELEMENT( M, 8 ) = T( 1 );
	}

	template < typename T >
	inline void M44Identity( __out_ecount( 16 ) T* M ) {
		ByteFill< T >( 0, 16, M );
		ELEMENT( M, 0 ) = T( 1 );
		ELEMENT( M, 5 ) = T( 1 );
		ELEMENT( M, 10 ) = T( 1 );
		ELEMENT( M, 15 ) = T( 1 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void V3AddV3( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2, __out_ecount( 3 ) T3* ov )
	{
		ELEMENT( ov, 0 ) = ELEMENT( v1, 0 ) + ELEMENT( v2, 0 );
		ELEMENT( ov, 1 ) = ELEMENT( v1, 1 ) + ELEMENT( v2, 1 );
		ELEMENT( ov, 2 ) = ELEMENT( v1, 2 ) + ELEMENT( v2, 2 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void V4AddV4( __in_ecount( 4 ) const T1* v1, __in_ecount( 4 ) const T2* v2, __out_ecount( 4 ) T3* ov )
	{
		ELEMENT( ov, 0 ) = ELEMENT( v1, 0 ) + ELEMENT( v2, 0 );
		ELEMENT( ov, 1 ) = ELEMENT( v1, 1 ) + ELEMENT( v2, 1 );
		ELEMENT( ov, 2 ) = ELEMENT( v1, 2 ) + ELEMENT( v2, 2 );
		ELEMENT( ov, 3 ) = ELEMENT( v1, 3 ) + ELEMENT( v2, 3 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void M33AddM33( __in_ecount( 9 ) const T1* M, __in_ecount( 9 ) const T2* K, __out_ecount( 3 ) T3* oM )
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
	inline void V3MulC( __in_ecount( 3 ) const T1* inV, __in const T2 c, __out_ecount( 3 ) T3* outV )
	{
		ELEMENT( outV, 0 ) = ELEMENT( inV, 0 ) * c;
		ELEMENT( outV, 1 ) = ELEMENT( inV, 1 ) * c;
		ELEMENT( outV, 2 ) = ELEMENT( inV, 2 ) * c;
	}

	template < typename T1, typename T2, typename T3 >
	inline void V4MulC( __in_ecount( 4 ) const T1* inV, __in const T2 c, __out_ecount( 4 ) T3* outV )
	{
		ELEMENT( outV, 0 ) = ELEMENT( inV, 0 ) * c;
		ELEMENT( outV, 1 ) = ELEMENT( inV, 1 ) * c;
		ELEMENT( outV, 2 ) = ELEMENT( inV, 2 ) * c;
		ELEMENT( outV, 3 ) = ELEMENT( inV, 3 ) * c;
	}

	template < typename T1, typename T2, typename T3 >
	inline void M33MulC( __in_ecount( 9 ) const T1* M, __in const T2 c, __out_ecount( 9 ) T3* oM ) {
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
	inline void M33MulV3( __in_ecount( 9 ) const T1* M, __in_ecount( 3 ) const T2* iv, __out_ecount( 3 ) T3* ov ) 
	{
		_ASSERT_EXPR( iv != ov, "Input and output vectors must be different!" );

		ELEMENT( ov, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( iv, 2 ) );
		ELEMENT( ov, 1 ) = ( ELEMENT( M, 3 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 4 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( iv, 2 ) );
		ELEMENT( ov, 2 ) = ( ELEMENT( M, 6 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 8 ) * ELEMENT( iv, 2 ) );
	}
	
	template < typename T1, typename T2, typename T3 >
	inline void M33MulM33( __in_ecount( 9 ) const T1* M, __in_ecount( 9 ) const T2* K, __out_ecount( 3 ) T3* oM )
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

	template < typename T1, typename T2, typename T3 >
	inline void M34MulV4( __in_ecount( 12 ) const T1* M, __in_ecount( 4 ) const T2* inV, __out_ecount( 3 ) T3* outV )
	{
		_ASSERT_EXPR( inV != outV, "Input and output vectors must be different!" );

		ELEMENT( outV, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 3 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 1 ) = ( ELEMENT( M, 4 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 6 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 2 ) = ( ELEMENT( M, 8 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 9 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 10 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 11 ) * ELEMENT( inV, 3 ) );
	}

	template < typename T1, typename T2, typename T3 >
	inline void M44MulV4( __in_ecount( 16 ) const T1* M, __in_ecount( 4 ) const T2* inV, __out_ecount( 4 ) T3* outV )
	{
		_ASSERT_EXPR( inV != outV, "Input and output vectors must be different!" );

		ELEMENT( outV, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( inV, 0 ) )	+ ( ELEMENT( M, 1 ) * ELEMENT( inV, 1 ) )	+ ( ELEMENT( M, 2 ) * ELEMENT( inV, 2 ) )	+ ( ELEMENT( M, 3 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 1 ) = ( ELEMENT( M, 4 ) * ELEMENT( inV, 0 ) )	+ ( ELEMENT( M, 5 ) * ELEMENT( inV, 1 ) )	+ ( ELEMENT( M, 6 ) * ELEMENT( inV, 2 ) )	+ ( ELEMENT( M, 7 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 2 ) = ( ELEMENT( M, 8 ) * ELEMENT( inV, 0 ) )	+ ( ELEMENT( M, 9 ) * ELEMENT( inV, 1 ) )	+ ( ELEMENT( M, 10 ) * ELEMENT( inV, 2 ) )	+ ( ELEMENT( M, 11 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 3 ) = ( ELEMENT( M, 12 ) * ELEMENT( inV, 0 ) )	+ ( ELEMENT( M, 13 ) * ELEMENT( inV, 1 ) )	+ ( ELEMENT( M, 14 ) * ELEMENT( inV, 2 ) )	+ ( ELEMENT( M, 15 ) * ELEMENT( inV, 3 ) );
	}

	template < typename T1, typename T2 >
	inline auto V3Dot( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2 )
	{
		return (
			( ELEMENT( v1, 0 ) * ELEMENT( v2, 0 ) ) +
			( ELEMENT( v1, 1 ) * ELEMENT( v2, 1 ) ) +
			( ELEMENT( v1, 2 ) * ELEMENT( v2, 2 ) ) );
	}

	template < typename T1, typename T2 >
	inline auto V4Dot( __in_ecount( 4 ) const T1* v1, __in_ecount( 4 ) const T2* v2 ) 
	{
		return ( 
			( ELEMENT( v1, 0 ) * ELEMENT( v2, 0 ) ) +
			( ELEMENT( v1, 1 ) * ELEMENT( v2, 1 ) ) +
			( ELEMENT( v1, 2 ) * ELEMENT( v2, 2 ) ) +
			( ELEMENT( v1, 3 ) * ELEMENT( v2, 3 ) ) );
	}

	template < typename T1 >
	inline auto V3Length( __in_ecount( 3 ) const T1* v ) {
		return sqrt( V3Dot( v, v ) );
	}

	template < typename T1 >
	inline auto V4Length( __in_ecount( 4 ) const T1* v ) {
		return sqrt( V4Dot( v, v ) );
	}

	template < typename T1, typename T2 >
	inline void V3Normalize( __in_ecount( 3 ) const T1* inV, __out_ecount( 3 ) T2* outV ) {
		const T1 lengthInv = T1( 1 ) / V3Length( inV );

		_ASSERT_EXPR( lengthInv != T1( 0 ), "Cannot normalize a nullvector" );

		ELEMENT( outV, 0 ) = ELEMENT( inV, 0 ) * lengthInv;
		ELEMENT( outV, 1 ) = ELEMENT( inV, 1 ) * lengthInv;
		ELEMENT( outV, 2 ) = ELEMENT( inV, 2 ) * lengthInv;
	}
	
	template < typename T1, typename T2 >
	inline void V4Normalize( __in_ecount( 4 ) const T1* inV, __out_ecount( 4 ) T2* outV ) {
		const T1 lengthInv = T1( 1 ) / V4Length( inV );

		_ASSERT_EXPR( lengthInv != T1( 0 ), "Cannot normalize a nullvector" );

		ELEMENT( outV, 0 ) = ELEMENT( inV, 0 ) * lengthInv;
		ELEMENT( outV, 1 ) = ELEMENT( inV, 1 ) * lengthInv;
		ELEMENT( outV, 2 ) = ELEMENT( inV, 2 ) * lengthInv;
		ELEMENT( outV, 3 ) = ELEMENT( inV, 3 ) * lengthInv;
	}

	template < typename T1, typename T2, typename T3 >
	inline void V3Cross( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2, __out_ecount( 3 ) T3* outV )
	{
		ELEMENT( outV, 0 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 2 ) - ELEMENT( v1, 2 ) * ELEMENT( v2, 1 );
		ELEMENT( outV, 1 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 0 ) - ELEMENT( v1, 0 ) * ELEMENT( v2, 2 );
		ELEMENT( outV, 2 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 1 ) - ELEMENT( v1, 1 ) * ELEMENT( v2, 0 );
	}

}
