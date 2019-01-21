#pragma once

#include "stdafx.h"

namespace NumericOptimization { namespace LinearSolver {

	template < typename Scalar >
	class ILinearSolver {

	public:

		virtual ~ILinearSolver() {};

		virtual Scalar Solve( 
			__in const Matrix<Scalar>& A, 
			__in const LinearAlgebra::Vector<Scalar>& b,
			__inout LinearAlgebra::Vector<Scalar>& x ) const = 0;

	protected:

		void CheckInput ( 
			__in const Matrix<Scalar>& A,
			__in const LinearAlgebra::Vector<Scalar>& b,
			__in const LinearAlgebra::Vector<Scalar>& x ) const
		{
			uint ARows = 0, ACols = 0;
			A.GetDimensions( ARows, ACols );

			if ( !A.CheckMultiplicationDimensions( x ) ) {
				throw InvalidDimensionsException( GET_VARIABLE_NAME( x ), x.GetDimension(), 1, ACols, 1 );
			}

			if ( b.GetDimension() != ARows ) {
				throw InvalidDimensionsException( GET_VARIABLE_NAME( b ), b.GetDimension(), 1, ARows, 1 );
			}
		};

	};

} }