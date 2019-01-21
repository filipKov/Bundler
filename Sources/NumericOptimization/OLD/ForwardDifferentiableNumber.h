#pragma once

#include "stdafx.h"
#include "DifferentiableNumber.h"

namespace NumericOptimization { namespace AutomaticDifferentiation {

	/*
		CPU Implementation of Forward Automatic Differentiation by operator overloading

		TODO List: 
			1) add Initialize method(s) with same stuff as constructors have, but HRESULT return value
				1.1) adapt all methods to this
			2) more math functions (sqrt could be useful for example)
			3) UNIT TEST! - WIP
	*/
	template <class NumericType> class ForwardDifferentiableNumber : public IDifferentiableNumber<NumericType> {
	public:

		// Constructors/Destructor/Initialization
		ForwardDifferentiableNumber(): functValue(0) {};

		ForwardDifferentiableNumber( __in const ForwardDifferentiableNumber &number ) {
			functValue = number.functValue;
			differentiations.Set( number.differentiations );
		}

		ForwardDifferentiableNumber( __in const NumericType value ) : functValue( value ) {};

		ForwardDifferentiableNumber( __in const NumericType value, __in const size_t maxDiffCount ) {
			functValue = value;
			differentiations.Allocate( maxDiffCount );
		}

		virtual ~ForwardDifferentiableNumber() {};

		// Setters/Getters
		virtual inline NumericType GetFunctionValue() const { 
			return functValue; 
		}

		virtual inline size_t GetDiffCount() const {
			return differentiations.Length();
		}

		virtual inline NumericType GetDerivative( __in const size_t index ) const {
			// for performance purpouses, bound checks will be done only in DEBUG by these asserts
			_ASSERT_EXPR( index < differentiations.Length(), L"Invalid index while retrieving partial differentiation." );
			return differentiations[index];
		}

		virtual inline void SetFunctionValue( __in const NumericType value ) { 
			functValue = value; 
		}

		virtual inline HRESULT SetDifferentiation( __in const size_t totalDiffs, __in const size_t diffIx ) {
			HRESULT hr = differentiations.Allocate( totalDiffs );
			if ( SUCCEEDED( hr ) ) {
				differentiations.Fill( 0 );
				differentiations[diffIx] = 1; 
			}

			return hr;
		}

		virtual inline void SetDerivativeValue( __in const size_t index, __in const NumericType value ) {
			_ASSERT_EXPR( index < differentiations.Length(), L"Invalid index while setting value for partial derivative" );
			differentiations[index] = value;
		}

		ForwardDifferentiableNumber& operator= ( __in const ForwardDifferentiableNumber &B ) {
			functValue = B.functValue;
			differentiations.Set( B.differentiations );
			return ( *this );
		}

		// Assignment operators
		ForwardDifferentiableNumber& operator+= ( __in const ForwardDifferentiableNumber &B ) {
			_ASSERT_EXPR( differentiations.Length() == B.GetDiffCount(), L"Total diff count differs in numbers" );
			functValue += B.functValue;

			for ( size_t i = 0; i < differentiations.Length(); i++ ) {
				differentiations[i] += B.differentiations[i];
			}
			return ( *this );
		}

		ForwardDifferentiableNumber& operator-= ( __in const ForwardDifferentiableNumber &B ) {
			_ASSERT_EXPR( differentiations.Length() == B.GetDiffCount(), L"Total diff count differs in numbers" );
			functValue -= B.functValue;

			for ( size_t i = 0; i < differentiations.Length(); i++ ) {
				differentiations[i] -= B.differentiations[i];
			}
			return ( *this );
		}

		ForwardDifferentiableNumber& operator*= ( __in const ForwardDifferentiableNumber &B ) {
			_ASSERT_EXPR( differentiations.Length() == B.GetDiffCount(), L"Total diff count differs in numbers" );
			
			for ( size_t i = 0; i < differentiations.Length(); i++ ) {
				differentiations[i] = ( functValue * B.differentiations[i] ) + ( B.functValue * differentiations[i] );
			}
			functValue *= B.functValue;

			return ( *this );
		}

		ForwardDifferentiableNumber& operator/= ( __in const ForwardDifferentiableNumber &B ) {
			_ASSERT_EXPR( differentiations.Length() == B.GetDiffCount(), L"Total diff count differs in numbers" );

			NumericType BSquared = B.functValue * B.functValue;
			for ( size_t i = 0; i < differentiations.Length(); i++ ) {
				differentiations[i] = (( B.functValue * differentiations[i] ) - ( functValue * B.differentiations[i] )) / BSquared;
			}
			functValue /= B.functValue;

			return ( *this );
		}

		ForwardDifferentiableNumber& operator+= ( __in const NumericType &c ) {
			functValue += c;
			return ( *this );
		}

		ForwardDifferentiableNumber& operator-= ( __in const NumericType &c ) {
			functValue -= c;
			return ( *this );
		}

		ForwardDifferentiableNumber& operator*= ( __in const NumericType &c ) {
			functValue *= c;
			for ( size_t i = 0; i < differentiations.Length(); i++ ) {
				differentiations[i] *= c;
			}
			return ( *this );
		}

		ForwardDifferentiableNumber& operator/= ( __in const NumericType &c ) {
			return ( ( *this ) *= ( 1 / c ) );
		}

	
	protected:

		NumericType functValue;
		Buffer<NumericType> differentiations;

	};

