#pragma once

namespace LinearAlgebra {

	namespace Internal {
	
		template < typename T >
		struct GaussJordanEliminationClassicCore
		{
			inline static void GetRowEschelonForm( __in const size_t n, __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
				for ( size_t rowI = 0; rowI < n - 1; rowI++ )
				{
					size_t pivotRowI = FindPivot( n, A, rowI );
					SwapRows( n, A, pivotRowI, rowI );
					SwapRows( n, InvertedA, pivotRowI, rowI );
					EliminateColumn( n, A, InvertedA, rowI );
				}
			}

			inline static size_t FindPivot( __in const size_t n, __in_ecount ( n * n ) const T* A, __in const size_t diagIx )
			{
				size_t maxRow = diagIx;
				for ( size_t rowIx = diagIx + 1; rowIx < n; rowIx++ )
				{
					if ( abs( ELEMENT( A, rowIx * n + diagIx ) ) > abs( ELEMENT( A, maxRow * n + diagIx ) ) )
					{
						maxRow = rowIx;
					}
				}

				return maxRow;
			}

			inline static void SwapRows( __in const size_t n, __inout_ecount( n * n ) T* A, __in const size_t row1, __in const size_t row2 )
			{
				Containers::Buffer< T > temp;
				temp.Allocate( n );

				ShallowCopy< T >( A + row1 * n, n, temp.Data() );
				ShallowCopy< T >( A + row2 * n, n, A + row1 * n );
				ShallowCopy< T >( temp.Data(), n, A + row2 * n );
			}

			inline static void EliminateColumn( __in const size_t n, __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA, __in const size_t diagIx )
			{
				T diagInv = 1 / ELEMENT( A, diagIx * n + diagIx );
				for ( size_t rowIx = diagIx + 1; rowIx < n; rowIx++ )
				{
					T rowFactor = ELEMENT( A, rowIx * n + diagIx ) * diagInv;

					MatrixSubC< T >( 1, n - diagIx, A + rowIx * n + diagIx, A + diagIx * n + diagIx, rowFactor, A + rowIx * n + diagIx );
					MatrixSubC< T >( 1, n, InvertedA + rowIx * n, InvertedA + diagIx * n, rowFactor, InvertedA + rowIx * n );
				}
			}

			inline static void BackSubstitute( __in const size_t n, __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
				for ( int64 diagIx = ( int64 )n - 1; diagIx >= 0; diagIx-- )
				{
					T diagInverse = 1 / ELEMENT( A, diagIx * n + diagIx );

					for ( int64 rowI = 0; rowI < diagIx; rowI++ )
					{
						T rowFactor = ELEMENT( A, rowI * n + diagIx ) * diagInverse;

						MatrixSubC< T >( 1, n, InvertedA + rowI * n, InvertedA + diagIx * n, rowFactor, InvertedA + rowI * n );
					}

					MatrixMultiplyC< T >( 1, n, InvertedA + diagIx * n, diagInverse, InvertedA + diagIx * n );
				}
			}

			
		};
		
		template < bool isBig >
		struct GaussJordanEliminationImpl
		{
			template < typename T, size_t n >
			static void GetInverse( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA );
		};

		template < >
		struct GaussJordanEliminationImpl< true >
		{
			template < typename T, size_t n >
			static void GetInverse( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
				MatrixIdentity< T >( n, InvertedA );

				Internal::GaussJordanEliminationClassicCore< T >::GetRowEschelonForm( n, A, InvertedA );
				Internal::GaussJordanEliminationClassicCore< T >::BackSubstitute( n, A, InvertedA );
			}
		};
	
		template < >
		struct GaussJordanEliminationImpl< false >
		{
			template < typename T, size_t n >
			static void GetInverse( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
				MatrixIdentity< T, n >( InvertedA );

				GaussJordanEliminationUnrolledCore< T, n >::GetRowEschelonForm< n >( A, InvertedA );
				GaussJordanEliminationUnrolledCore< T, n >::BackSubstitute< n >( A, InvertedA );
			}
		};
	}

	template < typename T, size_t n >
	void MatrixInvertGJ( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
	{
		Internal::GaussJordanEliminationImpl< ( n > size_t(7) ) >::GetInverse< T, n >( A, InvertedA );
	}

	template < typename T >
	void MatrixInvertGJ( __in const size_t n, __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
	{
		MatrixIdentity< T >( n, InvertedA );

		Internal::GaussJordanEliminationClassicCore< T >::GetRowEschelonForm( n, A, InvertedA );
		Internal::GaussJordanEliminationClassicCore< T >::BackSubstitute( n, A, InvertedA );
	}

}
