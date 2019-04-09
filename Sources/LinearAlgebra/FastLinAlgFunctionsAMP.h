#pragma once

namespace LinearAlgebra {

	// ToDo: move to separate file for fast Matrix3x4
	
	template < typename T1, typename T2, typename T3 >
	__forceinline void M34MulV4( __in_ecount( 12 ) const T1* M, __in_ecount( 4 ) const T2* inV, __out_ecount( 3 ) T3* outV ) restrict( amp )
	{
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
		
		template < typename T, uint m, uint n, uint p, uint row, uint col, uint i >
		struct MatrixEntry
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out T* entry ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get< type >( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * row + i ) * ELEMENT( B, p * i + col ) );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out T* entry ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * i + row ) * ELEMENT( B, p * i + col ) );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out T* entry ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * row + i ) * ELEMENT( B, n * col + i ) );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out T* entry ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, col, i - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, entry );
				( *entry ) += ( ELEMENT( A, n * i + row ) * ELEMENT( B, m * col + i ) );
			}
		};
		
		template < typename T, uint m, uint n, uint p, uint row, uint col >
		struct MatrixEntry< T, m, n, p, row, col, 0 >
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out T* entry ) restrict( amp )
			{
				( *entry ) = ELEMENT( A, n * row ) * ELEMENT( B, col );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out T* entry ) restrict( amp )
			{
				( *entry ) = ELEMENT( A, row ) * ELEMENT( B, col );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out T* entry ) restrict( amp )
			{
				( *entry ) = ELEMENT( A, n * row ) * ELEMENT( B, n * col );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out T* entry ) restrict( amp )
			{
				( *entry ) = ELEMENT( A, row ) * ELEMENT( B, m * col );
			}
		};
		
		
		template < typename T, uint m, uint n, uint p, uint row, uint col >
		struct MatrixRow
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, col, n - 1 >::Get< type >( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get< type >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out_ecount( n * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, col, m - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out_ecount( m * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, col, n - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out_ecount( n * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, col, m - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C + ( p * row + col ) );
				MatrixRow< T, m, n, p, row, col - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C );
			}
		};
		
		template < typename T, uint m, uint n, uint p, uint row >
		struct MatrixRow< T, m, n, p, row, 0 >
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, 0, n - 1 >::Get< type >( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get< type >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out_ecount( n * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, 0, m - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out_ecount( m * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, 0, n - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out_ecount( n * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, row, 0, m - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C + ( p * row ) );
				MatrixRow< T, m, n, p, row - 1, p - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C );
			}
		};
		
		template < typename T, uint m, uint n, uint p  >
		struct MatrixRow< T, m, n, p, 0, 0 >
		{
			template < MatrixMultiplicationType type = MatrixMultiplicationType::NORMAL >
			static __forceinline void Get( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, 0, 0, n - 1 >::Get< type >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::A_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out_ecount( n * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, 0, 0, m - 1 >::Get< MatrixMultiplicationType::A_TRANSPOSED >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::B_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out_ecount( m * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, 0, 0, n - 1 >::Get< MatrixMultiplicationType::B_TRANSPOSED >( A, B, C );
			}
		
			template <>
			static __forceinline void Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out_ecount( n * p ) T* C ) restrict( amp )
			{
				MatrixEntry< T, m, n, p, 0, 0, m - 1 >::Get< MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C );
			}
		};

	}
	
	namespace Internal {
	
		template < typename T, uint m, uint n, uint row, uint col >
		struct MatrixIterator
		{
			static __forceinline void Add( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n + col ) = ELEMENT( A, row * n + col ) + ELEMENT( B, row * n + col );
				MatrixIterator< T, m, n, row, col - 1 >::Add( A, B, C );
			}
		
			static __forceinline void AddC( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __in const T cnst, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n + col ) = ELEMENT( A, row * n + col ) + ( cnst * ELEMENT( B, row * n + col ) );
				MatrixIterator< T, m, n, row, col - 1 >::AddC( A, B, cnst, C );
			}
		
			static __forceinline void Subtract( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n + col ) = ELEMENT( A, row * n + col ) - ELEMENT( B, row * n + col );
				MatrixIterator< T, m, n, row, col - 1 >::Subtract( A, B, C );
			}
		
			static __forceinline void SubtractC( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __in const T cnst, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n + col ) = ELEMENT( A, row * n + col ) - ( cnst * ELEMENT( B, row * n + col ) );
				MatrixIterator< T, m, n, row, col - 1 >::SubtractC( A, B, cnst, C );
			}
		
			static __forceinline void MulElementWise( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n + col ) = ELEMENT( A, row * n + col ) * ELEMENT( B, row * n + col );
				MatrixIterator< T, m, n, row, col - 1 >::MulElementWise( A, B, C );
			}
		
			static __forceinline void MultiplyByCnst( __in_ecount( m * n ) const T* A, __in const T cnst, __out_ecount( m * n ) T* B ) restrict( amp )
			{
				ELEMENT( B, row * n + col ) = ELEMENT( A, row * n + col ) * cnst;
				MatrixIterator< T, m, n, row, col - 1 >::MultiplyByCnst( A, cnst, B );
			}
		
			static __forceinline T FrobeniusNorm( __in_ecount( m * n ) const T* A ) restrict( amp )
			{
				return MatrixIterator< T, m, n, row, col - 1 >::FrobeniusNorm( A ) + ( ELEMENT( A, row * n + col ) * ELEMENT( A, row * n + col ) );
			}
		
			static __forceinline void Negate( __in_ecount( m * n ) const T* A, __out_ecount( m * n ) T* B ) restrict( amp )
			{
				MatrixIterator< T, m, n, row, col - 1 >::Negate( A, B );
				ELEMENT( B, row * n + col ) = -ELEMENT( A, row * n + col );
			}
		};
		
		template < typename T, uint m, uint n, uint row >
		struct MatrixIterator< T, m, n, row, 0 >
		{
			static __forceinline void Add( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n ) = ELEMENT( A, row * n ) + ELEMENT( B, row * n );
				MatrixIterator< T, m, n, row - 1, n - 1 >::Add( A, B, C );
			}
		
			static __forceinline void AddC( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __in const T cnst, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n ) = ELEMENT( A, row * n ) + ( cnst * ELEMENT( B, row * n ) );
				MatrixIterator< T, m, n, row - 1, n - 1 >::AddC( A, B, cnst, C );
			}
		
			static __forceinline void Subtract( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n ) = ELEMENT( A, row * n ) - ELEMENT( B, row * n );
				MatrixIterator< T, m, n, row - 1, n - 1 >::Subtract( A, B, C );
			}
		
			static __forceinline void SubtractC( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __in const T cnst, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n ) = ELEMENT( A, row * n ) - ( cnst * ELEMENT( B, row * n ) );
				MatrixIterator< T, m, n, row - 1, n - 1 >::SubtractC( A, B, cnst, C );
			}
		
			static __forceinline void MulElementWise( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, row * n ) = ELEMENT( A, row * n ) * ELEMENT( B, row * n );
				MatrixIterator< T, m, n, row - 1, n - 1 >::MulElementWise( A, B, C );
			}
		
			static __forceinline void MultiplyByCnst( __in_ecount( m * n ) const T* A, __in const T cnst, __out_ecount( m * n ) T* B ) restrict( amp )
			{
				ELEMENT( B, row * n ) = ELEMENT( A, row * n ) * cnst;
				MatrixIterator< T, m, n, row - 1, n - 1 >::MultiplyByCnst( A, cnst, B );
			}
		
			static __forceinline T FrobeniusNorm( __in_ecount( m * n ) const T* A ) restrict( amp )
			{
				return MatrixIterator< T, m, n, row - 1, n - 1 >::FrobeniusNorm( A ) + ( ELEMENT( A, row * n ) * ELEMENT( A, row * n ) );
			}
		
			static __forceinline void Negate( __in_ecount( m * n ) const T* A, __out_ecount( m * n ) T* B ) restrict( amp )
			{
				MatrixIterator< T, m, n, row - 1, n - 1 >::Negate( A, B );
				ELEMENT( B, row * n ) = -ELEMENT( A, row * n );
			}
		};
		
		template < typename T, uint m, uint n >
		struct MatrixIterator< T, m, n, 0, 0 >
		{
			static __forceinline void Add( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, 0 ) = ELEMENT( A, 0 ) + ELEMENT( B, 0 );
			}
		
			static __forceinline void AddC( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __in const T cnst, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, 0 ) = ELEMENT( A, 0 ) + ( cnst * ELEMENT( B, 0 ) );
			}
		
			static __forceinline void Subtract( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, 0 ) = ELEMENT( A, 0 ) - ELEMENT( B, 0 );
			}
		
			static __forceinline void SubtractC( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __in const T cnst, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, 0 ) = ELEMENT( A, 0 ) - ( cnst * ELEMENT( B, 0 ) );
			}
		
			static __forceinline void MulElementWise( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
			{
				ELEMENT( C, 0 ) = ELEMENT( A, 0 ) * ELEMENT( B, 0 );
			}
		
			static __forceinline void MultiplyByCnst( __in_ecount( m * n ) const T* A, __in const T cnst, __out_ecount( m * n ) T* B ) restrict( amp )
			{
				ELEMENT( B, 0 ) = ELEMENT( A, 0 ) * cnst;
			}
		
			static __forceinline T FrobeniusNorm( __in_ecount( m * n ) const T* A ) restrict( amp )
			{
				return ELEMENT( A, 0 ) * ELEMENT( A, 0 );
			}
		
			static __forceinline void Negate( __in_ecount( m * n ) const T* A, __out_ecount( m * n ) T* B ) restrict( amp )
			{
				ELEMENT( B, 0 ) = -ELEMENT( A, 0 );
			}
		};

	}
	
	namespace Internal {
	
		template < typename T, uint n, uint diagIx >
		struct MatrixDiagonalIteratorImpl
		{
			static __forceinline void SetTo( __in const T cnst, __out_ecount( n * n ) T* A ) restrict( amp )
			{
				ELEMENT( A, diagIx * n + diagIx ) = cnst;
				MatrixDiagonalIteratorImpl< T, n, diagIx - 1 >::SetTo( cnst, A );
			}
		
			static __forceinline void MultiplyBy( __in const T cnst, __inout_ecount( n * n ) T* A ) restrict( amp )
			{
				ELEMENT( A, diagIx * n + diagIx ) *= cnst;
				MatrixDiagonalIteratorImpl< T, n, diagIx - 1 >::MultiplyBy( cnst, A );
			}
		};
		
		template < typename T, uint n >
		struct MatrixDiagonalIteratorImpl< T, n, 0 >
		{
			static __forceinline void SetTo( __in const T cnst, __out_ecount( n * n ) T* A ) restrict( amp )
			{
				ELEMENT( A, 0 ) = cnst;
			}
		
			static __forceinline void MultiplyBy( __in const T cnst, __inout_ecount( n * n ) T* A ) restrict( amp )
			{
				ELEMENT( A, 0 ) *= cnst;
			}
		};
		
		template < typename T, uint n >
		struct MatrixDiagonalIterator
		{
			static __forceinline void SetTo( __in const T cnst, __out_ecount( n * n ) T* A ) restrict( amp )
			{
				MatrixDiagonalIteratorImpl< T, n, n - 1 >::SetTo( cnst, A );
			}
		
			static __forceinline void MultiplyBy( __in const T cnst, __inout_ecount( n * n ) T* A ) restrict( amp )
			{
				MatrixDiagonalIteratorImpl< T, n, n - 1 >::MultiplyBy( cnst, A );
			}
		};
	
	}
	
	namespace Internal {
	
		template < typename T, uint n, uint i >
		struct VectorIteratorImpl
		{
			static __forceinline void Dot( __in_ecount( n ) const T* v1, __in_ecount( n ) const T* v2, __out T* res ) restrict( amp )
			{
				VectorIteratorImpl< T, n, i - 1 >::Dot( v1, v2, res );
				( *res ) += ELEMENT( v1, i ) * ELEMENT( v2, i );
			}
		};
		
		template < typename T, uint n >
		struct VectorIteratorImpl< T, n, 0 >
		{
			static __forceinline void Dot( __in_ecount( n ) const T* v1, __in_ecount( n ) const T* v2, __out T* res ) restrict( amp )
			{
				( *res ) += ELEMENT( v1, 0 ) * ELEMENT( v2, 0 );
			}
		};
		
		template < typename T, uint n >
		struct VectorIterator
		{
			static __forceinline T Dot( __in_ecount( n ) const T* v1, __in_ecount( n ) const T* v2 ) restrict( amp )
			{
				T dotProd = T( 0 );
				VectorIteratorImpl< T, n, n - 1 >::Dot( v1, v2, &dotProd );
				return dotProd;
			}
		};
	
	}
	
	template < typename T, uint m, uint n, uint p >
	__forceinline void MatrixMultiply( __in_ecount( m * n ) const T* A, __in_ecount( n * p ) const T* B, __out_ecount( m * p ) T* C ) restrict( amp )
	{
		Internal::MatrixRow< T, m, n, p, m - 1, p - 1 >::Get< Internal::MatrixMultiplicationType::NORMAL >( A, B, C );
	}
	
	template < typename T, uint m, uint n, uint p >
	__forceinline void MatrixMultiplyAtB( __in_ecount( m * n ) const T* A, __in_ecount( m * p ) const T* B, __out_ecount( n * p ) T* C ) restrict( amp )
	{
		Internal::MatrixRow< T, m, n, p, n - 1, p - 1 >::Get< Internal::MatrixMultiplicationType::A_TRANSPOSED >( A, B, C );
	}
	
	template < typename T, uint m, uint n, uint p >
	__forceinline void MatrixMultiplyABt( __in_ecount( m * n ) const T* A, __in_ecount( p * n ) const T* B, __out_ecount( m * p ) T* C ) restrict( amp )
	{
		Internal::MatrixRow< T, m, n, p, m - 1, p - 1 >::Get< Internal::MatrixMultiplicationType::B_TRANSPOSED >( A, B, C );
	}
	
	template < typename T, uint m, uint n, uint p >
	__forceinline void MatrixMultiplyAtBt( __in_ecount( m * n ) const T* A, __in_ecount( p * m ) const T* B, __out_ecount( n * p ) T* C ) restrict( amp )
	{
		Internal::MatrixRow< T, m, n, p, n - 1, p - 1 >::Get< Internal::MatrixMultiplicationType::BOTH_TRANSPOSED >( A, B, C );
	}
	
	template < typename T, uint m, uint n >
	__forceinline void MatrixAdd( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
	{
		Internal::MatrixIterator< T, m, n, m - 1, n - 1 >::Add( A, B, C );
	}
	
	template < typename T, uint m, uint n >
	__forceinline void MatrixAddC( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __in const T cnst, __out_ecount( m * n ) T* C ) restrict( amp )
	{
		Internal::MatrixIterator< T, m, n, m - 1, n - 1 >::AddC( A, B, cnst, C );
	}
	
	template < typename T, uint m, uint n >
	__forceinline void MatrixSub( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
	{
		Internal::MatrixIterator< T, m, n, m - 1, n - 1 >::Subtract( A, B, C );
	}
	
	template < typename T, uint m, uint n >
	__forceinline void MatrixSubC( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __in const T cnst, __out_ecount( m * n ) T* C ) restrict( amp )
	{
		Internal::MatrixIterator< T, m, n, m - 1, n - 1 >::SubtractC( A, B, cnst, C );
	}
	
	template < typename T, uint m, uint n >
	__forceinline void MatrixMultiplyElementWise( __in_ecount( m * n ) const T* A, __in_ecount( m * n ) const T* B, __out_ecount( m * n ) T* C ) restrict( amp )
	{
		Internal::MatrixIterator< T, m, n, m - 1, n - 1 >::MulElementWise( A, B, C );
	}
	
	template < typename T, uint n >
	__forceinline void MatrixIdentity( __out_ecount( n * n ) T* A ) restrict( amp )
	{
		Containers::ArrayUtils< T >::Fill< n * n >( T( 0 ), A );
		Internal::MatrixDiagonalIterator< T, n >::SetTo( T( 1 ), A );
	}
	
	template < typename T, uint m, uint n >
	__forceinline void MatrixMultiplyC( __in_ecount( m * n ) const T* A, __in const T cnst, __out_ecount( m * n ) T* B ) restrict( amp )
	{
		Internal::MatrixIterator< T, m, n, m - 1, n - 1 >::MultiplyByCnst( A, cnst, B );
	}
	
	template < typename T, uint n >
	__forceinline void MatrixMultiplyDiagonal( __in const T cnst, __inout_ecount( n * n ) T* A ) restrict( amp )
	{
		Internal::MatrixDiagonalIterator< T, n >::MultiplyBy( cnst, A );
	}
	
	template < typename T, uint m, uint n >
	__forceinline T MatrixFrobeniusNorm( __in_ecount( m * n ) const T* A ) restrict( amp )
	{
		return concurrency::precise_math::sqrt( Internal::MatrixIterator< T, m, n, m - 1, n - 1 >::FrobeniusNorm( A ) );
	}
	
	template < typename T, uint m, uint n >
	__forceinline void MatrixNegate( __in_ecount( m * n ) const T* A, __out_ecount( m * n ) T* B ) restrict( amp )
	{
		Internal::MatrixIterator< T, m, n, m - 1, n - 1 >::Negate( A, B );
	}
	
	template < typename T, uint n >
	__forceinline T VectorDot( __in_ecount( n ) const T* v1, __in_ecount( n ) const T* v2 ) restrict( amp )
	{
		return Internal::VectorIterator< T, n >::Dot( v1, v2 );
	}

}
