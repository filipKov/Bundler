#pragma once

namespace Bundler {

	struct OptimizerSettings
	{
		uint maxIterations;
		Scalar errorTolerance;
	
		Scalar initialDampeningFactor;
		Scalar dampeningUp;
		Scalar dampeningDown;
	
		LinearSolver::PCGSolverSettings linearSolverSettings;
	};
	
	struct OptimizerStatistics
	{
		Scalar initialGeometricError;
		Scalar finalGeometricError;
		uint iterationCount;
		uint acceptedIterations;
		uint rejectedIterations;
	};

}