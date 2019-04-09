#pragma once

namespace LinearAlgebra {

	// ----------------------------------------------------
	//   Basic Vectors
	// ----------------------------------------------------
	
	template < typename T1 >
	__forceinline void V4Zero( __out_ecount( 4 ) T1* v ) restrict( amp )
	{
		Containers::ArrayUtils< T1 >::Fill< 4 >( T1( 0 ), v );
	}
	
	template < typename T1 >
	__forceinline void V4UnitX( __out_ecount( 4 ) T1* v ) restrict( amp )
	{
		V4Zero( v );
		ELEMENT( v, 0 ) = T1( 1 );
	}
	
	template < typename T1 >
	__forceinline void V4UnitY( __out_ecount( 4 ) T1* v ) restrict( amp )
	{
		V4Zero( v );
		ELEMENT( v, 1 ) = T1( 1 );
	}
	
	template < typename T1 >
	__forceinline void V4UnitZ( __out_ecount( 4 ) T1* v ) restrict( amp )
	{
		V4Zero( v );
		ELEMENT( v, 2 ) = T1( 1 );
	}
	
	template < typename T1 >
	__forceinline void V4UnitW( __out_ecount( 4 ) T1* v ) restrict( amp )
	{
		V4Zero( v );
		ELEMENT( v, 3 ) = T1( 1 );
	}
	
	// ----------------------------------------------------
	//   Arithmetic operations
	// ----------------------------------------------------
	
	template < typename T1, typename T2 >
	__forceinline void V4Neg( __in_ecount( 4 ) const T1* v1, __out_ecount( 4 ) T2* ov ) restrict( amp )
	{
		ELEMENT( ov, 0 ) = -ELEMENT( v1, 0 );
		ELEMENT( ov, 1 ) = -ELEMENT( v1, 1 );
		ELEMENT( ov, 2 ) = -ELEMENT( v1, 2 );
		ELEMENT( ov, 3 ) = -ELEMENT( v1, 3 );
	}
	
	template < typename T1, typename T2, typename T3 >
	__forceinline void V4AddV4( __in_ecount( 4 ) const T1* v1, __in_ecount( 4 ) const T2* v2, __out_ecount( 4 ) T3* ov ) restrict( amp )
	{
		ELEMENT( ov, 0 ) = ELEMENT( v1, 0 ) + ELEMENT( v2, 0 );
		ELEMENT( ov, 1 ) = ELEMENT( v1, 1 ) + ELEMENT( v2, 1 );
		ELEMENT( ov, 2 ) = ELEMENT( v1, 2 ) + ELEMENT( v2, 2 );
		ELEMENT( ov, 3 ) = ELEMENT( v1, 3 ) + ELEMENT( v2, 3 );
	}
	
	template < typename T1, typename T2, typename T3 >
	__forceinline void V4SubV4( __in_ecount( 4 ) const T1* v1, __in_ecount( 4 ) const T2* v2, __out_ecount( 4 ) T3* ov ) restrict( amp )
	{
		ELEMENT( ov, 0 ) = ELEMENT( v1, 0 ) - ELEMENT( v2, 0 );
		ELEMENT( ov, 1 ) = ELEMENT( v1, 1 ) - ELEMENT( v2, 1 );
		ELEMENT( ov, 2 ) = ELEMENT( v1, 2 ) - ELEMENT( v2, 2 );
		ELEMENT( ov, 3 ) = ELEMENT( v1, 3 ) - ELEMENT( v2, 3 );
	}
	
	template < typename T1, typename T2, typename T3 >
	__forceinline void V4MulC( __in_ecount( 4 ) const T1* inV, __in const T2 c, __out_ecount( 4 ) T3* outV ) restrict( amp )
	{
		ELEMENT( outV, 0 ) = ELEMENT( inV, 0 ) * c;
		ELEMENT( outV, 1 ) = ELEMENT( inV, 1 ) * c;
		ELEMENT( outV, 2 ) = ELEMENT( inV, 2 ) * c;
		ELEMENT( outV, 3 ) = ELEMENT( inV, 3 ) * c;
	}
	
