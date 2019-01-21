#pragma once

#include "stdafx.h"

namespace NumericOptimization { namespace AutomaticDifferentiation {

		template <class Numeric> class DiffNumber {
		public:

			DiffNumber() : real( static_cast<Numeric>( 0.0 ) ) {};
			DiffNumber( Numeric realPart, size_t derivativesCount, Numeric *pSeeds ) : real( realPart ) {
				derivatives.Set( derivativesCount, pSeeds );
			}

			/*
			Addition: O = A + B
			O_r = r + B_r
			O_d[i] = d[i] + B_d[i]
			*/
			DiffNumber operator+ ( const DiffNumber &B ) const {
				DiffNumber out( real + B.real, derivatives.Length() );

				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = derivatives[i] + B.derivatives[i];
				}

				return out;
			};

			/*
			Subtraction: O = A - B
			O_r = r - B_r
			O_d[i] = d[i] - B_d[i]
			*/
			DiffNumber operator- ( const DiffNumber &B ) const {
				DiffNumber out( real - B.real, derivatives.Length() );

				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = derivatives[i] - B.derivatives[i];
				}

				return out;
			};

			/*
			Multiplication: O = A * B
			O_r = r * B_r
			O_d[i] = (r * B_d[i]) + (B_r * d[i])
			*/
			DiffNumber operator* ( const DiffNumber &B ) const {
				DiffNumber out( real * B.real, derivatives.Length() );

				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = ( real * B.derivatives[i] ) + ( B.real * derivatives[i] );
				}

				return out;
			};

			/*
			Division: O = A / B
			O_r = r / B_r
			O_d[i] = ( (B_r * d[i]) - ( r * B_d[i] ) ) / (B_r * B_r)
			*/
			DiffNumber operator/ ( const DiffNumber &B ) const {
				DiffNumber out( real / B.real, derivatives.Length() );

				Numeric bRealSqr = B.real * B.real;
				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = ( ( B.real * derivatives[i] ) - ( real * B.derivatives[i] ) ) / bRealSqr;
				}

				return out;
			};

			/*
			Sine function: O = Sin(A)
			O_r = sin(r)
			O_d[i] = cos(r) * d[i]
			*/
			DiffNumber sin() const {
				DiffNumber out( sin( real ), derivatives.Length() );

				Numeric cosReal = cos( real );
				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = cosReal * derivatives[i];
				}

				return out;
			}

			/*
			Cosine function: O = Cos(A)
			O_r = cos(r)
			O_d[i] = -sin(r) * d[i]
			*/
			DiffNumber cos() const {
				DiffNumber out( cos( real ), derivatives.Length() );

				Numeric negSinReal = -sin( real );
				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = negSinReal * derivatives[i];
				}

				return out;
			}

			/*
			Exponential function: O = e^A
			O_r = e ^ r
			O_d[i] = O_r * d[i]
			*/
			DiffNumber exp() const {
				Numeric ePowX = exp( real );
				DiffNumber out( ePowX, derivatives.Length() );

				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = ePowX * derivatives[i];
				}

				return out;
			}

			/*
			Logarithm function: O = log(A)
			O_r = log(r)
			O_d[i] = d[i] / r
			*/
			DiffNumber log() const {
				DiffNumber out( log( real ), derivatives.Length() );

				Numeric realInv = 1 / real;
				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = realInv * derivatives[i];
				}

				return out;
			}

			/*
			Power function: O = A^k
			O_r = r ^ k
			O_d[i] = k * r^(k-1) * d[i]
			*/
			DiffNumber pow( const Numeric k ) const {
				Numeric powK1 = pow( real, k - 1 );
				DiffNumber out( powK1 * k, derivatives.Length() );

				for ( size_t i = 0; i < derivatives.Length(); i++ ) {
					out.derivatives[i] = derivatives[i] * k * powK1;
				}

				return out;
			}

		protected:

			Numeric real;
			Buffer<Numeric> derivatives;

		protected:

			DiffNumber( Numeric realPart, size_t derivativesCount ) : real( realPart ) {
				derivatives.Allocate( derivativesCount );
			}

			inline void SetRealPart( Numeric x ) {
				real = x;
			}

			inline void SetDifferentiations( size_t count, Numeric *pDiffs ) {
				derivatives.Set( count, pDiffs );
			}

		};

	}
}
