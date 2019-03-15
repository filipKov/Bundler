#pragma once

namespace LinearAlgebra {

	// ToDo: move to separate file for fast Matrix3x4

	template < typename T1, typename T2, typename T3 >
	__forceinline void M34MulV4( __in_ecount( 12 ) const T1* M, __in_ecount( 4 ) const T2* inV, __out_ecount( 3 ) T3* outV )
	{
		_ASSERT_EXPR( (void*)inV != (void*)outV, "Input and output vectors must be different!" );

		ELEMENT( outV, 0 ) = ( ELEMENT( M, 0 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 1 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 2 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 3 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 1 ) = ( ELEMENT( M, 4 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 5 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 6 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 7 ) * ELEMENT( inV, 3 ) );
		ELEMENT( outV, 2 ) = ( ELEMENT( M, 8 ) * ELEMENT( inV, 0 ) ) + ( ELEMENT( M, 9 ) * ELEMENT( inV, 1 ) ) + ( ELEMENT( M, 10 ) * ELEMENT( inV, 2 ) ) + ( ELEMENT( M, 11 ) * ELEMENT( inV, 3 ) );
	}


	namespace Internal {

		enum class MatrixMultiplicationType
		{
			NORMAL,
			A_TRANSPOSED,
			B_TRANSPOSED,
			BOTH_TRANSPOSED
		};

		template < typename T, size_t m, size_t n, size_t p, size_t row, size_t col, size_t i >
		struct MatrixEntry
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out T* entry )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get< type >( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * row + i ) * ELEMENT( B, p * i + col ) );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out T* entry )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * i + row ) * ELEMENT( B, p * i + col ) );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out T* entry )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * row + i ) * ELEMENT( B, n * col + i ) );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out T* entry )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * i + row ) * ELEMENT( B, m * col + i ) );
			}
		};

		template < typename T, size_t m, size_t n, size_t p, size_t row, size_t col >
		struct MatrixEntry< T, m, n, p, row, col, 0 >
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out T* entry )
			{
				( *entry ) = ELEMENT( A, n * row ) * ELEMENT( B, col );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out T* entry )
			{
				( *entry ) = ELEMENT( A, row ) * ELEMENT( B, col );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out T* entry )
			{
				( *entry ) = ELEMENT( A, n * row ) * ELEMENT( B, n * col );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out T* entry )
			{
				( *entry ) = ELEMENT( A, row ) * ELEMENT( B, m * col );
			}
		};


		template < typename T, size_t m, size_t n, size_t p, size_t row, size_t col >
		struct MatrixRow
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, col, n - 1 >::Get< type >( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get< type >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out_ecount( n * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, col, m - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, col, n - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out_ecount( n * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, col, m - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n, size_t p, size_t row >
		struct MatrixRow< T, m, n, p, row, 0 >
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, 0, n - 1 >::Get< type >( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get< type >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out_ecount( n * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, 0, m - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, 0, n - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out_ecount( n * p ) T* C )
			{
				MatrixEntry< T, m, n, p, row, 0, m - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n, size_t p  >
		struct MatrixRow< T, m, n, p, 0, 0 >
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, 0, 0, n - 1 >::Get< type >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out_ecount( n * p ) T* C )
			{
				MatrixEntry< T, m, n, p, 0, 0, m - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out_ecount( m * p ) T* C )
			{
				MatrixEntry< T, m, n, p, 0, 0, n - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C );
			}

			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out_ecount( n * p ) T* C )
			{
				MatrixEntry< T, m, n, p, 0, 0, m - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C );
			}
		};
	}

	namespace Internal {

		template < typename T, size_t m, size_t n, size_t row, size_t col >
		struct MatrixAdder
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, row * n + col ) = ELEMENT( A, row * n + col ) + ELEMENT( B, row * n + col );
				MatrixAdder< T, m, n, row, col - 1 >::Get( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n, size_t row >
		struct MatrixAdder< T, m, n, row, 0 >
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, row * n ) = ELEMENT( A, row * n ) + ELEMENT( B, row * n );
				MatrixAdder< T, m, n, row - 1, n - 1 >::Get( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n >
		struct MatrixAdder< T, m, n, 0, 0 >
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, 0 ) = ELEMENT( A, 0 ) + ELEMENT( B, 0 );
			}
		};

		template < typename T, size_t m, size_t n, size_t row, size_t col >
		struct MatrixSubtractor
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, row * n + col ) = ELEMENT( A, row * n + col ) - ELEMENT( B, row * n + col );
				MatrixSubtractor< T, m, n, row, col - 1 >::Get( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n, size_t row >
		struct MatrixSubtractor< T, m, n, row, 0 >
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, row * n ) = ELEMENT( A, row * n ) - ELEMENT( B, row * n );
				MatrixSubtractor< T, m, n, row - 1, n - 1 >::Get( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n >
		struct MatrixSubtractor< T, m, n, 0, 0 >
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, 0 ) = ELEMENT( A, 0 ) - ELEMENT( B, 0 );
			}
		};

		template < typename T, size_t m, size_t n, size_t row, size_t col >
		struct MatrixElementWiseMultiplicator
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, row * n + col ) = ELEMENT( A, row * n + col ) * ELEMENT( B, row * n + col );
				MatrixElementWiseMultiplicator< T, m, n, row, col - 1 >::Get( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n, size_t row >
		struct MatrixElementWiseMultiplicator< T, m, n, row, 0 >
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, row * n ) = ELEMENT( A, row * n ) * ELEMENT( B, row * n );
				MatrixElementWiseMultiplicator< T, m, n, row - 1, n - 1 >::Get( A, B, C );
			}
		};

		template < typename T, size_t m, size_t n >
		struct MatrixElementWiseMultiplicator< T, m, n, 0, 0 >
		{
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
			{
				ELEMENT( C, 0 ) = ELEMENT( A, 0 ) * ELEMENT( B, 0 );
			}
		};
	}

	template < typename T, size_t m, size_t n, size_t p >
	__forceinline void MatrixMultiply( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C )
	{
		Internal::MatrixRow< T, m, n, p, m - 1, p - 1 >::Get< Internal::MatrixMultiplicationType::NORMAL >( A, B, C );
	}

	template < typename T, size_t m, size_t n, size_t p >
	__forceinline void MatrixMultiplyAtB( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out_ecount( n * p ) T* C )
	{
		Internal::MatrixRow< T, m, n, p, n - 1, p - 1 >::Get< Internal::MatrixMultiplicationType::A_TRANSPOSED >( A, B, C );
	}

	template < typename T, size_t m, size_t n, size_t p >
	__forceinline void MatrixMultiplyABt( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out_ecount( m * p ) T* C )
	{
		Internal::MatrixRow< T, m, n, p, m - 1, p - 1 >::Get< Internal::MatrixMultiplicationType::B_TRANSPOSED >( A, B, C );
	}

	template < typename T, size_t m, size_t n, size_t p >
	__forceinline void MatrixMultiplyAtBt( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out_ecount( n * p ) T* C )
	{
		Internal::MatrixRow< T, m, n, p, n - 1, p - 1 >::Get< Internal::MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C );
	}

	template < typename T, size_t m, size_t n >
	__forceinline void MatrixAdd( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
	{
		Internal::MatrixAdder< T, m, n, m - 1, n - 1 >::Get( A, B, C );
	}

	template < typename T, size_t m, size_t n >
	__forceinline void MatrixSub( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
	{
		Internal::MatrixSubtractor< T, m, n, m - 1, n - 1 >::Get( A, B, C );
	}

	template < typename T, size_t m, size_t n >
	__forceinline void MatrixMultiplyElementWise( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C )
	{
		Internal::MatrixElementWiseMultiplicator< T, m, n, m - 1, n - 1 >::Get( A, B, C );
	}

}
