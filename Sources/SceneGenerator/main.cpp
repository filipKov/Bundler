#include "stdafx.h"
#include <fstream>

#include "Optim.h"

#include "ApplicationContext.h"
#include "BundlerApp.h"

#include "ViewerLauncher.h"

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
	BundlerApp* pApp = BundlerApp::GetApp( );
	if ( pApp )
	{
		try
		{
			pApp->Start( argc, argv );
		}
		catch ( Exception err )
		{
			printf_s( "%s", err.GetMessage( ) );
		}

		pApp->Release( );
	}

	// UNREFERENCED_PARAMETER( argc );
	// UNREFERENCED_PARAMETER( argv );
	// 
	// _ASSERT_EXPR( argc > 1, "Missing path to bundle" );
	// 
	// Async::ListAccelerators( [ ] ( accelerator ac ) { return true; } );
	// 
	// Bundle bundle;
	// BundleAdditionalPayload metadata;
	// 
	// HRESULT hr = BundleImporter::Import( argv[1], &bundle, &metadata );
	// if ( SUCCEEDED( hr ) )
	// {
	// 	Scalar mean[3];
	// 	Scalar stdev = 0;
	// 	Bundle normalizedBundle;
	// 	Preprocess::Normalize( &bundle, &normalizedBundle, mean, &stdev );
	// 
	// 	double center[3];
	// 	double normalizedStdev;
	// 	Preprocess::GetMean( &normalizedBundle, center );
	// 	Preprocess::GetStdev( &normalizedBundle, center, &normalizedStdev );
	// 
	// 	Scalar filterCenter[3];
	// 	Scalar filterRadius = Scalar( 1 * normalizedStdev );
	// 	MatrixCast< double, Scalar, 3, 1 >( center, filterCenter );
	// 
	// 	Bundle filteredBundle;
	// 	BundleAdditionalPayload filteredMetadata;
	// 	Preprocess::FilterAroundCenter( &normalizedBundle, filterCenter, filterRadius, &filteredBundle, &metadata, &filteredMetadata, NULL );
	// 
	// 
	// 	constexpr uint noiseMask = SceneGenAutoNoiseMask::POINTS;
	// 	
	// 	const Scalar noiseStrength = filterRadius * Scalar( 0.0035 );
	// 	SceneGenNoiseSettings noise = { 0 };
	// 	noise.pointSettings.minDelta = -noiseStrength;
	// 	noise.pointSettings.maxDelta = noiseStrength;
	// 
	// 	Bundle noisyBundle;
	// 	SceneGen::AddNoise( &noise, &filteredBundle, &noisyBundle );
	// 	
	// 	// std::thread viewerThread( LaunchViewer, &bundle, &metadata );
	// 	// Bundler::BundleAdditionalPayload* pMeta = NULL;
	// 	// std::thread viewerThread( LaunchViewer, &filteredBundle, pMeta );
	// 
	// 	
	// 	// // std::thread optimizerThread( OptimizeBundle< NoiseMaskToCameraModel< noiseMask >::CameraModel< 10 >, 10 >, &bundle2, &optimizerStats );
	// 	// //optimizerThread.join( );
	// 	
	// 	Bundle optimizedBundle;
	// 	OptimizerStatistics optimizerStats;
	// 	// OptimizeBundle< NoiseMaskToCameraModel< noiseMask >::CameraModel< 5 >, 4 >( &noisyBundle, &optimizerStats, &optimizedBundle );
	// 	OptimizeBundleParallel< NoiseMaskToCameraModel< noiseMask >::CameraModel< 26 >, 25 >( &noisyBundle, &optimizerStats, &optimizedBundle );
	// 	
	// 	// viewerThread.join( );
	// 
	// 	double inputError = GetGeometricError< NoiseMaskToCameraModel< noiseMask >::CameraModel< 5 > >( &bundle );
	// 	double filteredError = GetGeometricError< NoiseMaskToCameraModel< noiseMask >::CameraModel< 5 > >( &filteredBundle );
	// 
	// 	printf( "Input data error (mean): %f (%f)\n", inputError, inputError / ( bundle.projections.Length() * 2 ) );
	// 	printf( "Filtered data error (mean): %f (%f)\n", filteredError, filteredError / ( filteredBundle.projections.Length() * 2 ) );
	// 
	// 	ViewerLauncher::InitializeEmpty( );
	// 
	// 	ViewerLauncher::ShowBundle( &normalizedBundle, &metadata, filterCenter );
	// 	ViewerLauncher::ShowBundle( &filteredBundle, &filteredMetadata, filterCenter );
	// 	ViewerLauncher::ShowBundle( &noisyBundle, &filteredMetadata, filterCenter );
	// 	ViewerLauncher::ShowBundle( &optimizedBundle, &filteredMetadata, filterCenter );
	// 
	// 
	// 	ViewerLauncher::LaunchViewer( );
	// }

	// if ( SUCCEEDED( hr ) )
	// {
	// 	ViewerLauncher::InitializePointCloud( ( uint )bundle.points.Length( ), bundle.points.Data( ), metadata.pointColors.Data( ) );
	// 	ViewerLauncher::LaunchViewer( );
	// }

	return 0;
}