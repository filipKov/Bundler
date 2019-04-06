#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include "TestData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	using CamModel = CameraModel6DoF< 5 >;

	TEST_CLASS( LocalHessianTest )
	{
		void GetTestBundle( __out Bundle* pBundle, __out_opt BundleAdditionalPayload* pMetadata )
		{
			std::istringstream bundleStream( cubesMaskedBundle );
			std::istringstream imListStream( cubesMaskedImageList );

			Import::BundleImporter::Import( &bundleStream, &imListStream, pBundle, pMetadata );
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

		void TestProjections(
			__in const ProjectionProvider< CamModel >* pGJacobian,
			__in const LocalProjectionProviderCPU< CamModel >* pLJacobian )
		{
			Scalar gCamBlock[2 * CamModel::cameraParameterCount], lCamBlock[2 * CamModel::cameraParameterCount];
			Scalar gPtBlock[2 * POINT_PARAM_COUNT], lPtBlock[2 * POINT_PARAM_COUNT];
			Scalar gResiduals[2], lResiduals[2];


			const uint count = pLJacobian->GetCameraCount( );
			for ( uint localIx = 0; localIx < count; localIx++ )
			{
				const uint globalIx = pLJacobian->GetGlobalCameraIndex( localIx );

				const uint globalProjCount = (uint)pGJacobian->GetCameraProjectionCount( globalIx );
				const uint localProjCount = pLJacobian->GetCameraProjectionCount( localIx );

				Assert::AreEqual( globalProjCount, localProjCount );
				
				for ( uint projI = 0; projI < globalProjCount; projI++ )
				{
					const uint globalProjIx = (uint)pGJacobian->GetCameraProjectionIndex( globalIx, projI );
					const uint localProjIx = pLJacobian->GetCameraProjectionIndex( localIx, projI );

					Assert::AreEqual( globalProjIx, localProjIx );

					pGJacobian->GetProjectionBlock< true, true, true >( globalProjIx, gCamBlock, gPtBlock, gResiduals );
					pLJacobian->GetProjectionBlock< true, true, true >( localProjIx, lCamBlock, lPtBlock, lResiduals );

					AssertAreEqual( gCamBlock, lCamBlock );
					AssertAreEqual( gPtBlock, lPtBlock );
					AssertAreEqual( gResiduals, lResiduals );
				}
			}
		}

		TEST_METHOD( LocalProjectionProvider0 )
		{
			Bundle bundle;
			BundleAdditionalPayload metadata;

			GetTestBundle( &bundle, &metadata );

			Containers::Buffer< CamModel > cameras;
			cameras.Allocate( bundle.cameras.Length( ) );
			
			ProjectionProvider< CamModel > globalJacobian;
			GetGlobalProjectionProvider( &bundle, cameras.Data( ), &globalJacobian );

			const uint localCount = 3;
			const uint cameraCount = ( uint )cameras.Length( );
			for ( uint i = 0; i < cameraCount; )
			{
				const uint trueLocalCount = min( localCount, cameraCount - i );

				LocalProjectionProviderCPU< CamModel > localJacobian;
				localJacobian.InitializeForCameras( &globalJacobian, i, trueLocalCount );
				i += trueLocalCount;

				Assert::AreEqual( trueLocalCount, localJacobian.GetCameraCount( ) );
				TestProjections( &globalJacobian, &localJacobian );
			}
		}
	};

}