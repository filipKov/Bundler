#pragma once

namespace Bundler { namespace LinearSolver {

	class ParallelPCGSolverTemp : public PCGSolverTemp
	{
	public:

		Async::InterlockedVariable< Scalar > errSqNew;
		Async::InterlockedVariable< Scalar > dDotAd;

		ParallelPCGSolverTemp( __in const uint paramCount ) : PCGSolverTemp( paramCount )
		{
			errSqNew = 0;
			dDotAd = 0;
		}

	};

} }
