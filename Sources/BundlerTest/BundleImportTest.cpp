#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include "TestData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( BundleImportTest )
	{
		TEST_METHOD( SnavelyBundleImport0 )
		{
			std::istringstream inputStream( parseTestBundle );
			Bundle bundle;
			BundleAdditionalPayload metadata;

			HRESULT hr = Import::BundleImporter::Import( &inputStream, &bundle, &metadata );
			
			Assert::AreEqual( S_OK, hr );

			Assert::AreEqual( size_t( 2 ), bundle.cameras.Length() );
			Assert::AreEqual( size_t( 3 ), bundle.points.Length() );
			Assert::AreEqual( size_t( 6 ), bundle.projections.Length() );

			{
				Camera& cam0 = bundle.cameras[ 0 ];
				CameraInfo& cam0Info = metadata.cameraInformation[ 0 ];

				Assert::AreEqual( Scalar( 12.453 ), cam0.focalLength );
				AssertAreEqual(
				{ Scalar( 1 ), Scalar( 3 ), Scalar( -2 ),
				  Scalar( -4 ), Scalar( -6 ), Scalar( 5 ),
				  Scalar( -7 ), Scalar( -9 ), Scalar( 8 ) },
					cam0.r.Elements()
				);

				AssertAreEqual( { Scalar( 10 ), Scalar( -11 ), Scalar( -12 ) }, cam0.t.Elements() );

				Assert::AreEqual( cam0Info.width, 1u );
				Assert::AreEqual( cam0Info.height, 1u );
			}

			{
				Camera& cam1 = bundle.cameras[ 1 ];
				CameraInfo& cam1Info = metadata.cameraInformation[ 1 ];

				Assert::AreEqual( Scalar( 18.543 ), cam1.focalLength );
				AssertAreEqual(
				{ Scalar( 12 ), Scalar( 10 ), Scalar( -11 ),
					Scalar( -9 ), Scalar( -7 ), Scalar( 8 ),
					Scalar( -6 ), Scalar( -4 ), Scalar( 5 ) },
					cam1.r.Elements()
				);

				AssertAreEqual( { Scalar( 3 ), Scalar( -2 ), Scalar( -1 ) }, cam1.t.Elements() );

				Assert::AreEqual( cam1Info.width, 1u );
				Assert::AreEqual( cam1Info.height, 1u );
			}

			{
				Vector3& pt = bundle.points[ 0 ];
				AssertAreEqual( { Scalar( 1.25 ), Scalar( 4.11 ), Scalar( -3.56 ) }, pt.Elements() );

				Assert::AreEqual( 0u, bundle.projections[ 0 ].cameraIndex );
				Assert::AreEqual( 1u, bundle.projections[ 1 ].cameraIndex );

				Assert::AreEqual( 0u, bundle.projections[ 0 ].pointIndex );
				Assert::AreEqual( 0u, bundle.projections[ 1 ].pointIndex );

				AssertAreEqual( { Scalar( 0.54 ), Scalar( -0.14 ) }, bundle.projections[ 0 ].projectedPoint.Elements() );
				AssertAreEqual( { Scalar( 0.75 ), Scalar( -0.34 ) }, bundle.projections[ 1 ].projectedPoint.Elements() );
			}

			{
				Vector3& pt = bundle.points[ 1 ];
				AssertAreEqual( { Scalar( 1.25 ), Scalar( 4.43 ), Scalar( -4.01 ) }, pt.Elements() );

				Assert::AreEqual( 0u, bundle.projections[ 2 ].cameraIndex );
				Assert::AreEqual( 1u, bundle.projections[ 3 ].cameraIndex );

				Assert::AreEqual( 1u, bundle.projections[ 2 ].pointIndex );
				Assert::AreEqual( 1u, bundle.projections[ 3 ].pointIndex );

				AssertAreEqual( { Scalar( 0.85 ), Scalar( -0.3 ) }, bundle.projections[ 2 ].projectedPoint.Elements() );
				AssertAreEqual( { Scalar( 0.23 ), Scalar( -0.4 ) }, bundle.projections[ 3 ].projectedPoint.Elements() );
			}

			{
				Vector3& pt = bundle.points[ 2 ];
				AssertAreEqual( { Scalar( 3.25 ), Scalar( 0.11 ), Scalar( -3 ) }, pt.Elements() );

				Assert::AreEqual( 0u, bundle.projections[ 4 ].cameraIndex );
				Assert::AreEqual( 1u, bundle.projections[ 5 ].cameraIndex );

				Assert::AreEqual( 2u, bundle.projections[ 4 ].pointIndex );
				Assert::AreEqual( 2u, bundle.projections[ 5 ].pointIndex );

				AssertAreEqual( { Scalar( 0.34 ), Scalar( -0.56 ) }, bundle.projections[ 4 ].projectedPoint.Elements() );
				AssertAreEqual( { Scalar( 0.81 ), Scalar( -0.25 ) }, bundle.projections[ 5 ].projectedPoint.Elements() );
			}
		}

		TEST_METHOD( SnavelyBundleImport1 )
		{
			std::istringstream inputStream( parseTestBundle );
			std::istringstream imListStream( parseTestImageList );
			Bundle bundle;
			BundleAdditionalPayload metadata;

			HRESULT hr = Import::BundleImporter::Import( &inputStream, &imListStream, &bundle, &metadata );

			Assert::AreEqual( S_OK, hr );

			Assert::AreEqual( size_t( 2 ), bundle.cameras.Length() );
			Assert::AreEqual( size_t( 3 ), bundle.points.Length() );
			Assert::AreEqual( size_t( 6 ), bundle.projections.Length() );

			{
				Camera& cam0 = bundle.cameras[ 0 ];
				CameraInfo& cam0Info = metadata.cameraInformation[ 0 ];

				Assert::AreEqual( Scalar( 12.453 ) / Scalar( 16 ), cam0.focalLength );
				AssertAreEqual(
				{ Scalar( 1 ), Scalar( 3 ), Scalar( -2 ),
					Scalar( -4 ), Scalar( -6 ), Scalar( 5 ),
					Scalar( -7 ), Scalar( -9 ), Scalar( 8 ) },
					cam0.r.Elements()
				);

				AssertAreEqual( { Scalar( 10 ), Scalar( -11 ), Scalar( -12 ) }, cam0.t.Elements() );

				Assert::AreEqual( cam0Info.width, 16u );
				Assert::AreEqual( cam0Info.height, 12u );
			}

			{
				Camera& cam1 = bundle.cameras[ 1 ];
				CameraInfo& cam1Info = metadata.cameraInformation[ 1 ];

				Assert::AreEqual( Scalar( 18.543 ) / Scalar( 16 ), cam1.focalLength );
				AssertAreEqual(
				{ Scalar( 12 ), Scalar( 10 ), Scalar( -11 ),
					Scalar( -9 ), Scalar( -7 ), Scalar( 8 ),
					Scalar( -6 ), Scalar( -4 ), Scalar( 5 ) },
					cam1.r.Elements()
				);

				AssertAreEqual( { Scalar( 3 ), Scalar( -2 ), Scalar( -1 ) }, cam1.t.Elements() );

				Assert::AreEqual( cam1Info.width, 16u );
				Assert::AreEqual( cam1Info.height, 12u );
			}

			{
				Vector3& pt = bundle.points[ 0 ];
				AssertAreEqual( { Scalar( 1.25 ), Scalar( 4.11 ), Scalar( -3.56 ) }, pt.Elements() );

				Assert::AreEqual( 0u, bundle.projections[ 0 ].cameraIndex );
				Assert::AreEqual( 1u, bundle.projections[ 1 ].cameraIndex );

				Assert::AreEqual( 0u, bundle.projections[ 0 ].pointIndex );
				Assert::AreEqual( 0u, bundle.projections[ 1 ].pointIndex );

				AssertAreEqual( { Scalar( 0.54 ) / Scalar( 16 ), Scalar( -0.14 ) / Scalar( 16 ) }, bundle.projections[ 0 ].projectedPoint.Elements() );
				AssertAreEqual( { Scalar( 0.75 ) / Scalar( 16 ), Scalar( -0.34 ) / Scalar( 16 ) }, bundle.projections[ 1 ].projectedPoint.Elements() );
			}

			{
				Vector3& pt = bundle.points[ 1 ];
				AssertAreEqual( { Scalar( 1.25 ), Scalar( 4.43 ), Scalar( -4.01 ) }, pt.Elements() );

				Assert::AreEqual( 0u, bundle.projections[ 2 ].cameraIndex );
				Assert::AreEqual( 1u, bundle.projections[ 3 ].cameraIndex );

				Assert::AreEqual( 1u, bundle.projections[ 2 ].pointIndex );
				Assert::AreEqual( 1u, bundle.projections[ 3 ].pointIndex );

				AssertAreEqual( { Scalar( 0.85 ) / Scalar( 16 ), Scalar( -0.3 ) / Scalar( 16 ) }, bundle.projections[ 2 ].projectedPoint.Elements() );
				AssertAreEqual( { Scalar( 0.23 ) / Scalar( 16 ), Scalar( -0.4 ) / Scalar( 16 ) }, bundle.projections[ 3 ].projectedPoint.Elements() );
			}

			{
				Vector3& pt = bundle.points[ 2 ];
				AssertAreEqual( { Scalar( 3.25 ), Scalar( 0.11 ), Scalar( -3 ) }, pt.Elements() );

				Assert::AreEqual( 0u, bundle.projections[ 4 ].cameraIndex );
				Assert::AreEqual( 1u, bundle.projections[ 5 ].cameraIndex );

				Assert::AreEqual( 2u, bundle.projections[ 4 ].pointIndex );
				Assert::AreEqual( 2u, bundle.projections[ 5 ].pointIndex );

				AssertAreEqual( { Scalar( 0.34 ) / Scalar( 16 ), Scalar( -0.56 ) / Scalar( 16 ) }, bundle.projections[ 4 ].projectedPoint.Elements() );
				AssertAreEqual( { Scalar( 0.81 ) / Scalar( 16 ), Scalar( -0.25 ) / Scalar( 16 ) }, bundle.projections[ 5 ].projectedPoint.Elements() );
			}
		}

		TEST_METHOD( _ImportFlipResearch )
		{
			Matrix< double, 3, 3 > R =
			{ { 0.9992772826309979, -2.162559242174209e-002, 3.126093680646133e-002 },
			{ -8.818622544508919e-003, -0.9318492685749851, -0.3627384354499577 },
			{ -3.6974914658811e-002, -0.3622005996801693, 0.9313665128601626 } };

			Matrix< double, 3, 3 > R2;
			R2[ 0 ][ 0 ] = R[ 0 ][ 0 ]; R2[ 0 ][ 1 ] = R[ 0 ][ 2 ]; R2[ 0 ][ 2 ] = R[ 0 ][ 1 ];
			R2[ 1 ][ 0 ] = R[ 1 ][ 0 ]; R2[ 1 ][ 1 ] = R[ 1 ][ 2 ]; R2[ 1 ][ 2 ] = R[ 1 ][ 1 ];
			R2[ 2 ][ 0 ] = R[ 2 ][ 0 ]; R2[ 2 ][ 1 ] = R[ 2 ][ 2 ]; R2[ 2 ][ 2 ] = R[ 2 ][ 1 ];


			Vector< double, 3 > t = { -2.418273783787768, 11.77408910880922, 633.8143373879709 };

			const double f = 20598.16150548524;
			Matrix< double, 3, 3 > K = { { f, 0, 0 },
			{ 0, f, 0 },
			{ 0, 0, 1 } };

			Vector< double, 3 > pt = { 1.3001678, 9.5217347, 3.1589597};
			
			Vector< double, 3 > pt2 = { 1.3001678, 3.1589597, 9.5217347 };

			auto projPt = ( R2 * pt2 ) + t;
			projPt[ 0 ] *= f;
			projPt[ 1 ] *= f;

			projPt *= 1 / projPt[ 2 ];

			Vector< double, 3 > ftCoords = { -40.7, 56.1, 1.0 };

			auto res = projPt - ftCoords;

			Assert::AreEqual( 0.0, res[ 0 ][ 0 ], 2.0 );
			Assert::AreEqual( 0.0, res[ 1 ][ 0 ], 2.0 );
		}
	};

}