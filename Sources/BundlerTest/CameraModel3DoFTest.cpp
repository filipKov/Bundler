#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( CameraModel3DoFTest ) {

		TEST_METHOD( TestParamsUpdate_0 ) {

			Camera cam;
			cam.t[ 0 ] = 12.0f;
			cam.t[ 1 ] = 3.14f;

			CameraModel3DoF_Translation camModel;
			camModel.Initialize( &cam );

			Scalar deltas[ 3 ] = { 1, 5, 9 };

			camModel.UpdateCamera( deltas );

			Scalar expected[ 3 ] = { 13.0f, 8.14f, 9.0f };
			AssertAreEqual( 3, expected, camModel.t.Elements() );
		}

		TEST_METHOD( TestProjectPoint_0 ) {
			Camera cam;
			cam.t = Vector3( { 2, 3, 4 } );
			cam.r = Matrix3x3( { 1,0,0, 0,1,0, 0,0,1 } );
			cam.focalLength = 1;

			CameraModel3DoF_Translation camModel;
			camModel.Initialize( &cam );

			Scalar pt[ 3 ] = { 2, 4, 6 };
			Scalar feature[2] = { 0.4f, 0.7f };
			DScalar< 6 > residuals[ 2 ];

			camModel.ProjectPoint( pt, feature, residuals );

			Assert::AreEqual( Scalar( 0.0 ), residuals[ 0 ].GetFx() );
			Assert::AreEqual( Scalar( 0.0 ), residuals[ 1 ].GetFx() );

		}

	};

}
