#pragma once

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
		MatrixMultiply< Scalar, rowsLeft, columnsLeft, columnsRight >( left.Elements( ), right.Elements( ), result.Elements( ) );
		return result;
	};

#ifdef IMPORT_AMP_LINEAR_ALGEBRA

	// -------------------------------------------------------------------------------------------------------
	//    Unary Operators
	// -------------------------------------------------------------------------------------------------------
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator- ( __in const Matrix<Scalar, rows, columns>& X ) restrict( amp )
	{
		Matrix<Scalar, rows, columns> Y( X );
		Y *= Scalar( -1 );
		return Y;
	};

	// -------------------------------------------------------------------------------------------------------
	//    Scalar Binary Operators
	// -------------------------------------------------------------------------------------------------------

	// Binary multiplication: Z = xY
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator* ( __in const Scalar& x, __in const Matrix<Scalar, rows, columns>& Y ) restrict( amp )
	{
		Matrix<Scalar, rows, columns> Z( Y );
		Z *= x;
		return Z;
	};

	// Binary multiplication: Z = Xy
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator* ( __in const Matrix<Scalar, rows, columns>& X, __in const Scalar& y ) restrict( amp )
	{
		Matrix<Scalar, rows, columns> Z( X );
		Z *= y;
		return Z;
	};


	// -------------------------------------------------------------------------------------------------------
	//    Matrix Binary Operators
	// -------------------------------------------------------------------------------------------------------

	// Binary addition: Z = X+Y
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator+ ( __in const Matrix<Scalar, rows, columns>& X, __in const Matrix<Scalar, rows, columns>& Y ) restrict( amp )
	{
		Matrix<Scalar, rows, columns> Z( X );
		Z += Y;
		return Z;
	};

	// Binary subtraction: Z = X-Y
	template <typename Scalar, uint rows, uint columns >
	inline Matrix<Scalar, rows, columns> operator- ( __in const Matrix<Scalar, rows, columns>& X, __in const Matrix<Scalar, rows, columns>& Y ) restrict( amp )
	{
		Matrix<Scalar, rows, columns> Z( X );
		Z -= Y;
		return Z;
	};

	template < typename Scalar, uint rowsLeft, uint columnsLeft, uint columnsRight >
	inline Matrix< Scalar, rowsLeft, columnsRight > operator* (
		__in const Matrix< Scalar, rowsLeft, columnsLeft >& left,
		__in const Matrix< Scalar, columnsLeft, columnsRight >& right ) restrict( amp )
	{
		Matrix< Scalar, rowsLeft, columnsRight > result;
		MatrixMultiply< Scalar, rowsLeft, columnsLeft, columnsRight >( left.Elements( ), right.Elements( ), result.Elements( ) );
		return result;
	};

#endif

}
