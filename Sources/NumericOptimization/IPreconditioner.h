#pragma once

namespace NumericOptimization { namespace LinearSolver {

	template < typename Scalar >
	class IPreconditioner {
	public:

		virtual void Initialize( __in const LinearAlgebra::Matrix< Scalar >& sourceMatrix ) = 0;

		virtual void Apply( __in const LinearAlgebra::Vector< Scalar >& x, __out LinearAlgebra::Vector< Scalar >& p ) = 0;

	};

} }