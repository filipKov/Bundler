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

	return 0;
}