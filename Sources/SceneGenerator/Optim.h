#pragma once

using namespace Bundler;
using namespace Bundler::Import;
using namespace Bundler::SceneGenerator;

template < uint noiseMask >
struct NoiseMaskToCameraModel
{
};

template < >
struct NoiseMaskToCameraModel< SceneGenAutoNoiseMask::POINTS >
{
	template < uint maxIters >
	using CameraModel = typename CameraModels::CameraModelAMP6DoF< maxIters >;
};

template < class CameraModel, uint maxIterations >
void OptimizeBundle( __inout Bundle* pBundle, __out OptimizerStatistics* pStats )
{
	OptimizerSettings settings;
	settings.errorTolerance = Scalar( 0.01 );
	settings.dampeningUp = Scalar( 10 );
	settings.dampeningDown = Scalar( 0.1 );
	settings.initialDampeningFactor = 0;
	settings.maxIterations = maxIterations;
	settings.linearSolverSettings.errorTolerance = Scalar( 0.001 );
	settings.linearSolverSettings.maxIterations = maxIterations * 2;

	BundleOptimizer< CameraModel, LinearSolver::BlockJacobiPreconditioner > optimizer;
	optimizer.Initialize( settings );
	optimizer.Optimize( pBundle, pStats );
}

template < class CameraModel, uint maxIterations >
void OptimizeBundleParallel( __inout Bundle* pBundle, __out OptimizerStatistics* pStats )
{
	OptimizerSettings settings;
	settings.errorTolerance = Scalar( 0.01 );
	settings.dampeningUp = Scalar( 10 );
	settings.dampeningDown = Scalar( 0.1 );
	settings.initialDampeningFactor = 0;
	settings.maxIterations = maxIterations;
	settings.linearSolverSettings.errorTolerance = Scalar( 0.001 );
	settings.linearSolverSettings.maxIterations = maxIterations * 2;

	Async::WorkerPool wpool;
	ParallelBundleOptimizer< CameraModel > optimizer2;
	optimizer2.Initialize( settings, &wpool );
	optimizer2.Optimize( pBundle, pStats );
}