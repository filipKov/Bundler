#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include "TestData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	static Bundle dummyBundleZeroError;

	TEST_CLASS( ProjectionProviderTest )
	{
	public:

		ProjectionProviderTest()
		{
			dummyBundleZeroError.cameras.Allocate( 3 );
			SetCamera( { 1.0f, 2.25f, 5.0f }, 45, { 0, 0, 1 }, dummyBundleZeroError.cameras[ 0 ] );
			SetCamera( { 3.0f, 2.0f, 5.0f }, 0, { 0, 0, 1 }, dummyBundleZeroError.cameras[ 1 ] );
			SetCamera( { 5.0f, 2.25f, 5.0f }, -45, { 0, 0, 1 }, dummyBundleZeroError.cameras[ 2 ] );

			dummyBundleZeroError.points.Allocate( 5 );
			SetPoint( { 2.3f, 4.1f, 3.0f }, dummyBundleZeroError.points[ 0 ] );
			SetPoint( { 2.7f, 3.8f, 4.5f }, dummyBundleZeroError.points[ 1 ] );
			SetPoint( { 3.1f, 3.9f, 5.2f }, dummyBundleZeroError.points[ 2 ] );
			SetPoint( { 3.6f, 4.15f, 5.0f }, dummyBundleZeroError.points[ 3 ] );
			SetPoint( { 2.9f, 4.5f, 6.0f }, dummyBundleZeroError.points[ 4 ] );

			dummyBundleZeroError.projections.Allocate( 9 );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 0, 0, dummyBundleZeroError.projections[0] );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 0, 1, dummyBundleZeroError.projections[1] );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 1, 0, dummyBundleZeroError.projections[2] );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 1, 1, dummyBundleZeroError.projections[3] );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 1, 2, dummyBundleZeroError.projections[4] );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 1, 3, dummyBundleZeroError.projections[5] );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 2, 2, dummyBundleZeroError.projections[6] );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 2, 3, dummyBundleZeroError.projections[7] );
			SetProjection( dummyBundleZeroError.cameras.Data(), dummyBundleZeroError.points.Data(), 2, 4, dummyBundleZeroError.projections[8] );
		}

		void SetCamera( __in const Scalar( &pos )[ 3 ], __in const Scalar rotAngle, __in const Scalar( &rotAxis )[ 3 ], __out Camera& camera )
		{
			camera.t = Vector3( pos );
			RodriguesRotation::GetRotation( rotAngle, rotAxis, camera.r.Elements() );
			camera.k.fScale = 1;
			camera.k.k1 = 0;
			camera.k.k2 = 0;
		}

		void SetPoint( __in const Scalar( &pos )[ 3 ], __out Vector3& point )
		{
			point = Vector3( pos );
		}

		void SetProjection( __in Camera* pCameras, __in Vector3* pPoints, __in const uint camIx, __in const uint ptIx, __out Projection& proj )
		{
			proj.cameraIndex = camIx;
			proj.pointIndex = ptIx;
			
			CameraModel3DoF camModel;
			camModel.Initialize( pCameras + camIx );

			Scalar ft[ 2 ] = { 0 };
			DScalar< 6 > residuals[ 2 ];
			camModel.ProjectPoint( ( pPoints + ptIx )->Elements(), ft, residuals );

			proj.projectedPoint[ 0 ] = residuals[ 0 ].GetFx();
			proj.projectedPoint[ 1 ] = residuals[ 1 ].GetFx();
		}


		TEST_METHOD( Initialize )
		{
			Containers::Buffer< CameraModel6DoF< 10 > > camModels;
			camModels.Allocate( dummyBundleZeroError.cameras.Length() );
			for ( size_t i = 0; i < dummyBundleZeroError.cameras.Length(); i++ )
				camModels[ i ].Initialize( &dummyBundleZeroError.cameras[ i ] );

			ProjectionProvider< CameraModel6DoF< 10 > > projectionProvider;
			projectionProvider.Initialize(
				dummyBundleZeroError.cameras.Length(),
				camModels.Data(),
				dummyBundleZeroError.points.Length(),
				dummyBundleZeroError.points.Data(),
				dummyBundleZeroError.projections.Length(),
				dummyBundleZeroError.projections.Data() );

			const size_t camCount = projectionProvider.GetCameraCount();
			Assert::AreEqual( dummyBundleZeroError.cameras.Length(), camCount );
			
			const size_t ptCount = projectionProvider.GetPointCount();
			Assert::AreEqual( dummyBundleZeroError.points.Length(), ptCount );


			Scalar zero[ 2 ] = { 0, 0 };
			for ( size_t i = 0; i < dummyBundleZeroError.projections.Length(); i++ )
			{
				Scalar residuals[ 2 ];

				projectionProvider.GetProjectionBlock< false, false, true >( i, NULL, NULL, residuals );
				AssertAreEqual< Scalar >( zero, residuals, 10e-6f );
			}
				
		}

		TEST_METHOD( _TestDataVerification0 )
		{
			Matrix< double, 3, 3 > R =
			{ { 0.9992772826309979, 2.162559242174209e-002, 3.126093680646133e-002 },
			{ 8.818622544508919e-003, -0.9318492685749851, 0.3627384354499577 },
			{ 3.6974914658811e-002, -0.3622005996801693, -0.9313665128601626 } };

			Vector< double, 3 > t = { -2.418273783787768, -11.77408910880922, -633.8143373879709 };

			const double f = 20598.16150548524;
			Matrix< double, 3, 3 > K = { { f, 0, 0 },
			{ 0, f, 0 },
			{ 0, 0, 1 } };

			Vector< double, 3 > pt = { 1.3001678, -9.5217347, 3.1589597 };

			auto projPt = ( R * pt ) + t;
			projPt *= -1 / projPt[ 2 ];
			projPt *= f;

			Vector< double, 3 > ftCoords = { -40.7, -56.1, 1.0 };

			auto res = projPt - ftCoords;

			Assert::AreEqual( 0.0, res[ 0 ][0], 2.0 );
			Assert::AreEqual( 0.0, res[ 1 ][0], 2.0 );
		}

		TEST_METHOD( InitializeFromFile )
		{
			// Use Snavely's Pinhole camera model: http://www.cs.cornell.edu/~snavely/bundler/bundler-v0.3-manual.html !!

			std::istringstream inputStream( cubesMaskedBundle );
			Bundle bundle;

			Import::BundleImporter importer;
			importer.Import( inputStream, &bundle, NULL );

			Containers::Buffer< CameraModel6DoF< 10 > > cameraModels;
			cameraModels.Allocate( bundle.cameras.Length() );
			for ( size_t i = 0; i < cameraModels.Length(); i++ )
			{
				cameraModels[ i ].Initialize( &bundle.cameras[ i ] );
			}

			ProjectionProvider< CameraModel6DoF< 10 > > projectionProvider;
			projectionProvider.Initialize(
				cameraModels.Length(),
				cameraModels.Data(),
				bundle.points.Length(),
				bundle.points.Data(),
				bundle.projections.Length(),
				bundle.projections.Data() );

			const size_t camCount = projectionProvider.GetCameraCount();
			Assert::AreEqual( bundle.cameras.Length(), camCount );

			const size_t ptCount = projectionProvider.GetPointCount();
			Assert::AreEqual( bundle.points.Length(), ptCount );


			Scalar zero[ 2 ] = { 0, 0 };
			for ( size_t i = 0; i < bundle.projections.Length(); i++ )
			{
				Scalar residuals[ 2 ];

				projectionProvider.GetProjectionBlock< false, false, true >( i, NULL, NULL, residuals );
				AssertAreEqual< Scalar >( zero, residuals, 10e-6f );
			}
		}

	};

}