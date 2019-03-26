
#include "stdafx.h"

#include "ViewerLauncher.h"

int main( int argc, char **argv ) 
{
	Bundler::Bundle bundle;
	Bundler::BundleAdditionalPayload metadata;

	HRESULT hr = Bundler::Import::BundleImporter::Import( GET_RESOURCE_PATH( "dragon.out" ), &bundle, &metadata );
	if ( SUCCEEDED( hr ) )
	{
		ViewerLauncher::InitializePointCloud( ( uint )bundle.points.Length( ), bundle.points.Data( ), metadata.pointColors.Data( ) );
		ViewerLauncher::LaunchViewer( );
	}

	return 0;
}