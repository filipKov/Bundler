#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( CameraModel6DoFTest ) {

		class CameraModelMockup : public CameraModel6DoF< 10 > {
		public:
			const Camera* GetCamera() {
				return m_pCamera;
			}
			
			const int GetRotationCount() {
				return _countof( m_incrementalRotations );
			}

			const uint GetCurrentRotationCount() {
				return m_currentRotationCount;
			}

			const DMatrix3x3< totalParamCount >& GetRotation( __in const uint index ) {
				return m_incrementalRotations[ index ];
			}
		};


		TEST_METHOD( TotalParameterCount ) {
			CameraModel6DoF< 25 > camModel;
			Assert::AreEqual( 10u, camModel.totalParamCount );
		}

		TEST_METHOD( RotationArraySize ) {
			CameraModelMockup camModel;
			Assert::AreEqual( 10, camModel.GetRotationCount() );
		}

		TEST_METHOD( Initialize0 ) {
			Camera camera;
			
			CameraModelMockup camModel;
			camModel.Initialize( &camera );

			Assert::AreEqual( (void*)&camera, (void*)camModel.GetCamera() );
			
			Assert::AreEqual( 1u, camModel.GetCurrentRotationCount() );
		}

		TEST_METHOD( Initialize1 ) {
			Camera camera;
			camera.r = Matrix3x3( { 0, -1, 0, 1, 0, 0, 0, 0, 1 } );

			CameraModelMockup camModel;
			camModel.Initialize( &camera );

			Assert::AreEqual( (void*)&camera, (void*)camModel.GetCamera() );

			Assert::AreEqual( 1u, camModel.GetCurrentRotationCount() );

			auto rot = camModel.GetRotation( 0 );
			for ( uint i = 0; i < 9; i++ ) {
				Assert::AreEqual( camera.r.Elements()[ i ], rot.Elements()[ i ].GetFx(), 10e-5f );
			}
		}

		TEST_METHOD( Update0 ) {
			Camera camera;
			camera.t = Vector3( { 1, 2, 3 } );
			camera.r = Matrix3x3( { 0, -1, 0, 1, 0, 0, 0, 0, 1 } );

			CameraModelMockup camModel;
			camModel.Initialize( &camera );

			Scalar updateParams[ 7 ] = { 0.436f, 0.0f, 1.0f, 0.0f, 4.3f, 2.1f, 5.8f };
			camModel.UpdateCamera( updateParams );

			AssertAreEqual( { 5.3f, 4.1f, 8.8f }, camera.t.Elements(), 10e-5f );

			Assert::AreEqual( 2u, camModel.GetCurrentRotationCount() );
		}

		TEST_METHOD( Project0 ) {
			Camera camera;
			camera.t = Vector3( { 0,0,0 } );
			camera.r = Matrix3x3( { 0, -1, 0, 1, 0, 0, 0, 0, 1 } );
			camera.focalLength = 1;

			CameraModel6DoF< 128 > camModel;
			camModel.Initialize( &camera );

			Scalar point[ 3 ] = { 5, 2, 1 };
			Scalar feature[ 2 ] = { 1.78f, 4.5f };
			DScalar< 10 > residual[ 2 ];
			camModel.ProjectPoint( point, feature, residual );

			Assert::AreEqual( -3.78f, residual[ 0 ].GetFx(), 10e-5f );
			Assert::AreEqual( 0.5f, residual[ 1 ].GetFx(), 10e-5f );

		}

	};

}