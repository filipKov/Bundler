#pragma once

#include "stdafx.h"

namespace NumericOptimization { namespace AutomaticDifferentiation {

	// Include global/std math functions for use with scalar values
	INCLUDE_STD_MATH_FUNCTIONS; 

	// -------------------------------------------------------------------------------------------------------
	//	CPU implementation of Forward Automatic Differentiation (revised)
	//			
	//		Implementation Note No.1: Implemetation with 1st order differentiation in mind!
	//
	//		ORDER OF WORK:
	//			6) Math functions:
	//				- pow ( TODO: scalar^diff, diff^diff )
	//				- other... (asin, acos, atan, atan2, sinh, cosh, tanh)
	//
	// -------------------------------------------------------------------------------------------------------
	template< typename BaseType, uint N > class DiffNum  {

	public:
		
		// -------------------------------------------------------------------------------------------------------
		//    Constructors/Initializers/Destructors
		// -------------------------------------------------------------------------------------------------------
		DiffNum() : m_real( 0 ) {};

		explicit DiffNum( __in const BaseType& value ) : m_real( value ) {};

		DiffNum( __in const BaseType &value, __in const uint diffIx ) : m_real(value) {
			if ( diffIx >= N ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( diffIx ), &diffIx );
			}

			m_infinitesimal[diffIx] = BaseType( 1 );
		};

		DiffNum( __in const BaseType& value, __in const Matrix< BaseType, N, 1 >& diffs ) :
			m_real( value ), m_infinitesimal( diffs ) 
		{};

