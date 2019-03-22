#pragma once

namespace Bundler { namespace LinearSolver {

	struct PCGSolverSettings
	{
		uint maxIterations;
		Scalar errorTolerance;
	};

	struct PCGSolverStatistics
	{
		uint iterationCount;
		Scalar finalError;
	};

	class PCGSolverTemp
	{
	public:

		Vector< Scalar > r;
		Vector< Scalar > d;
		Vector< Scalar > Ad;
		Vector< Scalar > MInvR;

		Scalar errSq;

		PCGSolverTemp( __in const uint paramCount ) :
			r( paramCount ),
			d( paramCount ),
			Ad( paramCount ),
			MInvR( paramCount )
		{
			errSq = Scalar( DBL_MAX );
		}
	};

} }