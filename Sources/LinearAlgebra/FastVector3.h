#pragma once

namespace LinearAlgebra {

	// ----------------------------------------------------
	//   Basic Vectors
	// ----------------------------------------------------

	template < typename T1 >
	__forceinline void V3Zero( __out_ecount( 3 ) T1* v ) 
	{
		ByteFill< T1 >( 0, 3, v );
	}

	template < typename T1 >
	__forceinline void V3UnitX( __out_ecount( 3 ) T1* v ) 
	{
		V3Zero( v );
		ELEMENT( v, 0 ) = T1( 1 );
	}

	template < typename T1 >
	__forceinline void V3UnitY( __out_ecount( 3 ) T1* v ) 
	{
		V3Zero( v );
		ELEMENT( v, 1 ) = T1( 1 );
	}

	template < typename T1 >
	__forceinline void V3UnitZ( __out_ecount( 3 ) T1* v ) 
	{
		V3Zero( v );
		ELEMENT( v, 2 ) = T1( 1 );
	}

	// ----------------------------------------------------
	//   Arithmetic operations
	// ----------------------------------------------------

	template < typename T1, typename T2 >
	__forceinline void V3Neg( __in_ecount( 3 ) const T1* v1, __out_ecount( 3 ) T2* ov )
	{
		ELEMENT( ov, 0 ) = -ELEMENT( v1, 0 );
		ELEMENT( ov, 1 ) = -ELEMENT( v1, 1 );
		ELEMENT( ov, 2 ) = -ELEMENT( v1, 2 );
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void V3AddV3( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2, __out_ecount( 3 ) T3* ov )
	{
		ELEMENT( ov, 0 ) = ELEMENT( v1, 0 ) + ELEMENT( v2, 0 );
		ELEMENT( ov, 1 ) = ELEMENT( v1, 1 ) + ELEMENT( v2, 1 );
		ELEMENT( ov, 2 ) = ELEMENT( v1, 2 ) + ELEMENT( v2, 2 );
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void V3SubV3( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2, __out_ecount( 3 ) T3* ov )
	{
		ELEMENT( ov, 0 ) = ELEMENT( v1, 0 ) - ELEMENT( v2, 0 );
		ELEMENT( ov, 1 ) = ELEMENT( v1, 1 ) - ELEMENT( v2, 1 );
		ELEMENT( ov, 2 ) = ELEMENT( v1, 2 ) - ELEMENT( v2, 2 );
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void V3MulC( __in_ecount( 3 ) const T1* inV, __in const T2 c, __out_ecount( 3 ) T3* outV )
	{
		ELEMENT( outV, 0 ) = ELEMENT( inV, 0 ) * c;
		ELEMENT( outV, 1 ) = ELEMENT( inV, 1 ) * c;
		ELEMENT( outV, 2 ) = ELEMENT( inV, 2 ) * c;
	}

	// ----------------------------------------------------
	//   Vector products
	// ----------------------------------------------------

	template < typename T1, typename T2 >
	__forceinline auto V3Dot( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2 )
	{
		return (
			( ELEMENT( v1, 0 ) * ELEMENT( v2, 0 ) ) +
			( ELEMENT( v1, 1 ) * ELEMENT( v2, 1 ) ) +
			( ELEMENT( v1, 2 ) * ELEMENT( v2, 2 ) ) );
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void V3Cross( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2, __out_ecount( 3 ) T3* outV )
	{
		ELEMENT( outV, 0 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 2 ) - ELEMENT( v1, 2 ) * ELEMENT( v2, 1 );
		ELEMENT( outV, 1 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 0 ) - ELEMENT( v1, 0 ) * ELEMENT( v2, 2 );
		ELEMENT( outV, 2 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 1 ) - ELEMENT( v1, 1 ) * ELEMENT( v2, 0 );
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void V3OuterProduct( __in_ecount( 3 ) const T1* v1, __in_ecount( 3 ) const T2* v2, __out_ecount( 9 ) T3* M )
	{
		ELEMENT( M, 0 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 0 );
		ELEMENT( M, 1 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 1 );
		ELEMENT( M, 2 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 2 );

		ELEMENT( M, 3 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 0 );
		ELEMENT( M, 4 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 1 );
		ELEMENT( M, 5 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 2 );

		ELEMENT( M, 6 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 0 );
		ELEMENT( M, 7 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 1 );
		ELEMENT( M, 8 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 2 );
	}

	// ----------------------------------------------------
	//   Other functions
	// ----------------------------------------------------

	template < typename T1 >
	__forceinline auto V3Length( __in_ecount( 3 ) const T1* v )
	{
		return sqrt( V3Dot( v, v ) );
	}

	template < typename T1, typename T2 >
	__forceinline void V3Normalize( __in_ecount( 3 ) const T1* inV, __out_ecount( 3 ) T2* outV )
	{
		const T1 lengthInv = T1( 1 ) / V3Length( inV );

		_ASSERT_EXPR( lengthInv != T1( 0 ), "Cannot normalize a nullvector" );

		ELEMENT( outV, 0 ) = ELEMENT( inV, 0 ) * lengthInv;
		ELEMENT( outV, 1 ) = ELEMENT( inV, 1 ) * lengthInv;
		ELEMENT( outV, 2 ) = ELEMENT( inV, 2 ) * lengthInv;
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void V3Project( __in_ecount( 3 ) const T1* srcV, __in_ecount( 3 ) const T2* dstV, __out_ecount( 3 ) T3* projected )
	{
		auto dot = V3Dot( srcV, dstV ) / V3Dot( dstV, dstV );

		ELEMENT( projected, 0 ) = dot * ELEMENT( dstV, 0 );
		ELEMENT( projected, 1 ) = dot * ELEMENT( dstV, 1 );
		ELEMENT( projected, 2 ) = dot * ELEMENT( dstV, 2 );
	}

}