	// Basic arithmetic

	// -- Addition
	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator+( __in const ForwardDifferentiableNumber<NumericType> &A, __in const ForwardDifferentiableNumber<NumericType> &B ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out += B;
		return out;
	};

	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator+ ( __in const ForwardDifferentiableNumber<NumericType> &A, __in const NumericType &x ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out += x;
		return out;
	};

	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator+ ( __in const NumericType x, __in const ForwardDifferentiableNumber<NumericType> &A ) {
		return ( A + x );
	};

	// -- Subtraction
	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator-( __in const ForwardDifferentiableNumber<NumericType> &A, __in const ForwardDifferentiableNumber<NumericType> &B ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out -= B;
		return out;
	};

	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator- ( __in const ForwardDifferentiableNumber<NumericType> &A, __in const NumericType x ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out -= x;
		return out;
	};

	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator- ( __in const NumericType x, __in const ForwardDifferentiableNumber<NumericType> &A ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out *= -1;
		out += x;
		return out;
	};

	// -- Multiplication
	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator*( __in const ForwardDifferentiableNumber<NumericType> &A, __in const ForwardDifferentiableNumber<NumericType> &B ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out *= B;
		return out;
	};

	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator* ( __in const ForwardDifferentiableNumber<NumericType> &A, __in const NumericType x ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out *= x;
		return out;
	};

	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator* ( __in const NumericType x, __in const ForwardDifferentiableNumber<NumericType> &A ) {
		return ( A * x );
	};

	// -- Division
	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator/( __in const ForwardDifferentiableNumber<NumericType> &A, __in const ForwardDifferentiableNumber<NumericType> &B ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out /= B;
		return out;
	};

	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator/ ( __in const ForwardDifferentiableNumber<NumericType> &A, __in const NumericType x ) {
		ForwardDifferentiableNumber<NumericType> out( A );
		out *= (1 / x);
		return out;
	};

	template <class NumericType>
	ForwardDifferentiableNumber<NumericType> operator/ ( __in const NumericType x, __in const ForwardDifferentiableNumber<NumericType> &A ) {
		ForwardDifferentiableNumber<NumericType> out( x / A.GetFunctionValue(), A.GetDiffCount() );

		NumericType XNegAInv = - ( x / ( A.GetFunctionValue() * A.GetFunctionValue() ) );
		for ( size_t i = 0; i < A.GetDiffCount(); i++ ) {
			out.SetDerivativeValue( i, XNegAInv * A.GetDerivative( i ) );
		}
		return out;
	};

	// Trigonometry
	template <class NumericType> ForwardDifferentiableNumber<NumericType> sin( __in const ForwardDifferentiableNumber<NumericType> &x ) {
		ForwardDifferentiableNumber<NumericType> out( ::sin( x.GetFunctionValue() ), x.GetDiffCount() );

		NumericType cosVal = ::cos( x.GetFunctionValue() );
		for ( size_t i = 0; i < x.GetDiffCount(); i++ ) {
			out.SetDerivativeValue( i, x.GetDerivative( i ) * cosVal );
		}

		return out;
	}

	template <class NumericType> ForwardDifferentiableNumber<NumericType> cos( __in const ForwardDifferentiableNumber<NumericType> &x ) {
		ForwardDifferentiableNumber<NumericType> out( ::cos( x.GetFunctionValue() ), x.GetDiffCount() );

		NumericType negSin = -( ::sin( x.GetFunctionValue() ) );
		for ( size_t i = 0; i < x.GetDiffCount(); i++ ) {
			out.SetDerivativeValue( i, x.GetDerivative( i ) * negSin );
		}

		return out;
	}

	// More math functions
	template <class NumericType> ForwardDifferentiableNumber<NumericType> exp( __in const ForwardDifferentiableNumber<NumericType> &x ) {
		NumericType eToX = ::exp( x.GetFunctionValue() );
		ForwardDifferentiableNumber<NumericType> out( eToX, x.GetDiffCount() );

		for ( size_t i = 0; i < x.GetDiffCount(); i++ ) {
			out.SetDerivativeValue( i, x.GetDerivative( i ) * eToX );
		}

		return out;
	}

	template <class NumericType> ForwardDifferentiableNumber<NumericType> log( __in const ForwardDifferentiableNumber<NumericType> &x ) {
		ForwardDifferentiableNumber<NumericType> out( ::log( x.GetFunctionValue() ), x.GetDiffCount() );

		NumericType functValueInv = 1 / x.GetFunctionValue();
		for ( size_t i = 0; i < x.GetDiffCount(); i++ ) {
			out.SetDerivativeValue( i, x.GetDerivative( i ) * functValueInv );
		}

		return out;
	}

	template <class NumericType> ForwardDifferentiableNumber<NumericType> pow( __in const ForwardDifferentiableNumber<NumericType> &x, __in const NumericType k ) {
		NumericType xPowK1 = ::pow( x.GetFunctionValue(), k - 1 );
		ForwardDifferentiableNumber<NumericType> out( xPowK1 * x.GetFunctionValue(), x.GetDiffCount() );

		for ( size_t i = 0; i < x.GetDiffCount(); i++ ) {
			out.SetDerivativeValue( i, x.GetDerivative( i ) * xPowK1 * k );
		}

		return out;
	}

} }