	// ----------------------------------------------------
	//   Vector products
	// ----------------------------------------------------
	
	template < typename T1, typename T2 >
	__forceinline auto V4Dot( __in_ecount( 4 ) const T1* v1, __in_ecount( 4 ) const T2* v2 ) restrict( amp )
	{
		return (
			( ELEMENT( v1, 0 ) * ELEMENT( v2, 0 ) ) +
			( ELEMENT( v1, 1 ) * ELEMENT( v2, 1 ) ) +
			( ELEMENT( v1, 2 ) * ELEMENT( v2, 2 ) ) +
			( ELEMENT( v1, 3 ) * ELEMENT( v2, 3 ) ) );
	}
	
	template < typename T1, typename T2, typename T3 >
	__forceinline void V4OuterProduct( __in_ecount( 4 ) const T1* v1, __in_ecount( 4 ) const T2* v2, __out_ecount( 16 ) T3* M ) restrict( amp )
	{
		ELEMENT( M, 0 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 0 );
		ELEMENT( M, 1 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 1 );
		ELEMENT( M, 2 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 2 );
		ELEMENT( M, 3 ) = ELEMENT( v1, 0 ) * ELEMENT( v2, 3 );
	
		ELEMENT( M, 4 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 0 );
		ELEMENT( M, 5 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 1 );
		ELEMENT( M, 6 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 2 );
		ELEMENT( M, 7 ) = ELEMENT( v1, 1 ) * ELEMENT( v2, 3 );
	
		ELEMENT( M, 8 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 0 );
		ELEMENT( M, 9 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 1 );
		ELEMENT( M, 10 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 2 );
		ELEMENT( M, 11 ) = ELEMENT( v1, 2 ) * ELEMENT( v2, 3 );
	
		ELEMENT( M, 12 ) = ELEMENT( v1, 3 ) * ELEMENT( v2, 0 );
		ELEMENT( M, 13 ) = ELEMENT( v1, 3 ) * ELEMENT( v2, 1 );
		ELEMENT( M, 14 ) = ELEMENT( v1, 3 ) * ELEMENT( v2, 2 );
		ELEMENT( M, 15 ) = ELEMENT( v1, 3 ) * ELEMENT( v2, 3 );
	}
	
	// ----------------------------------------------------
	//   Other functions
	// ----------------------------------------------------
	
	template < typename T1 >
	__forceinline auto V4Length( __in_ecount( 4 ) const T1* v ) restrict( amp )
	{
		return concurrency::precise_math::sqrt( V4Dot( v, v ) );
	}
	
	template < typename T1, typename T2 >
	__forceinline void V4Normalize( __in_ecount( 4 ) const T1* inV, __out_ecount( 4 ) T2* outV ) restrict( amp )
	{
		const T1 lengthInv = T1( 1 ) / V4Length( inV );
	
		ELEMENT( outV, 0 ) = ELEMENT( inV, 0 ) * lengthInv;
		ELEMENT( outV, 1 ) = ELEMENT( inV, 1 ) * lengthInv;
		ELEMENT( outV, 2 ) = ELEMENT( inV, 2 ) * lengthInv;
		ELEMENT( outV, 3 ) = ELEMENT( inV, 3 ) * lengthInv;
	}
	
	template < typename T1, typename T2, typename T3 >
	__forceinline void V4Project( __in_ecount( 4 ) const T1* srcV, __in_ecount( 4 ) const T2* dstV, __out_ecount( 4 ) T3* projected ) restrict( amp )
	{
		auto dot = V4Dot( srcV, dstV ) / V4Dot( dstV, dstV );
	
		ELEMENT( projected, 0 ) = dot * ELEMENT( dstV, 0 );
		ELEMENT( projected, 1 ) = dot * ELEMENT( dstV, 1 );
		ELEMENT( projected, 2 ) = dot * ELEMENT( dstV, 2 );
		ELEMENT( projected, 3 ) = dot * ELEMENT( dstV, 3 );
	}

}
