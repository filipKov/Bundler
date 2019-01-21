#pragma once

#include "stdafx.h"

namespace NumericOptimization {

	template < typename Scalar, class Residual >
	class ILeastSquaresCostFunction {

	public:

		virtual inline void AddResidual( __in const Residual& residual ) = 0;

		virtual inline void Reset() = 0;

		virtual inline uint GetResidualCount() const = 0;

		virtual inline const Residual& GetResidual( __in const uint index ) const = 0;

		virtual void operator() (
			__in const LinearAlgebra::Vector<Scalar>& parameters,
			__inout LinearAlgebra::Vector<Scalar>& residualVector,
			__inout Matrix<Scalar>& jacobian ) const = 0;

		virtual Scalar operator() ( __in const LinearAlgebra::Vector<Scalar>& parameters ) const = 0;

	};

}