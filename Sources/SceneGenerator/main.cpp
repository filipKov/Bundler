#include "stdafx.h"

#include "Optim.h"

#include "ViewerLauncher.h"

void LaunchViewer( 
	__in const uint count,
	__in_ecount( count ) const Vector3* pVertices,
	__in_ecount_opt( count ) const uint* pVertexColors )
{
	ViewerLauncher::InitializePointCloud( count, pVertices, pVertexColors );
	ViewerLauncher::LaunchViewer( );
}

int main( int argc, char **argv ) 
{
	UNREFERENCED_PARAMETER( argc );
	UNREFERENCED_PARAMETER( argv );

	Async::ListAccelerators( [ ] ( accelerator ac ) { return true; } );

	Bundle bundle;
	BundleAdditionalPayload metadata;

	HRESULT hr = BundleImporter::Import( GET_RESOURCE_PATH( "schwimmy.out" ), &bundle, &metadata );
	if ( SUCCEEDED( hr ) )
	{
		Scalar mean[3];
		Scalar stdev = 0;
		Preprocess::Normalize( &bundle, mean, &stdev );

		constexpr uint noiseMask = SceneGenAutoNoiseMask::POINTS;

		SceneGenNoiseSettings noise = { 0 };
		SceneGen::GetAutoNoise( &bundle, noiseMask, &noise );

		Bundle bundle2;
		SceneGen::AddNoise( &noise, &bundle, &bundle2 );

		//std::thread viewerThread( LaunchViewer, ( uint )bundle2.points.Length( ), bundle2.points.Data( ), metadata.pointColors.Data( ) );

		OptimizerStatistics optimizerStats;
		// std::thread optimizerThread( OptimizeBundle< NoiseMaskToCameraModel< noiseMask >::CameraModel< 10 >, 10 >, &bundle2, &optimizerStats );
		//optimizerThread.join( );

		OptimizeBundle< NoiseMaskToCameraModel< noiseMask >::CameraModel< 11 >, 10 >( &bundle2, &optimizerStats );
		OptimizeBundleParallel< NoiseMaskToCameraModel< noiseMask >::CameraModel< 11 >, 10 >( &bundle2, &optimizerStats );

		//viewerThread.join( );
	}

	// if ( SUCCEEDED( hr ) )
	// {
	// 	ViewerLauncher::InitializePointCloud( ( uint )bundle.points.Length( ), bundle.points.Data( ), metadata.pointColors.Data( ) );
	// 	ViewerLauncher::LaunchViewer( );
	// }

	return 0;
}