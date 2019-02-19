#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( RodriguesRotationTest ) {

		TEST_METHOD( ParamCount ) {
			Assert::AreEqual( 4u, RodriguesRotation::rotationParameterCount );
		}

		TEST_METHOD( GetAxis0 ) {
			Scalar M1[ 9 ] = { 0, -1, 0, 1, 0, 0, 0, 0, 1 };
			
			Scalar angle;
			Scalar axis[ 3 ];
			RodriguesRotation::GetFromRotationMatrix( M1, &angle, axis );

			AssertAreEqual( { 0.0f, 0.0f, 1.0f }, axis, 10e-5f );
			Assert::AreEqual( 1.5708f, angle, 10e-4f );
		}

		TEST_METHOD( GetRotation0 ) {
			Scalar angle = 1.57079632679f;
			Scalar axis[ 3 ] = { 0,0,1 };

			DScalar<4> M[ 9 ];
			RodriguesRotation::GetRotation<4, 0, false>( angle, axis, M );

			DScalar<4> expected[ 9 ] = {
				DScalar<4>( 0 ), DScalar<4>( -1 ), DScalar<4>( 0 ),
				DScalar<4>( 1 ), DScalar<4>( 0 ), DScalar<4>( 0 ),
				DScalar<4>( 0 ), DScalar<4>( 0 ), DScalar<4>( 1 ),
			};

			for ( uint i = 0; i < 9; i++ ) {
				Assert::AreEqual( expected[ i ].GetFx(), M[ i ].GetFx(), 10e-5f );
			}
		}

	};

}
