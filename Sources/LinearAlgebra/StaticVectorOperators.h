#pragma once

#include "stdafx.h"
#include "StaticVector.h"

namespace LinearAlgebra {

	template < typename Scalar, uint rows, uint cols >
	inline Vector< Scalar, rows > operator* (
		__in const Matrix<Scalar, rows, cols >& matrix,
		__in const Vector<Scalar, cols >& vector )
	{
		Vector< Scalar, rows > res;

		for ( uint r = 0; r < rows; r++ ) {
			Scalar tmp = Scalar( 0 );
			for ( uint c = 0; c < cols; c++ ) {
				tmp += matrix[r][c] * vector[c];
			}
			res[r] = tmp;
		}

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
		Scalar dot = Scalar( 0 );
		for ( uint i = 0; i < dimension; i++ )
		{
			dot += vLeft[i] * vRight[i];
		}

		return dot;
	};

	template < typename Scalar >
	inline Vector<Scalar, 3> CrossProduct( __in const Vector<Scalar, 3>& vectL, __in const Vector<Scalar, 3>& vectR ) {
		return Vector<Scalar, 3>( {
			( vectL[1] * vectR[2] ) - ( vectL[2] * vectR[1] ),
			( vectL[2] * vectR[0] ) - ( vectL[0] * vectR[2] ),
			( vectL[0] * vectR[1] ) - ( vectL[1] * vectR[0] )
		} );
	};

}