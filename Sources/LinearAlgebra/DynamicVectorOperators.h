#pragma once

#include "stdafx.h"
#include "DynamicVector.h"

namespace LinearAlgebra {

	// -------------------------------------------------------------------------------------------------------
	//    Unary Operators
	// -------------------------------------------------------------------------------------------------------
	template < typename Scalar >
	inline Vector< Scalar, MatrixBase::Dynamic > operator- ( __in const Vector< Scalar, MatrixBase::Dynamic >& u ) {
		Vector<Scalar, MatrixBase::Dynamic> v( u );
		v *= Scalar( -1 );
		return v;
	};

	// -------------------------------------------------------------------------------------------------------
	//    Scalar Binary Operators
	// -------------------------------------------------------------------------------------------------------
	template < typename Scalar >
	inline Vector< Scalar, MatrixBase::Dynamic > operator* ( __in const Vector< Scalar, MatrixBase::Dynamic >& u, __in const Scalar& cnst ) {
		Vector<Scalar, MatrixBase::Dynamic> v( u );
		v *= cnst;
		return v;
	};

	template < typename Scalar >
	inline Vector< Scalar, MatrixBase::Dynamic > operator* ( __in const Scalar& cnst, __in const Vector< Scalar, MatrixBase::Dynamic >& u ) {
		return u * cnst;
	};

	// -------------------------------------------------------------------------------------------------------
	//    Vector/Matrix Binary Operators
	// -------------------------------------------------------------------------------------------------------
	template < typename Scalar >
	inline Vector< Scalar, MatrixBase::Dynamic > operator+ (
		__in const Vector< Scalar, MatrixBase::Dynamic >& u,
		__in const Vector< Scalar, MatrixBase::Dynamic >& v )
	{
		if ( !u.HasEqualDimensions( v.GetDimension() ) ) {
			throw InvalidDimensionsException( GET_VARIABLE_NAME( B ) );
		}

		Vector< Scalar, MatrixBase::Dynamic > w( u );
		w += v;
		return w;
	};

	template < typename Scalar >
	inline Vector< Scalar, MatrixBase::Dynamic > operator- (
		__in const Vector< Scalar, MatrixBase::Dynamic >& u,
		__in const Vector< Scalar, MatrixBase::Dynamic >& v )
	{
		if ( !u.HasEqualDimensions( v.GetDimension() ) ) {
			throw InvalidDimensionsException( GET_VARIABLE_NAME( B ) );
		}

		Vector< Scalar, MatrixBase::Dynamic > w( u );
		w -= v;
		return w;
	};

	template < typename Scalar >
	inline Vector< Scalar, MatrixBase::Dynamic > operator* (
		__in const Matrix< Scalar, MatrixBase::Dynamic >& A,
		__in const Vector< Scalar, MatrixBase::Dynamic >& u )
	{
		uint ARows = 0, ACols = 0;
		A.GetDimensions( ARows, ACols );

		if ( !A.CheckMultiplicationDimensions( u ) ) {
			throw InvalidDimensionsException( u.GetDimension(), 1, ACols, 1 );
		}

		Vector< Scalar, MatrixBase::Dynamic > v( ARows );

		Scalar *pA = A.Elements();
		Scalar *pU = u.Elements();
		Scalar *pV = v.Elements();

		for ( uint r = 0; r < ARows; r++ ) {
			Scalar tmp = Scalar( 0 );
			for ( uint c = 0; c < ACols; c++ ) {
				tmp += ( *( pA + r * ACols + c ) ) * ( *( pU + c ) );
			}
			*( pV + r ) = tmp;
		}

		return v;
	};

}