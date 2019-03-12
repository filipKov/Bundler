#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class Preconditioner
	{
	public:

		virtual void Apply(
			__in const HessianBlockProvider< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pPreconditionedX ) const = 0;

	};

} }