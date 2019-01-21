#pragma once

#include "stdafx.h"
#include "DiffNum.h"

namespace NumericOptimization {

	using namespace AutomaticDifferentiation;

	// -------------------------------------------------------------------------------------------------------
	//	Interface for residual		
	// -------------------------------------------------------------------------------------------------------
	template < typename Scalar, uint parameterCount >
	class IResidual {

	public:

		virtual ~IResidual() {};

		virtual inline DiffNum<Scalar, parameterCount > Evaluate ( __in const LinearAlgebra::Vector<Scalar>& parameters ) const = 0;

	};

}