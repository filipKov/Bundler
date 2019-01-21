#pragma once

#include "stdafx.h"
#include "StaticMatrix.h"

namespace LinearAlgebra {

	// -------------------------------------------------------------------------------------------------------
	//    Unary Operators
	// -------------------------------------------------------------------------------------------------------
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator- ( __in const Matrix<Scalar, rows, columns>& X ) {
		Matrix<Scalar, rows, columns> Y( X );
		Y *= Scalar( -1 );
		return Y;
	};

	// -------------------------------------------------------------------------------------------------------
	//    Scalar Binary Operators
	// -------------------------------------------------------------------------------------------------------

	// Binary multiplication: Z = xY
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator* ( __in const Scalar& x, __in const Matrix<Scalar, rows, columns>& Y ) {
		Matrix<Scalar, rows, columns> Z( Y );
		Z *= x;
		return Z;
	};

	// Binary multiplication: Z = Xy
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator* ( __in const Matrix<Scalar, rows, columns>& X, __in const Scalar& y ) {
		Matrix<Scalar, rows, columns> Z( X );
		Z *= y;
		return Z;
	};


	// -------------------------------------------------------------------------------------------------------
	//    Matrix Binary Operators
	// -------------------------------------------------------------------------------------------------------

	// Binary addition: Z = X+Y
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator+ ( __in const Matrix<Scalar, rows, columns>& X, __in const Matrix<Scalar, rows, columns>& Y ) {
		Matrix<Scalar, rows, columns> Z( X );
		Z += Y;
		return Z;
	};

	// Binary subtraction: Z = X-Y
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator- ( __in const Matrix<Scalar, rows, columns>& X, __in const Matrix<Scalar, rows, columns>& Y ) {
		Matrix<Scalar, rows, columns> Z( X );
		Z -= Y;
		return Z;
	};

	template < typename Scalar, uint rowsLeft, uint columnsLeft, uint columnsRight >
	inline Matrix< Scalar, rowsLeft, columnsRight > operator* (
		__in const Matrix< Scalar, rowsLeft, columnsLeft >& left,
		__in const Matrix< Scalar, columnsLeft, columnsRight >& right )
	{
		Matrix< Scalar, rowsLeft, columnsRight > result;

		for ( uint i = 0; i < rowsLeft; i++ ) {
			for ( uint j = 0; j < columnsRight; j++ ) {
				Scalar* pTile = &result[i][j];
				for ( uint k = 0; k < columnsLeft; k++ ) {
					*pTile += ( left[i][k] * right[k][j] );
				}
			}
		}

		return result;
	};

}