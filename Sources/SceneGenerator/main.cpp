#include "stdafx.h"
#include <fstream>

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

void PrintBundle( __in const Bundle* pBundle, __in std::ostream* pStream )
{
	( *pStream ) << pBundle->points[0][0];
	for ( size_t i = 1; i < pBundle->points.Length( ); i++ )
	{
		( *pStream ) << "#" << pBundle->points[i][0];
	}
	( *pStream ) << std::endl;

	( *pStream ) << pBundle->points[0][1];
	for ( size_t i = 1; i < pBundle->points.Length( ); i++ )
	{
		( *pStream ) << "#" << pBundle->points[i][1];
	}
	( *pStream ) << std::endl;

	( *pStream ) << pBundle->points[0][2];
	for ( size_t i = 1; i < pBundle->points.Length( ); i++ )
	{
		( *pStream ) << "#" << pBundle->points[i][2];
	}
	( *pStream ) << std::endl;
}

int main( int argc, char **argv ) 
{
	UNREFERENCED_PARAMETER( argc );
	UNREFERENCED_PARAMETER( argv );

	Async::ListAccelerators( [ ] ( accelerator ac ) { return true; } );

	Bundle bundle;
	BundleAdditionalPayload metadata;

	HRESULT hr = BundleImporter::Import( GET_RESOURCE_PATH( "gargoyle.out" ), &bundle, &metadata );
	if ( SUCCEEDED( hr ) )
	{
		Scalar mean[3];
		Scalar stdev = 0;
		Bundle normalizedBundle;
		Preprocess::Normalize( &bundle, &normalizedBundle, mean, &stdev );

		double center[3];
		double normalizedStdev;
		Preprocess::GetMean( &normalizedBundle, center );
		Preprocess::GetStdev( &normalizedBundle, center, &normalizedStdev );

		Scalar filterCenter[3];
		Scalar filterRadius = Scalar( 2 * normalizedStdev );
		MatrixCast< double, Scalar, 3, 1 >( center, filterCenter );

		Bundle filteredBundle;
		Preprocess::FilterAroundCenter( &normalizedBundle, filterCenter, filterRadius, &filteredBundle, NULL );


		constexpr uint noiseMask = SceneGenAutoNoiseMask::POINTS;
		
		const Scalar noiseStrength = filterRadius * Scalar( 0.035 );
		SceneGenNoiseSettings noise = { 0 };
		noise.pointSettings.minDelta = -noiseStrength;
		noise.pointSettings.maxDelta = noiseStrength;

		// SceneGen::GetAutoNoise( &normalizedBundle, noiseMask, &noise );		
		
		Bundle noisyBundle;
		SceneGen::AddNoise( &noise, &filteredBundle, &noisyBundle );
		
		// //std::thread viewerThread( LaunchViewer, ( uint )bundle2.points.Length( ), bundle2.points.Data( ), metadata.pointColors.Data( ) );
		 
		OptimizerStatistics optimizerStats;
		// // std::thread optimizerThread( OptimizeBundle< NoiseMaskToCameraModel< noiseMask >::CameraModel< 10 >, 10 >, &bundle2, &optimizerStats );
		// //optimizerThread.join( );
		
		OptimizeBundle< NoiseMaskToCameraModel< noiseMask >::CameraModel< 5 >, 4 >( &noisyBundle, &optimizerStats );
		// OptimizeBundleParallel< NoiseMaskToCameraModel< noiseMask >::CameraModel< 11 >, 10 >( &bundle2, &optimizerStats );
		
		// //viewerThread.join( );
	}

	// if ( SUCCEEDED( hr ) )
	// {
	// 	ViewerLauncher::InitializePointCloud( ( uint )bundle.points.Length( ), bundle.points.Data( ), metadata.pointColors.Data( ) );
	// 	ViewerLauncher::LaunchViewer( );
	// }

	return 0;
}