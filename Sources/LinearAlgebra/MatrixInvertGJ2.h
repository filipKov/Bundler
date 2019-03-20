#pragma once

namespace LinearAlgebra {

	template < typename T, size_t n >
	struct PivotEngine
	{
		static size_t FindPivot( __in_ecount ( n * n ) const T* A, __in const size_t diagIx )
		{
			size_t maxRow = diagIx;
			for ( size_t rowIx = n - 1; rowIx > diagIx; rowIx-- )
			{
				if ( abs( ELEMENT( A, rowIx * n + diagIx ) ) > abs( ELEMENT( A, maxRow * n + diagIx ) ) )
				{
					maxRow = rowIx;
				}
			}

			return maxRow;
		}

		static void SwapRows( __inout_ecount( n * n ) T* A, __in const size_t row1, __in const size_t row2 )
		{
			T temp[n];
			ShallowCopy< T >( A + row1 * n, n, temp );
			ShallowCopy< T >( A + row2 * n, n, A + row1 * n );
			ShallowCopy< T >( temp, n, A + row2 * n );
		}
	};

	template < typename T, size_t n >
	void EliminateColumn( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA, __in const size_t diagIx )
	{
		T diagInv = 1 / ELEMENT( A, diagIx * n + diagIx );
		for ( size_t rowIx = diagIx + 1; rowIx < n; rowIx++ )
		{
			T rowFactor = ELEMENT( A, rowIx * n + diagIx ) * diagInv;

			//MatrixSubC< T, 1, n - diagIx >( A + rowIx * n + diagIx, A + diagIx * n + diagIx, rowFactor, A + rowIx * n + diagIx );

			for ( size_t colIx = diagIx; colIx < n; colIx++ )
			{
				ELEMENT( A, rowIx * n + colIx ) -= ELEMENT( A, diagIx * n + colIx ) * rowFactor;
			}

			MatrixSubC< T, 1, n >( InvertedA + rowIx * n, InvertedA + diagIx * n, rowFactor, InvertedA + rowIx * n );
		}
	}

	template < typename T, size_t n >
	void MatrixInvertGJ( __inout_ecount( n * n ) T* A, __out_ecount( n * n ) T* InvertedA )
	{
		MatrixIdentity< T, n >( InvertedA );

		// Row-echelon form
		for ( size_t rowI = 0; rowI < n - 1; rowI++ )
		{
			size_t pivotRowI = PivotEngine< T, n >::FindPivot( A, rowI );
			PivotEngine< T, n >::SwapRows( A, pivotRowI, rowI );
			PivotEngine< T, n >::SwapRows( InvertedA, pivotRowI, rowI );
			EliminateColumn< T, n >( A, InvertedA, rowI );
		}

		// Diagonalize( back-substitution )
		for ( int64 diagI = ( int64 )n - 1; diagI >= 0; diagI-- )
		{
			T diagInv = 1 / ELEMENT( A, diagI * n + diagI );

			for ( int64 rowI = 0; rowI < diagI; rowI++ )
			{
				T rowFactor = ELEMENT( A, rowI * n + diagI ) * diagInv;

				MatrixSubC< T, 1, n >( InvertedA + rowI * n, InvertedA + diagI * n, rowFactor, InvertedA + rowI * n );
			}

			MatrixMultiplyC< T, 1, n >( InvertedA + diagI * n, diagInv, InvertedA + diagI * n );
		}
	}

}
