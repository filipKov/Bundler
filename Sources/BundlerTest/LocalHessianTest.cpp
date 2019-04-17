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
			// HRESULT hr = Import::BundleImporter::Import( "Resources/gargoyle.out", pBundle, pMetadata );
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

		void TestLocalHessianCamera(
			__in const ProjectionProvider< CamModel >* pGJacobian,
			__in const LocalProjectionProviderCPU< CamModel >* pLJacobian )
		{
			HessianBlockProvider< CamModel > hessian;
			hessian.Initialize( pGJacobian );

			LocalHessianBlockProviderCPU< CamModel > localHessian;
			localHessian.Initialize( pLJacobian );

			Scalar gCamBlock[CamModel::cameraParameterCount * CamModel::cameraParameterCount], lCamBlock[CamModel::cameraParameterCount * CamModel::cameraParameterCount];
			Scalar gCamPtBlock[CamModel::cameraParameterCount * POINT_PARAM_COUNT], lCamPtBlock[CamModel::cameraParameterCount * POINT_PARAM_COUNT];

			const uint count = pLJacobian->GetCameraCount( );
			for ( uint localIx = 0; localIx < count; localIx++ )
			{
				const uint globalIx = pLJacobian->GetGlobalCameraIndex( localIx );

				hessian.GetCameraBlock( globalIx, gCamBlock );
				localHessian.GetCameraBlock( localIx, lCamBlock );

				AssertAreEqual( gCamBlock, lCamBlock );

				const uint globalProjCount = ( uint )hessian.GetCameraProjectionCount( globalIx );
				const uint localProjCount = localHessian.GetCameraProjectionCount( localIx );

				Assert::AreEqual( globalProjCount, localProjCount );

				for ( uint projI = 0; projI < globalProjCount; projI++ )
				{
					size_t gPtIx = 0;
					uint lPtIx = 0;

					hessian.GetCameraPointBlockCam( globalIx, projI, &gPtIx, gCamPtBlock );
					localHessian.GetCameraPointBlockCam( localIx, projI, &lPtIx, lCamPtBlock );

					Assert::AreEqual( ( uint )gPtIx, localHessian.GetGlobalPointIndex( lPtIx ) );
					AssertAreEqual( gCamPtBlock, lCamPtBlock );
				}
			}
		}

		void TestLocalHessianPoints(
			__in const ProjectionProvider< CamModel >* pGJacobian,
			__in const LocalProjectionProviderCPU< CamModel >* pLJacobian )
		{
			HessianBlockProvider< CamModel > hessian;
			hessian.Initialize( pGJacobian );

			LocalHessianBlockProviderCPU< CamModel > localHessian;
			localHessian.Initialize( pLJacobian );

			Scalar gPtBlock[POINT_PARAM_COUNT * POINT_PARAM_COUNT], lPtBlock[POINT_PARAM_COUNT * POINT_PARAM_COUNT];
			Scalar gPtCamBlock[CamModel::cameraParameterCount * POINT_PARAM_COUNT], lPtCamBlock[CamModel::cameraParameterCount * POINT_PARAM_COUNT];

			const uint count = pLJacobian->GetPointCount( );
			for ( uint localIx = 0; localIx < count; localIx++ )
			{
				const uint globalIx = pLJacobian->GetGlobalPointIndex( localIx );

				hessian.GetPointBlock( globalIx, gPtBlock );
				localHessian.GetPointBlock( localIx, lPtBlock );

				AssertAreEqual( gPtBlock, lPtBlock );

				const uint globalProjCount = ( uint )hessian.GetPointProjectionCount( globalIx );
				const uint localProjCount = localHessian.GetPointProjectionCount( localIx );

				Assert::AreEqual( globalProjCount, localProjCount );

				for ( uint projI = 0; projI < globalProjCount; projI++ )
				{
					size_t gCamIx = 0;
					uint lCamIx = 0;

					hessian.GetCameraPointBlockPt( globalIx, projI, &gCamIx, gPtCamBlock );
					localHessian.GetCameraPointBlockPt( localIx, projI, &lCamIx, lPtCamBlock );

					Assert::AreEqual( ( uint )gCamIx, localHessian.GetGlobalCameraIndex( lCamIx ) );
					AssertAreEqual( gPtCamBlock, lPtCamBlock );
				}
			}
		}

		void TestLocalHessianMECamera(
			__in const ProjectionProvider< CamModel >* pGJacobian,
			__in const LocalProjectionProviderCPU< CamModel >* pLJacobian )
		{
			HessianMultiplicationEngine< CamModel > hessian;
			hessian.Initialize( pGJacobian, 1 );

			LocalHessianMultiplicationEngineCPU< CamModel > localHessian;
			localHessian.Initialize( pLJacobian, 1 );
				
			uint camParamCount = ( uint )( hessian.GetCameraCount( ) * CamModel::cameraParameterCount );
			uint pointParamCount = ( uint )( hessian.GetPointCount( ) * POINT_PARAM_COUNT );
			uint totalParamCount = camParamCount + pointParamCount;

			Vector< Scalar > x( totalParamCount );
			Random< Scalar >::Fill( totalParamCount, x.Elements( ) );

			Scalar gy[CamModel::cameraParameterCount];
			Scalar ly[CamModel::cameraParameterCount];

			const uint count = pLJacobian->GetCameraCount( );
			for ( uint localIx = 0; localIx < count; localIx++ )
			{
				const uint globalIx = pLJacobian->GetGlobalCameraIndex( localIx );

				hessian.MultiplyCameraRow( globalIx, totalParamCount, x.Elements(), gy );
				localHessian.MultiplyCameraRow( localIx, camParamCount, x.Elements(), pointParamCount, x.Elements() + camParamCount, ly );

				AssertAreEqual( gy, ly );
			}
		}

		void TestLocalHessianMEPoints(
			__in const ProjectionProvider< CamModel >* pGJacobian,
			__in const LocalProjectionProviderCPU< CamModel >* pLJacobian )
		{
			HessianMultiplicationEngine< CamModel > hessian;
			hessian.Initialize( pGJacobian, 1 );

			LocalHessianMultiplicationEngineCPU< CamModel > localHessian;
			localHessian.Initialize( pLJacobian, 1 );

			uint camParamCount = ( uint )( hessian.GetCameraCount( ) * CamModel::cameraParameterCount );
			uint pointParamCount = ( uint )( hessian.GetPointCount( ) * POINT_PARAM_COUNT );
			uint totalParamCount = camParamCount + pointParamCount;

			Vector< Scalar > x( totalParamCount );
			Random< Scalar >::Fill( totalParamCount, x.Elements( ) );

			Scalar gy[POINT_PARAM_COUNT];
			Scalar ly[POINT_PARAM_COUNT];

			const uint count = pLJacobian->GetPointCount( );
			for ( uint localIx = 0; localIx < count; localIx++ )
			{
				const uint globalIx = pLJacobian->GetGlobalPointIndex( localIx );

				hessian.MultiplyPointRow( globalIx, totalParamCount, x.Elements( ), gy );
				localHessian.MultiplyPointRow( localIx, camParamCount, x.Elements( ), pointParamCount, x.Elements( ) + camParamCount, ly );

				AssertAreEqual( gy, ly );
			}
		}

		void MultiplyByHessian(
			__in const ProjectionProvider< CamModel >* pJacobian,
			__in const Scalar diagonalFactor,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY ) const
		{
			HessianMultiplicationEngine< CamModel > hessian;
			hessian.Initialize( pJacobian, diagonalFactor );

			auto pHessian = &hessian;

			Scalar* pYDestination = pY;

			const size_t cameraCount = pHessian->GetCameraCount( );
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				pHessian->MultiplyCameraRow( cameraIx, vectorSize, pX, pYDestination );
				pYDestination += CamModel::cameraParameterCount;
			}

			const size_t pointCount = pHessian->GetPointCount( );
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				pHessian->MultiplyPointRow( pointIx, vectorSize, pX, pYDestination );
				pYDestination += POINT_PARAM_COUNT;
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

		TEST_METHOD( LocalHessianBlockProvider0 )
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

				TestLocalHessianCamera( &globalJacobian, &localJacobian );
			}

			const uint localPtCount = 50;
			const uint pointCount = ( uint )globalJacobian.GetPointCount( );
			for ( uint i = 0; i < pointCount; )
			{
				const uint trueLocalCount = min( localPtCount, pointCount - i );

				LocalProjectionProviderCPU< CamModel > localJacobian;
				localJacobian.InitializeForPoints( &globalJacobian, i, trueLocalCount );
				i += trueLocalCount;

				TestLocalHessianPoints( &globalJacobian, &localJacobian );
			}
		}

		TEST_METHOD( LocalHessianMultiplicationEngine0 )
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

				TestLocalHessianMECamera( &globalJacobian, &localJacobian );
			}

			const uint localPtCount = 50;
			const uint pointCount = ( uint )globalJacobian.GetPointCount( );
			for ( uint i = 0; i < pointCount; )
			{
				const uint trueLocalCount = min( localPtCount, pointCount - i );

				LocalProjectionProviderCPU< CamModel > localJacobian;
				localJacobian.InitializeForPoints( &globalJacobian, i, trueLocalCount );
				i += trueLocalCount;

				TestLocalHessianMEPoints( &globalJacobian, &localJacobian );
			}
		}

	};

}