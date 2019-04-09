#pragma once

namespace LinearAlgebra {

	template < typename Scalar, uint rows, uint cols >
	inline Vector< Scalar, rows > operator* (
		__in const Matrix<Scalar, rows, cols >& matrix,
		__in const Vector<Scalar, cols >& vector )
	{
		Vector< Scalar, rows > res;
		MatrixMultiply< Scalar, rows, cols, 1 >( matrix.Elements( ), vector.Elements( ), res.Elements( ) );
		return res;
	};

	template < typename Scalar, uint dimension >
	inline Vector< Scalar, dimension > operator+ (
		__in const Vector<Scalar, dimension>& vLeft,
		__in const Vector<Scalar, dimension>& vRight )
	{
		Vector<Scalar, dimension> res( vLeft );
		res += vRight;
		return res;
	};

	template < typename Scalar, uint dimension >
	inline Scalar operator* (
		__in const Vector<Scalar, dimension>& vLeft,
		__in const Vector<Scalar, dimension>& vRight )
	{
		return VectorDot< Scalar, dimension >( vLeft.Elements( ), vRight.Elements( ) );
	};

	template < typename Scalar >
	inline Vector<Scalar, 3> CrossProduct( __in const Vector<Scalar, 3>& vectL, __in const Vector<Scalar, 3>& vectR ) {
		Vector< Scalar, 3 > cp;
		V3Cross( vectL.Elements( ), vectR.Elements( ), cp.Elements( ) );
		return cp;
	};

#ifdef IMPORT_AMP_LINEAR_ALGEBRA

	template < typename Scalar, uint rows, uint cols >
	inline Vector< Scalar, rows > operator* (
		__in const Matrix<Scalar, rows, cols >& matrix,
		__in const Vector<Scalar, cols >& vector ) restrict( amp )
	{
		Vector< Scalar, rows > res;
		MatrixMultiply< Scalar, rows, cols, 1 >( matrix.Elements( ), vector.Elements( ), res.Elements( ) );
		return res;
	};

	template < typename Scalar, uint dimension >
	inline Vector< Scalar, dimension > operator+ (
		__in const Vector<Scalar, dimension>& vLeft,
		__in const Vector<Scalar, dimension>& vRight ) restrict( amp )
	{
		Vector<Scalar, dimension> res( vLeft );
		res += vRight;
		return res;
	};

	template < typename Scalar, uint dimension >
	inline Scalar operator* (
		__in const Vector<Scalar, dimension>& vLeft,
		__in const Vector<Scalar, dimension>& vRight ) restrict( amp )
	{
		return VectorDot< Scalar, dimension >( vLeft.Elements( ), vRight.Elements( ) );
	};

	template < typename Scalar >
	inline Vector<Scalar, 3> CrossProduct( __in const Vector<Scalar, 3>& vectL, __in const Vector<Scalar, 3>& vectR ) restrict( amp )
	{
		Vector< Scalar, 3 > cp;
		V3Cross( vectL.Elements( ), vectR.Elements( ), cp.Elements( ) );
		return cp;
	};

#endif

}
