#pragma once

namespace Bundler {

	struct OptimizerSettings
	{
		uint maxIterations;
		double errorTolerance;
	
		Scalar initialDampeningFactor;
		Scalar dampeningUp;
		Scalar dampeningDown;
	
		LinearSolver::PCGSolverSettings linearSolverSettings;
	};
	
	struct OptimizerStatistics
	{
		double initialGeometricError;
		double finalGeometricError;
		uint iterationCount;
		uint acceptedIterations;
		uint rejectedIterations;
	};

}