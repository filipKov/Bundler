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

		TEST_METHOD( GetAxis1 ) {
			Scalar M1[ 9 ] = { 
				0.5365f, -0.6499f, 0.5383f,
				0.7635f, 0.6456f, 0.0186f,
				-0.3596f, 0.4010f, 0.8425f,
			};

			Scalar angle;
			Scalar axis[ 3 ];
			RodriguesRotation::GetFromRotationMatrix( M1, &angle, axis );

			V3MulC( axis, angle, axis );

			AssertAreEqual( { 0.23f, 0.54f, 0.85f }, axis, 10e-3f );
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

		TEST_METHOD( GetRotation1 ) {
			Scalar angle = 1.0186f;
			Scalar axis[ 3 ] = { 0.5242f, 0.2690f, 0.8080f };

			Scalar M[ 9 ];
			RodriguesRotation::GetRotation( angle, axis, M );

			Scalar expected[ 9 ] = {
				0.6552f, -0.6208f, 0.4304f,
				0.7549f, 0.5590f, -0.3430f,
				-0.0276f, 0.5497f, 0.8349f
			};

			AssertAreEqual( expected, M, 10e-4f );
		}

		TEST_METHOD( InverseTransform0 ) {
			Scalar M1[ 9 ] = {
				0.7150f, -0.5592f, 0.4196f,
				0.6984f, 0.5438f, -0.4654f,
				0.0321f, 0.6258f, 0.7793f
			};

			Scalar angle;
			Scalar axis[ 3 ];
			RodriguesRotation::GetFromRotationMatrix( M1, &angle, axis );

			Scalar M2[ 9 ];
			RodriguesRotation::GetRotation( angle, axis, M2 );

			AssertAreEqual( M1, M2, 10e-4f );
		}

	};

}
