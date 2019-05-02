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
	using CameraModel = typename CameraModels::CameraModel6DoF< maxIters >;
};

template < class CameraModel, uint maxIterations >
void OptimizeBundle( __in const Bundle* pBundle, __out OptimizerStatistics* pStats, __out Bundle* pOptimizedBundle )
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

	if ( pBundle != pOptimizedBundle )
	{
		Bundler::Utils::CopyBundle( pBundle, pOptimizedBundle );
	}

	optimizer.Optimize( pBundle, pStats );
}

template < class CameraModel, uint maxIterations >
void OptimizeBundleParallel( __in const Bundle* pBundle, __out OptimizerStatistics* pStats, __out Bundle* pOptimizedBundle )
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

	if ( pBundle != pOptimizedBundle )
	{
		Bundler::Utils::CopyBundle( pBundle, pOptimizedBundle );
	}

	optimizer2.Optimize( pOptimizedBundle, pStats );
}