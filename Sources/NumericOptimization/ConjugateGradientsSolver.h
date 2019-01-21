#pragma once

#include "stdafx.h"
#include "ILinearSolver.h"

namespace NumericOptimization { namespace LinearSolver {

	template < typename Scalar >
	class ConjugateGradientsSolver : public ILinearSolver<Scalar> {

	public:

		ConjugateGradientsSolver( __in const uint maxIterations, __in const Scalar tolerance ) {
			m_maxIterations = maxIterations;
			m_tolerance = tolerance;
		};

		Scalar Solve ( 
			__in const Matrix<Scalar, MatrixBase::Dynamic>& A, 
			__in const LinearAlgebra::Vector<Scalar, MatrixBase::Dynamic>& b,
			__inout LinearAlgebra::Vector<Scalar, MatrixBase::Dynamic>& x ) const override
		{
			CheckInput( A, b, x );

			uint dimension = x.GetDimension();

			LinearAlgebra::Vector<Scalar, MatrixBase::Dynamic> residual = b - A * x;		// r_0 = b - Ax_0
			LinearAlgebra::Vector<Scalar, MatrixBase::Dynamic> delta = residual;			// d_0 = r_0	
			LinearAlgebra::Vector<Scalar, MatrixBase::Dynamic> Adelta ( dimension );

			for ( uint k = 0; k < m_maxIterations; k++ ) {
				Adelta = A * delta;

				Scalar rTr = residual.Dot( residual );
				if ( sqrt(rTr) <= m_tolerance ) {
					// printf( "Solver finished after %d iterations\n", k );
					break;
				}

				Scalar alpha = rTr / ( delta.Dot( Adelta ) );								// a_i = ( rTr_i, r_i )/ ( dT_i, Ad_i )

				x += delta * alpha;															// x_(i+1) = x_i + a_i * d_i
				residual -= alpha * Adelta;													// r_(i+1) = r_i - a_i * Ad_i
				
				Scalar beta = residual.Dot( residual ) / rTr;								// b_(i+1) = ( rT_(i+1), r_(i+1) ) / (rT_i, r_i)
				delta = residual + delta * beta;											// d_(i+1) = r_(i+1) + b_(i+1) * d_i
			}

			// printf( "Final residual is %f\n", residual.Length() );
			return residual.Length();
		};

	protected:

		uint m_maxIterations;
		Scalar m_tolerance;

	};

} }