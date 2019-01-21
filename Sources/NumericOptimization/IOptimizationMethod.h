#pragma once

#include "stdafx.h"
#include "ILeastSquaresCostFunction.h"

namespace NumericOptimization {

	template < typename Scalar >
	struct OptimizationStatistics {

		uint iterations;
		Scalar residualSize;
		Scalar delta;

	};

	template < typename Scalar >
	struct OptimizationMethodSettings {

		uint maxIterations;
		Scalar tolerance;
		
	};

	template < typename Scalar, class Residual >
	class IOptimizationMethod {

	public:

		virtual void Solve(
			__in std::shared_ptr< ILeastSquaresCostFunction< Scalar, Residual > > spCostFunction,
			__inout LinearAlgebra::Vector<Scalar>& parameterVector,
			__out_opt OptimizationStatistics<Scalar>* pResult ) const = 0;

	protected:

		inline void WriteResult(
			__in const uint iterations,
			__in const Scalar residualSize,
			__in const Scalar delta,
			__out_opt OptimizationStatistics<Scalar>* pResult ) const
		{
			if ( pResult != nullptr ) {
				pResult->delta = delta;
				pResult->iterations = iterations;
				pResult->residualSize = residualSize;
			}
		}

	};

}