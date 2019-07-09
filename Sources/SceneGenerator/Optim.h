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

template < class CameraModel >
void OptimizeBundleSeq( __in const Bundle* pBundle, __in const OptimizerSettings* pSettings, __out OptimizerStatistics* pStats, __out Bundle* pOptimizedBundle )
{
	BundleOptimizer< CameraModel > optimizer;
	optimizer.Initialize( *pSettings );

	if ( pBundle != pOptimizedBundle )
	{
		Bundler::Utils::CopyBundle( pBundle, pOptimizedBundle );
	}

	optimizer.Optimize( pOptimizedBundle, pStats );
}

template < class CameraModel >
void OptimizeBundleParallelCPU( __in const Bundle* pBundle, __in const OptimizerSettings* pSettings, __out OptimizerStatistics* pStats, __out Bundle* pOptimizedBundle )
{
	BundleOptimizerOpenMP< CameraModel > optimizer;
	optimizer.Initialize( *pSettings );

	if ( pBundle != pOptimizedBundle )
	{
		Bundler::Utils::CopyBundle( pBundle, pOptimizedBundle );
	}

	optimizer.Optimize( pOptimizedBundle, pStats );
}

template < class CameraModel >
void OptimizeBundleParallelCombined( __in const Bundle* pBundle, __in const OptimizerSettings* pSettings, __out OptimizerStatistics* pStats, __out Bundle* pOptimizedBundle )
{
	Async::WorkerPool wpool;
	ParallelBundleOptimizer< CameraModel > optimizer;
	optimizer.Initialize( *pSettings, &wpool );

	if ( pBundle != pOptimizedBundle )
	{
		Bundler::Utils::CopyBundle( pBundle, pOptimizedBundle );
	}

	optimizer.Optimize( pOptimizedBundle, pStats );
}

template < class CameraModel >
void InitializeJacobian(
	__in const Bundle* pBundle,
	__out CameraModel* pCamModels,
	__out ProjectionProvider< CameraModel >* pJacobian )
{
	const int cameraCount = ( int )pBundle->cameras.Length( );

	#pragma omp parallel for
	for ( int i = 0; i < cameraCount; i++ )
	{
		pCamModels[i].Initialize( &pBundle->cameras[i] );
	}

	pJacobian->Initialize(
		cameraCount,
		pCamModels,
		pBundle->points.Length( ),
		pBundle->points.Data( ),
		pBundle->projections.Length( ),
		pBundle->projections.Data( )
	);
}

template < class CameraModel >
double GetGeometricError( __in const Bundle* pBundle )
{
	Containers::Buffer< CameraModel > camModels;
	camModels.Allocate( pBundle->cameras.Length( ) );
	
	ProjectionProvider< CameraModel > jacobian;
	InitializeJacobian( pBundle, camModels.Data( ), &jacobian );

	return ParallelBundleOptimizer< CameraModel >::GetGeometricError( &jacobian );
}
