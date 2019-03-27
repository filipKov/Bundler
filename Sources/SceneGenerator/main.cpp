#include "stdafx.h"

#include "ViewerLauncher.h"

using namespace Bundler;
using namespace Bundler::Import;
using namespace Bundler::SceneGenerator;

void LaunchViewer( 
	__in const uint count,
	__in_ecount( count ) const Vector3* pVertices,
	__in_ecount_opt( count ) const uint* pVertexColors )
{
	ViewerLauncher::InitializePointCloud( count, pVertices, pVertexColors );
	ViewerLauncher::LaunchViewer( );
}

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


int main( int argc, char **argv ) 
{
	Bundle bundle;
	BundleAdditionalPayload metadata;

	HRESULT hr = BundleImporter::Import( GET_RESOURCE_PATH( "gargoyle.out" ), &bundle, &metadata );
	if ( SUCCEEDED( hr ) )
	{
		constexpr uint noiseMask = SceneGenAutoNoiseMask::POINTS;

		SceneGenNoiseSettings noise = { 0 };
		SceneGen::GetAutoNoise( &bundle, noiseMask, &noise );

		Bundle bundle2;
		SceneGen::AddNoise( &noise, &bundle, &bundle2 );

		//std::thread viewerThread( LaunchViewer, ( uint )bundle2.points.Length( ), bundle2.points.Data( ), metadata.pointColors.Data( ) );

		OptimizerStatistics optimizerStats;
		// std::thread optimizerThread( OptimizeBundle< NoiseMaskToCameraModel< noiseMask >::CameraModel< 10 >, 10 >, &bundle2, &optimizerStats );
		//optimizerThread.join( );

		OptimizeBundle< NoiseMaskToCameraModel< noiseMask >::CameraModel< 10 >, 10 >( &bundle2, &optimizerStats );

		//viewerThread.join( );
	}

	// if ( SUCCEEDED( hr ) )
	// {
	// 	ViewerLauncher::InitializePointCloud( ( uint )bundle.points.Length( ), bundle.points.Data( ), metadata.pointColors.Data( ) );
	// 	ViewerLauncher::LaunchViewer( );
	// }

	return 0;
}