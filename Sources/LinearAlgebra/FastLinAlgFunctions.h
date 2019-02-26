#pragma once

namespace LinearAlgebra {

	// ToDo: move to separate file for fast Matrix3x4

	template < typename T1, typename T2, typename T3 >
	inline void M34MulV4( __in_ecount( 12 ) const T1* M, __in_ecount( 4 ) const T2* inV, __out_ecount( 3 ) T3* outV )
	{
		_ASSERT_EXPR( (void*)inV != (void*)outV, "Input and output vectors must be different!" );

		ELEMENT( outV, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 3 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 1 ) = ( ELEMENT( M, 4 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 6 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 2 ) = ( ELEMENT( M, 8 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 9 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 10 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 11 ) * ELEMENT( inV, 3 ) );
	}


	namespace Internal {
	
		template < typename T, size_t m, size_t n, size_t p, size_t row, size_t col, size_t i = n - 1 >
		struct MatrixEntry
		{
			static inline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out T* entry )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * row + i ) * ELEMENT( B, p * i + col ) );
			}
		};

		template < typename T, size_t m, size_t n, size_t p, size_t row, size_t col >
		struct MatrixEntry< T, m, n, p, row, col, 0 >
		{
			static inline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out T* entry )
			{
				( *entry ) = ELEMENT( A, n * row ) * ELEMENT( B, col );
			}
		};


		template < typename T, size_t m, size_t n, size_t p, size_t row, size_t col >
		struct MatrixRow
		{
			static inline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, col >::Get( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n, size_t p, size_t row >
		struct MatrixRow< T, m, n, p, row, 0 >
		{
			static inline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, 0 >::Get( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n, size_t p >
		struct MatrixRow< T, m, n, p, 0, 0 >
		{
			static inline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, 0, 0 >::Get( A, B, C );
			}
		};
	}

	template < typename T, size_t m, size_t n, size_t p >
	inline void MatrixMultiply( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C )
	{
		Internal::MatrixRow< T, m, n, p, n-1, p-1 >::Get( A, B, C );
	}

}
