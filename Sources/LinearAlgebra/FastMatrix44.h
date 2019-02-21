#pragma once

namespace LinearAlgebra {

	// ----------------------------------------------------
	//   Basic Matrices
	// ----------------------------------------------------

	template < typename T >
	inline void M44Zero( __out_ecount( 16 ) T* M )
	{
		ByteFill< T >( 0, 16, M );
	}

	template < typename T >
	inline void M44Identity( __out_ecount( 9 ) T* M )
	{
		M44Zero( M );
		ELEMENT( M, 0 ) = T( 1 );
		ELEMENT( M, 5 ) = T( 1 );
		ELEMENT( M, 10 ) = T( 1 );
		ELEMENT( M, 15 ) = T( 1 );
	}

	// ----------------------------------------------------
	//   Arithmetic operations
	// ----------------------------------------------------

	template < typename T1, typename T2 >
	inline void M44Neg( __in_ecount( 16 ) const T1* M, __out_ecount( 16 ) T2* oM )
	{
		ELEMENT( oM, 0 ) = -ELEMENT( M, 0 );
		ELEMENT( oM, 1 ) = -ELEMENT( M, 1 );
		ELEMENT( oM, 2 ) = -ELEMENT( M, 2 );
		ELEMENT( oM, 3 ) = -ELEMENT( M, 3 );
		ELEMENT( oM, 4 ) = -ELEMENT( M, 4 );
		ELEMENT( oM, 5 ) = -ELEMENT( M, 5 );
		ELEMENT( oM, 6 ) = -ELEMENT( M, 6 );
		ELEMENT( oM, 7 ) = -ELEMENT( M, 7 );
		ELEMENT( oM, 8 ) = -ELEMENT( M, 8 );
		ELEMENT( oM, 9 ) = -ELEMENT( M, 9 );
		ELEMENT( oM, 10 ) = -ELEMENT( M, 10 );
		ELEMENT( oM, 11 ) = -ELEMENT( M, 11 );
		ELEMENT( oM, 12 ) = -ELEMENT( M, 12 );
		ELEMENT( oM, 13 ) = -ELEMENT( M, 13 );
		ELEMENT( oM, 14 ) = -ELEMENT( M, 14 );
		ELEMENT( oM, 15 ) = -ELEMENT( M, 15 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void M44AddM44( __in_ecount( 16 ) const T1* M, __in_ecount( 16 ) const T2* K, __out_ecount( 16 ) T3* oM )
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
		ELEMENT( oM, 9 ) = ELEMENT( M, 9 ) + ELEMENT( K, 9 );
		ELEMENT( oM, 10 ) = ELEMENT( M, 10 ) + ELEMENT( K, 10 );
		ELEMENT( oM, 11 ) = ELEMENT( M, 11 ) + ELEMENT( K, 11 );

		ELEMENT( oM, 12 ) = ELEMENT( M, 12 ) + ELEMENT( K, 12 );
		ELEMENT( oM, 13 ) = ELEMENT( M, 13 ) + ELEMENT( K, 13 );
		ELEMENT( oM, 14 ) = ELEMENT( M, 14 ) + ELEMENT( K, 14 );
		ELEMENT( oM, 15 ) = ELEMENT( M, 15 ) + ELEMENT( K, 15 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void M44SubM44( __in_ecount( 16 ) const T1* M, __in_ecount( 16 ) const T2* K, __out_ecount( 16 ) T3* oM )
	{
		ELEMENT( oM, 0 ) = ELEMENT( M, 0 ) - ELEMENT( K, 0 );
		ELEMENT( oM, 1 ) = ELEMENT( M, 1 ) - ELEMENT( K, 1 );
		ELEMENT( oM, 2 ) = ELEMENT( M, 2 ) - ELEMENT( K, 2 );
		ELEMENT( oM, 3 ) = ELEMENT( M, 3 ) - ELEMENT( K, 3 );

		ELEMENT( oM, 4 ) = ELEMENT( M, 4 ) - ELEMENT( K, 4 );
		ELEMENT( oM, 5 ) = ELEMENT( M, 5 ) - ELEMENT( K, 5 );
		ELEMENT( oM, 6 ) = ELEMENT( M, 6 ) - ELEMENT( K, 6 );
		ELEMENT( oM, 7 ) = ELEMENT( M, 7 ) - ELEMENT( K, 7 );

		ELEMENT( oM, 8 ) = ELEMENT( M, 8 ) - ELEMENT( K, 8 );
		ELEMENT( oM, 9 ) = ELEMENT( M, 9 ) - ELEMENT( K, 9 );
		ELEMENT( oM, 10 ) = ELEMENT( M, 10 ) - ELEMENT( K, 10 );
		ELEMENT( oM, 11 ) = ELEMENT( M, 11 ) - ELEMENT( K, 11 );

		ELEMENT( oM, 12 ) = ELEMENT( M, 12 ) - ELEMENT( K, 12 );
		ELEMENT( oM, 13 ) = ELEMENT( M, 13 ) - ELEMENT( K, 13 );
		ELEMENT( oM, 14 ) = ELEMENT( M, 14 ) - ELEMENT( K, 14 );
		ELEMENT( oM, 15 ) = ELEMENT( M, 15 ) - ELEMENT( K, 15 );
	}

	template < typename T1, typename T2, typename T3 >
	inline void M44MulC( __in_ecount( 16 ) const T1* M, __in const T2 c, __out_ecount( 16 ) T3* oM )
	{
		ELEMENT( oM, 0 ) = ELEMENT( M, 0 ) * c;
		ELEMENT( oM, 1 ) = ELEMENT( M, 1 ) * c;
		ELEMENT( oM, 2 ) = ELEMENT( M, 2 ) * c;
		ELEMENT( oM, 3 ) = ELEMENT( M, 3 ) * c;

		ELEMENT( oM, 4 ) = ELEMENT( M, 4 ) * c;
		ELEMENT( oM, 5 ) = ELEMENT( M, 5 ) * c;
		ELEMENT( oM, 6 ) = ELEMENT( M, 6 ) * c;
		ELEMENT( oM, 7 ) = ELEMENT( M, 7 ) * c;

		ELEMENT( oM, 8 ) = ELEMENT( M, 8 ) * c;
		ELEMENT( oM, 9 ) = ELEMENT( M, 9 ) * c;
		ELEMENT( oM, 10 ) = ELEMENT( M, 10 ) * c;
		ELEMENT( oM, 11 ) = ELEMENT( M, 11 ) * c;

		ELEMENT( oM, 12 ) = ELEMENT( M, 12 ) * c;
		ELEMENT( oM, 13 ) = ELEMENT( M, 13 ) * c;
		ELEMENT( oM, 14 ) = ELEMENT( M, 14 ) * c;
		ELEMENT( oM, 15 ) = ELEMENT( M, 15 ) * c;
	}

	template < typename T1, typename T2, typename T3 >
	inline void M44MulV4( __in_ecount( 16 ) const T1* M, __in_ecount( 4 ) const T2* inV, __out_ecount( 4 ) T3* outV )
	{
		_ASSERT_EXPR( (void*)inV != (void*)outV, "Input and output vectors must be different!" );

		ELEMENT( outV, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 3 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 1 ) = ( ELEMENT( M, 4 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 6 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 2 ) = ( ELEMENT( M, 8 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 9 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 10 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 11 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 3 ) = ( ELEMENT( M, 12 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 13 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 14 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 15 ) * ELEMENT( inV, 3 ) );
	}

	template < typename T1, typename T2, typename T3 >
	inline void M44MulM44( __in_ecount( 16 ) const T1* M, __in_ecount( 16 ) const T2* K, __out_ecount( 16 ) T3* oM )
	{
		_ASSERT_EXPR( ( (void*)M != (void*)oM ) && ( (void*)K != (void*)oM ), "Input and output matrices must be different!" );

		ELEMENT( oM, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( K, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( K, 4 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( K, 8 ) ) + ( ELEMENT( M, 3 ) * ELEMENT( K, 12 ) );
		ELEMENT( oM, 1 ) = ( ELEMENT( M, 0 ) * ELEMENT( K, 1 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( K, 5 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( K, 9 ) ) + ( ELEMENT( M, 3 ) * ELEMENT( K, 13 ) );
		ELEMENT( oM, 2 ) = ( ELEMENT( M, 0 ) * ELEMENT( K, 2 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( K, 6 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( K, 10 ) ) + ( ELEMENT( M, 3 ) * ELEMENT( K, 14 ) );
		ELEMENT( oM, 3 ) = ( ELEMENT( M, 0 ) * ELEMENT( K, 3 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( K, 7 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( K, 11 ) ) + ( ELEMENT( M, 3 ) * ELEMENT( K, 15 ) );

		ELEMENT( oM, 4 ) = ( ELEMENT( M, 4 ) * ELEMENT( K, 0 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( K, 4 ) ) + ( ELEMENT( M, 6 ) * ELEMENT( K, 8 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( K, 12 ) );
		ELEMENT( oM, 5 ) = ( ELEMENT( M, 4 ) * ELEMENT( K, 1 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( K, 5 ) ) + ( ELEMENT( M, 6 ) * ELEMENT( K, 9 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( K, 13 ) );
		ELEMENT( oM, 6 ) = ( ELEMENT( M, 4 ) * ELEMENT( K, 2 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( K, 6 ) ) + ( ELEMENT( M, 6 ) * ELEMENT( K, 10 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( K, 14 ) );
		ELEMENT( oM, 7 ) = ( ELEMENT( M, 4 ) * ELEMENT( K, 3 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( K, 7 ) ) + ( ELEMENT( M, 6 ) * ELEMENT( K, 11 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( K, 15 ) );

		ELEMENT( oM, 0 ) = ( ELEMENT( M, 8 ) * ELEMENT( K, 0 ) ) + ( ELEMENT( M, 9 ) * ELEMENT( K, 4 ) ) + ( ELEMENT( M, 10 ) * ELEMENT( K, 8 ) ) + ( ELEMENT( M, 11 ) * ELEMENT( K, 12 ) );
		ELEMENT( oM, 1 ) = ( ELEMENT( M, 8 ) * ELEMENT( K, 1 ) ) + ( ELEMENT( M, 9 ) * ELEMENT( K, 5 ) ) + ( ELEMENT( M, 10 ) * ELEMENT( K, 9 ) ) + ( ELEMENT( M, 11 ) * ELEMENT( K, 13 ) );
		ELEMENT( oM, 2 ) = ( ELEMENT( M, 8 ) * ELEMENT( K, 2 ) ) + ( ELEMENT( M, 9 ) * ELEMENT( K, 6 ) ) + ( ELEMENT( M, 10 ) * ELEMENT( K, 10 ) ) + ( ELEMENT( M, 11 ) * ELEMENT( K, 14 ) );
		ELEMENT( oM, 3 ) = ( ELEMENT( M, 8 ) * ELEMENT( K, 3 ) ) + ( ELEMENT( M, 9 ) * ELEMENT( K, 7 ) ) + ( ELEMENT( M, 10 ) * ELEMENT( K, 11 ) ) + ( ELEMENT( M, 11 ) * ELEMENT( K, 15 ) );

		ELEMENT( oM, 0 ) = ( ELEMENT( M, 12 ) * ELEMENT( K, 0 ) ) + ( ELEMENT( M, 13 ) * ELEMENT( K, 4 ) ) + ( ELEMENT( M, 14 ) * ELEMENT( K, 8 ) ) + ( ELEMENT( M, 15 ) * ELEMENT( K, 12 ) );
		ELEMENT( oM, 1 ) = ( ELEMENT( M, 12 ) * ELEMENT( K, 1 ) ) + ( ELEMENT( M, 13 ) * ELEMENT( K, 5 ) ) + ( ELEMENT( M, 14 ) * ELEMENT( K, 9 ) ) + ( ELEMENT( M, 15 ) * ELEMENT( K, 13 ) );
		ELEMENT( oM, 2 ) = ( ELEMENT( M, 12 ) * ELEMENT( K, 2 ) ) + ( ELEMENT( M, 13 ) * ELEMENT( K, 6 ) ) + ( ELEMENT( M, 14 ) * ELEMENT( K, 10 ) ) + ( ELEMENT( M, 15 ) * ELEMENT( K, 14 ) );
		ELEMENT( oM, 3 ) = ( ELEMENT( M, 12 ) * ELEMENT( K, 3 ) ) + ( ELEMENT( M, 13 ) * ELEMENT( K, 7 ) ) + ( ELEMENT( M, 14 ) * ELEMENT( K, 11 ) ) + ( ELEMENT( M, 15 ) * ELEMENT( K, 15 ) );
	}

	// ----------------------------------------------------
	//   Other functions
	// ----------------------------------------------------

	template< typename T1 >
	inline T1 M44Trace( __in_ecount( 16 ) const T1* M )
	{
		return ELEMENT( M, 0 ) + ELEMENT( M, 5 ) + ELEMENT( M, 10 ) + ELEMENT( M, 15 );
	}

	template < typename T1, typename T2 >
	inline void M44Transpose( __in_ecount( 9 ) const T1* M, __out_ecount( 3 ) T2* oM )
	{
		_ASSERT_EXPR( (void*)M != (void*)oM, "Input and output matrices must be different" );

		ELEMENT( oM, 0 ) = ELEMENT( M, 0 );
		ELEMENT( oM, 1 ) = ELEMENT( M, 4 );
		ELEMENT( oM, 2 ) = ELEMENT( M, 8 );
		ELEMENT( oM, 3 ) = ELEMENT( M, 12 );

		ELEMENT( oM, 4 ) = ELEMENT( M, 1 );
		ELEMENT( oM, 5 ) = ELEMENT( M, 5 );
		ELEMENT( oM, 6 ) = ELEMENT( M, 9 );
		ELEMENT( oM, 7 ) = ELEMENT( M, 13 );

		ELEMENT( oM, 8 ) = ELEMENT( M, 2 );
		ELEMENT( oM, 9 ) = ELEMENT( M, 6 );
		ELEMENT( oM, 10 ) = ELEMENT( M, 10 );
		ELEMENT( oM, 11 ) = ELEMENT( M, 14 );

		ELEMENT( oM, 12 ) = ELEMENT( M, 3 );
		ELEMENT( oM, 13 ) = ELEMENT( M, 7 );
		ELEMENT( oM, 14 ) = ELEMENT( M, 11 );
		ELEMENT( oM, 15 ) = ELEMENT( M, 15 );
	}

}