		DiffNum( __in const DiffNum<BaseType, N>& src ) {
			m_real = src.m_real;
			m_infinitesimal = src.m_infinitesimal;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Accessors
		// -------------------------------------------------------------------------------------------------------
		inline BaseType& GetFx() {
			return m_real;
		};

		inline const BaseType& GetFx() const {
			return m_real;
		};

		inline void SetFx( __in const BaseType& value ) {
			m_real = value;
		};

		inline Vector< BaseType, N >& GetDiff() {
			return m_infinitesimal;
		};
		
		inline const Vector< BaseType, N >& GetDiff() const {
			return m_infinitesimal;
		};

		inline BaseType& GetDiff( __in const uint index ) {
			if ( index >= N ) {
				throw IndexOutOfRangeException( index, N - 1 );
			}

			return m_infinitesimal[index];
		};

		inline const BaseType& GetDiff( __in const uint index ) const {
			if ( index >= N ) {
				throw IndexOutOfRangeException( index, N - 1 );
			}

			return m_infinitesimal[index];
		};

		inline void SetDiff( __in const uint index ) {
			if ( index >= N ) {
				throw IndexOutOfRangeException( index, N - 1 );
			}

			m_infinitesimal[index] = BaseType( 1 );
		};

		inline void SetDiffExplicit( __in const uint index, __in BaseType& value ) {
			if ( index >= N ) {
				throw IndexOutOfRangeException( index, N - 1 );
			}

			m_infinitesimal[index] = value;
		};

		constexpr inline uint GetDiffCount() const {
			return N;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Scalar Comparators
		// -------------------------------------------------------------------------------------------------------
		
		inline bool operator==( __in const BaseType& scalar ) const {
			return m_real == scalar;
		}

		inline bool operator!=( __in const BaseType& scalar ) const {
			return m_real != scalar;
		}

		inline bool operator<( __in const BaseType& scalar ) const {
			return m_real < scalar;
		}

		inline bool operator>( __in const BaseType& scalar ) const {
			return m_real > scalar;
		}

		inline bool operator<=( __in const BaseType& scalar ) const {
			return m_real <= scalar;
		}

		inline bool operator>=( __in const BaseType& scalar ) const {
			return m_real >= scalar;
		}
		


		// -------------------------------------------------------------------------------------------------------
		//    Basic operators
		// -------------------------------------------------------------------------------------------------------
		inline DiffNum<BaseType, N>& operator= ( __in const DiffNum<BaseType, N>& other ) {
			m_real = other.m_real;
			m_infinitesimal = other.m_infinitesimal;
			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Scalar operators
		// -------------------------------------------------------------------------------------------------------
		inline DiffNum<BaseType, N>& operator+= ( __in const BaseType& scalar ) {
			m_real += scalar;
			return *this;
		};

		inline DiffNum<BaseType, N>& operator-= ( __in const BaseType& scalar ) {
			m_real -= scalar;
			return *this;
		};

		inline DiffNum<BaseType, N>& operator*= ( __in const BaseType& scalar ) {
			m_real *= scalar;
			m_infinitesimal *= scalar;
			return *this;
		};

		inline DiffNum<BaseType, N>& operator/= ( __in const BaseType& scalar ) {
			if ( scalar == BaseType( 0 ) ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( scalar ), ExceptionReasons::DIVISION_BY_ZERO );
			}

			BaseType scalarInverse = BaseType( 1 ) / scalar;
			m_real *= scalarInverse;
			m_infinitesimal *= scalarInverse;
			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    DiffNum operators
		// -------------------------------------------------------------------------------------------------------
		inline DiffNum<BaseType, N>& operator+= ( __in const DiffNum<BaseType, N>& other ) {
			m_real += other.m_real;
			m_infinitesimal += other.m_infinitesimal;
			return *this;
		};

		inline DiffNum<BaseType, N>& operator-= ( __in const DiffNum<BaseType, N>& other ) {
			m_real -= other.m_real;
			m_infinitesimal -= other.m_infinitesimal;
			return *this;
		};

		inline DiffNum<BaseType, N>& operator*= ( __in const DiffNum<BaseType, N>& other ) {
			m_infinitesimal = ( m_infinitesimal * other.m_real ) + ( m_real * other.m_infinitesimal );
			m_real *= other.m_real;
			return *this;
		};

		inline DiffNum<BaseType, N>& operator/= ( __in const DiffNum<BaseType, N>& other ) {
			if ( other.m_real == BaseType( 0 ) ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( other.m_real ), ExceptionReasons::DIVISION_BY_ZERO );
			}

			const BaseType oRealSquaredInverse = BaseType( 1 ) / (other.m_real * other.m_real);
			m_infinitesimal = ( ( m_infinitesimal * other.m_real ) - ( m_real * other.m_infinitesimal ) ) * oRealSquaredInverse;
			m_real /= other.m_real;
			return *this;
		};

	protected:
		
		BaseType m_real;
		Vector< BaseType, N > m_infinitesimal;
		
	};

	// -------------------------------------------------------------------------------------------------------
	//    Unary operators
	// -------------------------------------------------------------------------------------------------------
	// Unary -
	template< typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator-( __in const DiffNum<BaseType, N>& x ) {
		return DiffNum<BaseType, N>( -x.GetFx(), -x.GetDiff() );
	};

	// -------------------------------------------------------------------------------------------------------
	//    Binary operators
	//
	//		Scalar (scalar = s, DiffNum = x )
	// -------------------------------------------------------------------------------------------------------
	
	// Scalar x + s
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator+ ( __in const DiffNum<BaseType, N>& x, __in const BaseType& s ) {
		return DiffNum<BaseType, N>( x.GetFx() + s, x.GetDiff() );
	};

	// Scalar s + x
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator+ ( __in const BaseType& s, __in const DiffNum<BaseType, N>& x ) {
		return ( x + s );
	};

	// Scalar x - s
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator- ( __in const DiffNum<BaseType, N>& x, __in const BaseType& s ) {
		return DiffNum<BaseType, N>( x.GetFx() - s, x.GetDiff() );
	};

	// Scalar s - x
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator- ( __in const BaseType& s, __in const DiffNum<BaseType, N>& x ) {
		return DiffNum<BaseType, N>( s - x.GetFx(), -x.GetDiff() );
	};

	// Scalar x * s
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator* ( __in const DiffNum<BaseType, N>& x, __in const BaseType& s ) {
		return DiffNum<BaseType, N>( x.GetFx() * s, x.GetDiff() * s );
	};

	// Scalar s * x
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator* ( __in const BaseType& s, __in const DiffNum<BaseType, N>& x ) {
		return ( x * s );
	};

	// Scalar x / s
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator/ ( __in const DiffNum<BaseType, N>& x, __in const BaseType& s ) {
		if ( s == BaseType( 0 ) ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( s ), ExceptionReasons::DIVISION_BY_ZERO );
		}

		const BaseType scalarInverse = BaseType(1) / s;
		return DiffNum<BaseType, N>( x.GetFx() * scalarInverse, x.GetDiff() * scalarInverse );
	};

