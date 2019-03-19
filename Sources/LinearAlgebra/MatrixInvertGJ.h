#pragma once

namespace LinearAlgebra {

	template < typename T, size_t n, size_t diagIx, size_t itCnt >
	struct PivotCore
	{
		static size_t FindPivot( __in_ecount( n * n ) const T* A )
		{
			constexpr const size_t rowIx = n - itCnt;

			const size_t pivotRowIx = PivotCore< T, n, diagIx, itCnt - 1 >::FindPivot( A );
			if ( abs( ELEMENT( A, rowIx * n + diagIx ) ) > abs( ELEMENT( A, pivotRowIx * n + diagIx ) ) )
			{
				return rowIx;
			}
			return pivotRowIx;
		}
	};

	template < typename T, size_t n, size_t diagIx >
	struct PivotCore< T, n, diagIx, 0 >
	{
		static size_t FindPivot( __in_ecount( n * n ) const T* A )
		{
			return n - 1;
		}
	};

	template < typename T, size_t n >
	struct GaussJordanEliminationCore
	{
		template < size_t i >
		static void MakeRowEschelonForm( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
			constexpr const size_t diagIx = n - i;
			const size_t pivotRowIx = PivotCore< T, n, diagIx, i >::FindPivot( A );
			
			if ( pivotRowIx != diagIx )
			{
				SwapRows( A, diagIx, pivotRowIx );
				SwapRows( InvertedA, diagIx, pivotRowIx );
			}

			EliminateColumn< diagIx >( A, InvertedA );
			// EliminateColumn( A, InvertedA, diagIx );
			MakeRowEschelonForm< i - 1 >( A, InvertedA );
		}

		template <>
		static void MakeRowEschelonForm< 0 >( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
			UNREFERENCED_PARAMETER( A );
			UNREFERENCED_PARAMETER( InvertedA );
		}
		
		static void SwapRows( __inout_ecount( n * n ) T* A, __in const size_t row1, __in const size_t row2 )
		{
			T temp[ n ];
			ShallowCopy< T >( A + row1 * n, n, temp );
			ShallowCopy< T >( A + row2 * n, n, A + row1 * n );
			ShallowCopy< T >( temp, n, A + row2 * n );
		}

		template < size_t diagIx >
		static void EliminateColumn( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
			T diagInv = 1 / ELEMENT( A, diagIx * n + diagIx );
			for ( size_t rowI = diagIx + 1; rowI < n; rowI++ )
			{
				T rowFactor = ELEMENT( A, rowI * n + diagIx ) * diagInv;
		
				MatrixSubC< T, 1, n - diagIx >( A + rowI * n + diagIx, A + diagIx * n + diagIx, rowFactor, A + rowI * n + diagIx ); 
				MatrixSubC< T, 1, n >( InvertedA + rowI * n, InvertedA + diagIx * n, rowFactor, InvertedA + rowI * n );
			}
		}

		// static void EliminateColumn( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA, __in const size_t diagIx )
		// {
		// 	T diagInv = 1 / ELEMENT( A, diagIx * n + diagIx );
		// 	for ( size_t rowI = diagIx + 1; rowI < n; rowI++ )
		// 	{
		// 		T rowFactor = ELEMENT( A, rowI * n + diagIx ) * diagInv;
		// 
		// 		for ( size_t colI = diagIx; colI < n; colI++ )
		// 		{
		// 			ELEMENT( A, rowI * n + colI ) -= ELEMENT( A, diagIx * n + colI ) * rowFactor;
		// 		}
		// 
		// 		for ( size_t colI = 0; colI < n; colI++ )
		// 		{
		// 			ELEMENT( InvertedA, rowI * n + colI ) -= ELEMENT( InvertedA, diagIx * n + colI ) * rowFactor;
		// 		}
		// 	}
		// }

		template < size_t diagIx, size_t rowIx >
		struct BackSubstitutionRow
		{
			static void Get( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
				T rowFactor = ELEMENT( A, rowIx * n + diagIx ) / ELEMENT( A, diagIx * n + diagIx );
				MatrixSubC< T, 1, n >( InvertedA + rowIx * n, InvertedA + diagIx * n, rowFactor, InvertedA + rowIx * n );
			}
		};

		template < size_t diagIx >
		struct BackSubstitutionRow< diagIx, 0 >
		{
			static void Get( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
			{
				T rowFactor = ELEMENT( A, diagIx ) / ELEMENT( A, 0 );
				MatrixSubC< T, 1, n >( InvertedA, InvertedA + diagIx * n, rowFactor, InvertedA );
			}
		};

		template < size_t diagIx >
		static void BackSubstitute( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
			BackSubstitutionRow< diagIx, diagIx >::Get( A, InvertedA );

			T diagInv = 1 / ELEMENT( A, diagIx * n + diagIx );
			MatrixMultiplyC< T, 1, n >( InvertedA + diagIx * n, diagInv, InvertedA + diagIx * n );
			BackSubstitute< diagIx - 1 >( A, InvertedA );
		}

		template <>
		static void BackSubstitute< 0 >( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
		{
			T diagInv = 1 / ELEMENT( A, 0 );
			MatrixMultiplyC< T, 1, n >( InvertedA, diagInv, InvertedA );
		}
	};

	template < typename T, size_t n >
	void MatrixInvertGJ( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
	{
		MatrixIdentity< T, n >( InvertedA );

		GaussJordanEliminationCore< T, n >::MakeRowEschelonForm< n >( A, InvertedA );
		// Row-echelon form
		// for ( size_t rowI = 0; rowI < n - 1; rowI++ )
		// {
		// 	size_t pivotRowI = PivotEngine< T, n >::FindPivot( A, rowI );
		// 	PivotEngine< T, n >::SwapRows< false >( A, pivotRowI, rowI );
		// 	PivotEngine< T, n >::SwapRows< false >( InvertedA, pivotRowI, rowI );
		// 	EliminateColumn< T, n >( A, InvertedA, rowI );
		// }

		// Diagonalize( back-substitution )
		// for ( int64 diagI = (int64)n - 1; diagI >= 0; diagI-- )
		// {
		// 	T diagInv = 1 / ELEMENT( A, diagI * n + diagI );
		// 	
		// 	for ( int64 rowI = 0; rowI < diagI; rowI++ )
		// 	{
		// 		T rowFactor = ELEMENT( A, rowI * n + diagI ) * diagInv;
		// 
		// 		MatrixSubC< T, 1, n >( InvertedA + rowI * n, InvertedA + diagI * n, rowFactor, InvertedA + rowI * n );
		// 		ELEMENT( A, rowI * n + diagI ) -= ELEMENT( A, diagI * n + diagI ) * rowFactor; // delete, we don't need to set A to Identity
		// 	}
		// 
		// 	ELEMENT( A, diagI * n + diagI ) *= diagInv; // delete, we don't need to set A to Identity
		// 	MatrixMultiplyC< T, 1, n >( InvertedA + diagI * n, diagInv, InvertedA + diagI * n );
		// }
		GaussJordanEliminationCore< T, n >::BackSubstitute< n >( A, InvertedA );
	}

}