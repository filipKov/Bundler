#pragma once

#include "stdafx.h"

namespace NumericOptimization { namespace AutomaticDifferentiation {

	/*
		Interface for Automatic Differentiation implemented through operator overloading method
	*/
	template <class NumericType> class IDifferentiableNumber {
	public:

		virtual ~IDifferentiableNumber() {};

		// Setters/Getters
		virtual inline NumericType GetFunctionValue() const = 0;
		virtual inline NumericType GetDerivative( __in const size_t index ) const = 0;
		virtual inline size_t GetDiffCount() const = 0;

		virtual inline void SetFunctionValue( __in const NumericType value ) = 0;
		virtual inline HRESULT SetDifferentiation( __in const size_t totalDiffs, __in const size_t diffIx ) = 0;
		virtual inline void SetDerivativeValue( __in const size_t index, __in const NumericType value ) = 0;

	};

} }