	// Scalar s / x
	// 
	//    x          x(a-be)         xa - xbe      x        xbe
	// ------- = --------------- = ------------ = ---  -  ------- 
	//	a + be	  (a+be)(a-be)         a^2         a        a^2
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator/ ( __in const BaseType& s, __in const DiffNum<BaseType, N>& x ) {
		if ( x.GetFx() == BaseType( 0 ) ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( x.fx ), ExceptionReasons::DIVISION_BY_ZERO );
		}

		const BaseType coef = -s / ( x.GetFx() * x.GetFx() );
		return DiffNum<BaseType, N>( s / x.GetFx(), x.GetDiff() * coef );
	};

	// -------------------------------------------------------------------------------------------------------
	//		DiffNum
	// -------------------------------------------------------------------------------------------------------
	
	// Diffnum x + y
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator+ ( __in const DiffNum<BaseType, N>& x, __in const DiffNum<BaseType, N>& y ) {
		return DiffNum<BaseType, N>( x.GetFx() + y.GetFx(), x.GetDiff() + y.GetDiff() );
	};

	// Diffnum x - y
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator- ( __in const DiffNum<BaseType, N>& x, __in const DiffNum<BaseType, N>& y ) {
		return DiffNum<BaseType, N>( x.GetFx() - y.GetFx(), x.GetDiff() - y.GetDiff() );
	};

	// Diffnum x * y
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator* ( __in const DiffNum<BaseType, N>& x, __in const DiffNum<BaseType, N>& y ) {
		return DiffNum<BaseType, N>( x.GetFx() * y.GetFx(), ( x.GetDiff() * y.GetFx() ) + ( x.GetFx() * y.GetDiff() ) );
	};

	// Diffnum x / y
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> operator/ ( __in const DiffNum<BaseType, N>& x, __in const DiffNum<BaseType, N>& y ) {
		if ( y.GetFx() == BaseType( 0 ) ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( y ), ExceptionReasons::DIVISION_BY_ZERO );
		}
 
