#pragma once

namespace LinearAlgebra {

	template < typename T, size_t n >
	struct GaussJordanEliminationCore
	{
		template < size_t iter >
		static void GetRowEschelonForm( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
			constexpr const size_t rowI = n - iter;

			size_t pivotRowI = Pivoting< rowI, iter >::FindPivot( A );
			SwapRows( A, pivotRowI, rowI );
			SwapRows( InvertedA, pivotRowI, rowI );

			ColumnElimination< rowI, n - rowI - 1 >::EliminateColumn( A, InvertedA );

			GetRowEschelonForm< iter - 1 >( A, InvertedA );
		}

		template <>
		static void GetRowEschelonForm< 0 >( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
		}

		template < size_t diagIx, size_t iter >
		struct Pivoting
		{
			static size_t FindPivot( __in_ecount ( n * n ) const T* A )
			{
				const size_t maxRow = Pivoting< diagIx, iter -1 >::FindPivot( A );

				constexpr const size_t rowIx = n - iter;
				if ( abs( ELEMENT( A, rowIx * n + diagIx ) ) > abs( ELEMENT( A, maxRow * n + diagIx ) ) )
				{
					return rowIx;
				}
				return maxRow;
			}
		};

		template < size_t diagIx >
		struct Pivoting< diagIx, 0 >
		{
			static size_t FindPivot( __in_ecount ( n * n ) const T* A )
			{
				return diagIx;
			}
		};

		static void SwapRows( __inout_ecount( n * n ) T* A, __in const size_t row1, __in const size_t row2 )
		{
			T temp[n];
			ShallowCopy< T >( A + row1 * n, n, temp );
			ShallowCopy< T >( A + row2 * n, n, A + row1 * n );
			ShallowCopy< T >( temp, n, A + row2 * n );
		}

		template < size_t diagIx, size_t iter >
		struct ColumnElimination
		{
			static void EliminateColumn( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
				constexpr const size_t rowIx = iter + diagIx;
				T rowFactor = ELEMENT( A, rowIx * n + diagIx ) / ELEMENT( A, diagIx * n + diagIx );

				MatrixSubC< T, 1, n - diagIx >( A + rowIx * n + diagIx, A + diagIx * n + diagIx, rowFactor, A + rowIx * n + diagIx );
				MatrixSubC< T, 1, n >( InvertedA + rowIx * n, InvertedA + diagIx * n, rowFactor, InvertedA + rowIx * n );

				ColumnElimination< diagIx, iter - 1 >::EliminateColumn( A, InvertedA );
			}
		};

		template < size_t diagIx >
		struct ColumnElimination< diagIx, 0 >
		{
			static void EliminateColumn( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
			}
		};

		template < size_t iter >
		static void BackSubstitute( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
			constexpr const size_t diagIx = iter - 1;
			
			const T diagInv = 1 / ELEMENT( A, diagIx * n + diagIx );
			BackSubstitution< diagIx, diagIx >::BackSubstituteRow( diagInv, A, InvertedA );

			MatrixMultiplyC< T, 1, n >( InvertedA + diagIx * n, diagInv, InvertedA + diagIx * n );

			BackSubstitute< iter - 1 >( A, InvertedA );
		}

		template <>
		static void BackSubstitute< 0 >( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
		}

		template < size_t diagIx, size_t iter >
		struct BackSubstitution
		{
			static void BackSubstituteRow( __in const T diagInverse, __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
				constexpr const size_t rowIx = iter - 1;

				const T rowFactor = ELEMENT( A, rowIx * n + diagIx ) * diagInverse;
				MatrixSubC< T, 1, n >( InvertedA + rowIx * n, InvertedA + diagIx * n, rowFactor, InvertedA + rowIx * n );

				BackSubstitution< diagIx, iter - 1 >::BackSubstituteRow( diagInverse, A, InvertedA );
			}
		};

		template < size_t diagIx >
		struct BackSubstitution< diagIx, 0 >
		{
			static void BackSubstituteRow( __in const T diagInverse, __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
			}
		};
	};

	template < typename T, size_t n >
	void MatrixInvertGJ( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
	{
		MatrixIdentity< T, n >( InvertedA );

		GaussJordanEliminationCore< T, n >::GetRowEschelonForm< n >( A, InvertedA );
		GaussJordanEliminationCore< T, n >::BackSubstitute< n >( A, InvertedA );
	}

}
