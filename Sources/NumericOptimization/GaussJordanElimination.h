#pragma once

#include "stdafx.h"
#include "ILinearSolver.h"

namespace NumericOptimization { namespace LinearSolver {

	template < typename Scalar >
	class GaussJordanElimination : public ILinearSolver< Scalar > {

	public :

		virtual Scalar Solve(
			__in const Matrix<Scalar>& A,
			__in const Vector<Scalar>& b,
			__inout Vector<Scalar>& x ) const override
		{
			CheckInput( A, b, x );
			Matrix<Scalar> ATemp = A;
			Vector<Scalar> bTemp = b;

			MakeUpperTriangular( ATemp, bTemp );
			BackSubstitute( ATemp, bTemp, x );

			return Scalar( 0 );
		};


	protected:

		void MakeUpperTriangular( __inout Matrix<Scalar>& A, __inout Vector<Scalar>& b ) const {
			uint totalRows = b.GetDimension();

			for ( uint rowCol = 0; rowCol < totalRows; rowCol++ ) {
				Pivot( A, b, rowCol );
				
				if ( abs( A[rowCol][rowCol] ) < singularityEps ) {
					throw InvalidArgumentException( GET_VARIABLE_NAME( A ), ExceptionReasons::MATRIX_SINGULAR );
				}

				EliminateColumn( A, b, rowCol );
			}
		};

		void Pivot( __inout Matrix<Scalar>& A, __inout Vector<Scalar>& b, __in const uint currentRC ) const {
			uint maxRow = currentRC;
		
			for ( uint rowCol = currentRC + 1; rowCol < b.GetDimension(); rowCol++ ) {
				if ( abs( A[rowCol][currentRC] ) > abs( A[maxRow][currentRC] ) ) {
					maxRow = rowCol;
				}
			}

			SwapRows( A, b, currentRC, maxRow );
		};

		inline void SwapRows( __inout Matrix<Scalar>& A, __inout Vector<Scalar>& b, __in const uint row1, __in const uint row2 ) const {
			Scalar bTmp = b[row1];
			b[row1] = b[row2];
			b[row2] = bTmp;

			uint count = 0, dummy = 0;
			A.GetDimensions( dummy, count );

			Scalar* pATmp = MemoryAllocator<Scalar>::Allocate( count );
			ShallowCopy( A[row1], count, pATmp );
			ShallowCopy( A[row2], count, A[row1] );
			ShallowCopy( pATmp, count, A[row2] );
			MemoryAllocator<Scalar>::Release( pATmp );
		};

		void EliminateColumn( __inout Matrix<Scalar>& A, __inout Vector<Scalar>& b, __in const uint currentRC ) const {
			uint totalRows = 0, totalColumns = 0;
			A.GetDimensions( totalRows, totalColumns );

			for ( uint row = currentRC + 1; row < totalRows; row++ ) {
				Scalar factor = A[row][currentRC] / A[currentRC][currentRC];
				b[row] -= b[currentRC] * factor;
				for ( uint col = currentRC; col < totalColumns; col++ ) {
					A[row][col] -= A[currentRC][col] * factor;
				}
			}
		};

		void BackSubstitute(
			__in const Matrix<Scalar>& A,
			__in const Vector<Scalar>& b,
			__inout Vector<Scalar>& x ) const 
		{
			int dimension = static_cast<int>( b.GetDimension() );

			for ( int row = dimension - 1; row >= 0; row-- ) {
				Scalar tmpSum = Scalar(0);

				for ( int col = row + 1; col < dimension; col++ ) {
					tmpSum += A[row][col] * x[col];
				}
				x[row] = ( b[row] - tmpSum ) / A[row][row];
			}
		};

	protected:

		const Scalar singularityEps = Scalar( 1e-12 );

	};

} }