		const BaseType yRealInvSquared = BaseType( 1 ) / ( y.GetFx() * y.GetFx() );
		return DiffNum<BaseType, N>( x.GetFx() / y.GetFx(), ( ( x.GetDiff() * y.GetFx() ) - ( x.GetFx() * y.GetDiff() ) ) * yRealInvSquared );
	};


	// -------------------------------------------------------------------------------------------------------
	//	Functions
	// -------------------------------------------------------------------------------------------------------
	
	// abs based on f(x)
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> abs( __in const DiffNum<BaseType, N>& x ) {
		return x.GetFx() < BaseType( 0 ) ? -x : x;
	};

	// -------------------------------------------------------------------------------------------------------
	//		Exponential and Logarithmic functions
	// -------------------------------------------------------------------------------------------------------
	
	// natural logarithm (base e)
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> log( __in const DiffNum<BaseType, N>& x ) {
		if ( x.GetFx() <= BaseType( 0 ) ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( x ), ExceptionReasons::BAD_LOGARITHM_INPUT );
		}

		return DiffNum<BaseType, N>( log( x.GetFx() ), x.GetDiff() * ( 1 / x.GetFx() ) );
	};

	// base 10 logarithm
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> log10( __in const DiffNum<BaseType, N>& x ) {
		if ( x.GetFx() <= BaseType( 0 ) ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( x ), ExceptionReasons::BAD_LOGARITHM_INPUT );
		}

		const BaseType diffLog10 = BaseType( 1 ) / ( x.GetFx() * log( BaseType( 10 ) ) );
		return DiffNum<BaseType, N>( log10( x.GetFx() ), diffLog10 * x.GetDiff() );
	};

	// binary logarithm (base 2)
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> log2( __in const DiffNum<BaseType, N>& x ) {
		if ( x.GetFx() <= BaseType( 0 ) ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( x ), ExceptionReasons::BAD_LOGARITHM_INPUT );
		}

		const BaseType diffLog2 = BaseType( 1 ) / ( x.GetFx() * log( BaseType( 2 ) ) );
		return DiffNum<BaseType, N>( log2( x.GetFx() ), diffLog2 * x.GetDiff() );
	};

	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> exp( __in const DiffNum<BaseType, N>& x ) {
		const BaseType ePowX = exp( x.GetFx() );
		return DiffNum<BaseType, N>( ePowX, x.GetDiff() * ePowX );
	};

	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> sqrt( __in const DiffNum<BaseType, N>& x ) {
		if ( x.GetFx() <= 0 ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( x ), ExceptionReasons::BAD_SQRT_INPUT );
		}

		const BaseType sqRoot = sqrt( x.GetFx() );
		const BaseType halfSqRootInverse = BaseType( 1 ) / ( BaseType( 2 ) * sqRoot );
		return DiffNum<BaseType, N>( sqRoot, x.GetDiff() * halfSqRootInverse );
	};

	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> cbrt( __in const DiffNum<BaseType, N>& x ) {
		if ( x.GetFx() == 0 ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( x ), ExceptionReasons::BAD_CBRT_INPUT );
		}

		const BaseType y = pow( x.GetFx(), BaseType( BaseType(2) / BaseType(3) ) );
		const BaseType cbRootDiffCoef = BaseType( 1 ) / ( BaseType(3) * y );
		return DiffNum<BaseType, N>( cbrt( x.GetFx() ), x.GetDiff() * cbRootDiffCoef );
	};

	// pow x^s
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> pow( __in const DiffNum<BaseType, N>& x, __in const BaseType& s ) {
		const BaseType fxPowS1 = pow( x.GetFx(), s - BaseType(1) );
		return DiffNum<BaseType, N>( fxPowS1 * x.GetFx(), ( s * fxPowS1 ) * x.GetDiff() );
	};


	// -------------------------------------------------------------------------------------------------------
	//		Trigonometric functions
	// -------------------------------------------------------------------------------------------------------
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> sin( __in const DiffNum<BaseType, N>& x ) {
		return DiffNum<BaseType, N>( sin( x.GetFx() ), cos( x.GetFx() ) * x.GetDiff());
	};

	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> cos( __in const DiffNum<BaseType, N>& x ) {
		return DiffNum<BaseType, N>( cos( x.GetFx() ), -sin( x.GetFx() ) * x.GetDiff() );
	};

	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> tan( __in const DiffNum<BaseType, N>& x ) {
		const BaseType tanFx = tan( x.GetFx() );
		const BaseType onePlusTanFxSquared = BaseType( 1 ) + tanFx * tanFx;
		return DiffNum<BaseType, N>( tanFx, onePlusTanFxSquared * x.GetDiff() );
	};

	/* Copy this template when defining new functions :)
	template < typename BaseType, uint N >
	inline DiffNum<BaseType, N> **FUNCTION_NAME**( __in const DiffNum<BaseType, N>& x ) {
		return DiffNum<BaseType, N>();
	};
	*/

	/* Implemented math functions from std-imported
	
	cos; \ - Y
	sin; \ - Y
	tan; \ - Y
	acos; \ - N
	asin; \ - N
	atan; \ - N
	atan2; \ - N
	cosh; \ - N
	sinh; \ - N
	tanh; \ - N
	acosh; \ - N
	asinh; \ - N
	atanh; \ - N
	exp; \ - Y
	log; \ - Y
	log10; \ - Y
	log2; \ - Y
	pow; \ -PARTIALLY
	sqrt; \ - Y
	cbrt; \ - Y
	ceil; \ -N (possibly won't bother)
	floor; \ -N (same)
	abs; -Y

	*/

} }
