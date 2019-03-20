#pragma once

namespace LinearAlgebra {

	// ----------------------------------------------------
	//   Basic Matrices
	// ----------------------------------------------------

	template < typename T >
	__forceinline void M33Zero( __out_ecount( 9 ) T* M )
	{
		ByteFill< T >( 0, 9, M );
	}

	template < typename T >
	__forceinline void M33Identity( __out_ecount( 9 ) T* M )
	{
		M33Zero( M );
		ELEMENT( M, 0 ) = T( 1 );
		ELEMENT( M, 4 ) = T( 1 );
		ELEMENT( M, 8 ) = T( 1 );
	}

	// ----------------------------------------------------
	//   Arithmetic operations
	// ----------------------------------------------------

	template < typename T1, typename T2 >
	__forceinline void M33Neg( __in_ecount( 9 ) const T1* M, __out_ecount( 16 ) T2* oM )
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
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void M33AddM33( __in_ecount( 9 ) const T1* M, __in_ecount( 9 ) const T2* K, __out_ecount( 9 ) T3* oM )
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
	__forceinline void M33SubM33( __in_ecount( 9 ) const T1* M, __in_ecount( 9 ) const T2* K, __out_ecount( 9 ) T3* oM )
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
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void M33MulC( __in_ecount( 9 ) const T1* M, __in const T2 c, __out_ecount( 9 ) T3* oM )
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
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void M33MulV3( __in_ecount( 9 ) const T1* M, __in_ecount( 3 ) const T2* iv, __out_ecount( 3 ) T3* ov )
	{
		_ASSERT_EXPR( (void*)iv != (void*)ov, "Input and output vectors must be different!" );

		ELEMENT( ov, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( iv, 2 ) );
		ELEMENT( ov, 1 ) = ( ELEMENT( M, 3 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 4 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( iv, 2 ) );
		ELEMENT( ov, 2 ) = ( ELEMENT( M, 6 ) * ELEMENT( iv, 0 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( iv, 1 ) ) + ( ELEMENT( M, 8 ) * ELEMENT( iv, 2 ) );
	}

	template < typename T1, typename T2, typename T3 >
	__forceinline void M33MulM33( __in_ecount( 9 ) const T1* M, __in_ecount( 9 ) const T2* K, __out_ecount( 3 ) T3* oM )
	{
		_ASSERT_EXPR( ( (void*)M != (void*)oM ) && ( (void*)K != (void*)oM ), "Input and output matrices must be different!" );

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

	// ----------------------------------------------------
	//   Inverse and Determinant
	// ----------------------------------------------------
	template < typename T1 >
	__forceinline T1 M33Det( __in_ecount( 9 ) const T1* M )
	{
		return (
			ELEMENT( M, 0 ) * ELEMENT( M, 4 ) * ELEMENT( M, 8 ) +
			ELEMENT( M, 1 ) * ELEMENT( M, 5 ) * ELEMENT( M, 6 ) +
			ELEMENT( M, 2 ) * ELEMENT( M, 3 ) * ELEMENT( M, 7 ) -
			ELEMENT( M, 2 ) * ELEMENT( M, 4 ) * ELEMENT( M, 6 ) -
			ELEMENT( M, 1 ) * ELEMENT( M, 3 ) * ELEMENT( M, 8 ) -
			ELEMENT( M, 0 ) * ELEMENT( M, 5 ) * ELEMENT( M, 8 ) );
	}

	template < typename T1, typename T2 >
	__forceinline void M33Inverse( __in_ecount( 9 ) const T1* M, __out_ecount( 9 ) T2* MI ) 
	{
		_ASSERT_EXPR( (void*)M != (void*)MI, "Input and output matrices must be different" );

		ELEMENT( MI, 0 ) = ( ELEMENT( M, 4 ) * ELEMENT( M, 8 ) - ELEMENT( M, 5 ) * ELEMENT( M, 7 ) );
		ELEMENT( MI, 1 ) = -( ELEMENT( M, 1 ) * ELEMENT( M, 8 ) - ELEMENT( M, 2 ) * ELEMENT( M, 7 ) );
		ELEMENT( MI, 2 ) = ( ELEMENT( M, 1 ) * ELEMENT( M, 5 ) - ELEMENT( M, 2 ) * ELEMENT( M, 4 ) );

		ELEMENT( MI, 3 ) = -( ELEMENT( M, 3 ) * ELEMENT( M, 8 ) - ELEMENT( M, 5 ) * ELEMENT( M, 6 ) );
		ELEMENT( MI, 4 ) = ( ELEMENT( M, 0 ) * ELEMENT( M, 8 ) - ELEMENT( M, 2 ) * ELEMENT( M, 6 ) );
		ELEMENT( MI, 5 ) = -( ELEMENT( M, 0 ) * ELEMENT( M, 5 ) - ELEMENT( M, 2 ) * ELEMENT( M, 3 ) );

		ELEMENT( MI, 6 ) = ( ELEMENT( M, 3 ) * ELEMENT( M, 7 ) - ELEMENT( M, 4 ) * ELEMENT( M, 6 ) );
		ELEMENT( MI, 7 ) = -( ELEMENT( M, 0 ) * ELEMENT( M, 7 ) - ELEMENT( M, 1 ) * ELEMENT( M, 6 ) );
		ELEMENT( MI, 8 ) = ( ELEMENT( M, 0 ) * ELEMENT( M, 4 ) - ELEMENT( M, 1 ) * ELEMENT( M, 3 ) );

		T2 det = ELEMENT( M, 0 ) * ELEMENT( MI, 0 ) + ELEMENT( M, 1 ) * ELEMENT( MI, 3 ) + ELEMENT( M, 2 ) * ELEMENT( MI, 6 );
		_ASSERT_EXPR( det != T2( 0 ), "Matrix is singular" );

		det = T2( 1 ) / det;
		M33MulC( MI, det, MI );
	}

	// ----------------------------------------------------
	//   Other functions
	// ----------------------------------------------------

	template< typename T1 >
	__forceinline T1 M33Trace( __in_ecount( 9 ) const T1* M )
	{
		return ELEMENT( M, 0 ) + ELEMENT( M, 4 ) + ELEMENT( M, 8 );
	}

	template < typename T1, typename T2 >
	__forceinline void M33Transpose( __in_ecount( 9 ) const T1* M, __out_ecount( 3 ) T2* oM )
	{
		_ASSERT_EXPR( (void*)M != (void*)oM, "Input and output matrices must be different" );

		ELEMENT( oM, 0 ) = ELEMENT( M, 0 );
		ELEMENT( oM, 1 ) = ELEMENT( M, 3 );
		ELEMENT( oM, 2 ) = ELEMENT( M, 6 );

		ELEMENT( oM, 3 ) = ELEMENT( M, 1 );
		ELEMENT( oM, 4 ) = ELEMENT( M, 4 );
		ELEMENT( oM, 5 ) = ELEMENT( M, 7 );

		ELEMENT( oM, 6 ) = ELEMENT( M, 2 );
		ELEMENT( oM, 7 ) = ELEMENT( M, 5 );
		ELEMENT( oM, 8 ) = ELEMENT( M, 8 );
	}

}