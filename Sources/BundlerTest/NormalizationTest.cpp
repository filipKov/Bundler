#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include "TestData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	using CamModel = CameraModel6DoF< 5 >;

	TEST_CLASS( NormalizationTest )
	{
		void GetTestBundle( __out Bundle* pBundle, __out_opt BundleAdditionalPayload* pMetadata )
		{
			// HRESULT hr = Import::BundleImporter::Import( "Resources/dragon.out", pBundle, pMetadata );
			HRESULT hr = E_FAIL;
			if ( FAILED( hr ) )
			{
				std::istringstream bundleStream( cubesMaskedBundle );
				std::istringstream imListStream( cubesMaskedImageList );
	
				hr = Import::BundleImporter::Import( &bundleStream, &imListStream, pBundle, pMetadata );
			}
	
			Assert::IsTrue( SUCCEEDED( hr ) );
		}
	
		void GetGlobalProjectionProvider( __in Bundle* pBundle, __inout CamModel* pCameras, __out ProjectionProvider< CamModel >* pProj )
		{
			for ( size_t i = 0; i < pBundle->cameras.Length( ); i++ )
			{
				pCameras[i].Initialize( &pBundle->cameras[i] );
			}
	
			pProj->Initialize(
				pBundle->cameras.Length( ),
				pCameras,
				pBundle->points.Length( ),
				pBundle->points.Data( ),
				pBundle->projections.Length( ),
				pBundle->projections.Data( )
			);
		}
	
		void GetResiduals( __in ProjectionProvider< CamModel >* pProj, __in const size_t projCount, __out_ecount( 2 * projCount ) Scalar* pResiduals )
		{
			for ( size_t i = 0; i < projCount; i++ )
			{
				pProj->GetProjectionBlock< false, false, true >( i, NULL, NULL, pResiduals );
				pResiduals += 2;
			}
		}

		TEST_METHOD( NormalizedResiduals )
		{
			Bundle bundle;
			Bundle normalizedBundle;
			BundleAdditionalPayload metadata;
			GetTestBundle( &bundle, &metadata );
			Preprocess::Normalize( &bundle, &normalizedBundle, NULL, NULL );


			Containers::Buffer< CamModel > cameras;
			Containers::Buffer< CamModel > normalizedCameras;

			cameras.Allocate( bundle.cameras.Length( ) );
			normalizedCameras.Allocate( normalizedBundle.cameras.Length( ) );

			ProjectionProvider< CamModel > globalJacobian;
			ProjectionProvider< CamModel > normalizedJacobian;

			GetGlobalProjectionProvider( &bundle, cameras.Data( ), &globalJacobian );
			GetGlobalProjectionProvider( &normalizedBundle, normalizedCameras.Data( ), &normalizedJacobian );


			const size_t projCount = globalJacobian.GetProjectionCount();

			Containers::Buffer< Scalar > expectedProjections;
			Containers::Buffer< Scalar > actualProjections;
			
			expectedProjections.Allocate( projCount * 2 );
			actualProjections.Allocate( projCount * 2 );

			GetResiduals( &globalJacobian, projCount, expectedProjections.Data( ) );
			GetResiduals( &normalizedJacobian, projCount, actualProjections.Data( ) );

			AssertAreEqual( projCount * 2, expectedProjections.Data( ), actualProjections.Data( ), Scalar( 10e-6 ) );
		}
	
	};

}