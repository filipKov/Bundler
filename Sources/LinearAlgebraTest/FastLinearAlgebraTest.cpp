#include "stdafx.h"

namespace LinearAlgebraTest {

	TEST_CLASS( FastVector4Test ) {

		TEST_METHOD( BUILD_BREAK_TEST ) {
			float M[ 16 ];
			float v1[ 4 ];
			float v2[ 4 ];

			V4AddV4( v1, v2, v1 );
			auto d = V4DotV4( v1, v2 );
			auto L = V4Length( v1 );
			M34MulV4( M, v1, v2 );
			M44MulV4( M, v1, v2 );
			V4MulC( v1, 10.0f, v2 );
			V4Normalize( v1, v1 );

			Assert::IsTrue( true );
		}

		TEST_METHOD( Addition0 ) {
			float v1[ 4 ] = { 1, 2, 3, 4 };
			float v2[ 4 ] = { 0,0,0 };

			V4AddV4( v1, v2, v1 );

			AssertAreEqual( { 1.0f, 2.0f, 3.0f, 4.0f }, v1 );
		}

		TEST_METHOD( Addition1 ) {
			float v1[ 4 ] = { 1, 2, 3, 4 };
			float v2[ 4 ] = { 5, 8, 12.3f, 56.12f };

			V4AddV4( v1, v2, v1 );

			AssertAreEqual( { 6.0f, 10.0f, 15.3f, 60.12f }, v1, 10e-5f );
		}

		TEST_METHOD( Dot0 ) {
			float v1[ 4 ] = { 1, 2, 3, 4 };
			float v2[ 4 ] = { 0,0,0,0 };

			float dot = V4DotV4( v1, v2 );
			Assert::AreEqual( 0.0f, dot );
		}

		TEST_METHOD( Dot1 ) {
			float v1[ 4 ] = { 1, 2, 3, 4 };
			float v2[ 4 ] = { 2, 4, 7, 13 };

			float dot = V4DotV4( v1, v2 );
			Assert::AreEqual( 83.0f, dot );
		}

		TEST_METHOD( Length0 ) {
			float v[ 4 ] = { 0,0,0,0 };

			float l = V4Length( v );
			Assert::AreEqual( 0.0f, l );
		}

		TEST_METHOD( Length1 ) {
			float v[ 4 ] = { 0 };
			for ( uint i = 0; i < 4; i++ ) {
				v[ i ] = 1.0f;
				Assert::AreEqual( 1.0f, V4Length( v ) );
				v[ i ] = 0.0f;
			}
		}

		TEST_METHOD( MulByConst0 ) {
			float v[ 4 ] = { 0,0,0,0 };
			V4MulC( v, 0.0f, v );
			AssertAreEqual( { 0.0f,0.0f,0.0f,0.0f }, v );
		}

		TEST_METHOD( MulByConst1 ) {
			float v[ 4 ] = { 0,0,0,0 };
			V4MulC( v, 1214.32423f, v );
			AssertAreEqual( { 0.0f,0.0f,0.0f,0.0f }, v );
		}

		TEST_METHOD( MulByConst2 ) {
			float v[ 4 ] = { 5, 6, 1, 10 };
			V4MulC( v, 3.0f, v );
			AssertAreEqual( { 15.0f, 18.0f, 3.0f, 30.0f }, v );
		}

		TEST_METHOD( Normalize0 ) {
			float v1[ 4 ] = { 0,0,0,0 };
			float v2[ 4 ] = { 0,0,0,0 };
			for ( uint i = 0; i < 4; i++ ) {
				v1[ i ] = 1.0f; v2[ i ] = 1.0f;
				V4Normalize( v1, v2 );

				AssertAreEqual( v2, v1 );
				Assert::AreEqual( 1.0f, V4Length( v1 ) );

				v1[ i ] = 0.0f; v2[ i ] = 0.0f;
			}
		}

		TEST_METHOD( Normalize1 ) {
			float v1[ 4 ] = { 34.32f, 23.121f, 1.434f, 212.1157f };
			V4Normalize( v1, v1 );

			Assert::AreEqual( 1.0f, V4Length( v1 ), 10e-5f );
		}

		TEST_METHOD( MultiplyM34_0 ) {
			float M[ 16 ] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			float v[ 4 ] = { 15, 12, 2, 1 };
			float v2[ 3 ];

			M34MulV4( M, v, v2 );

			AssertAreEqual( { 15.0f, 12.0f, 2.0f }, v2, 10e-5f );
		}

		TEST_METHOD( MultiplyM34_1 ) {
			float M[ 16 ] = { 
				1, 2, 3, 4, 
				5, 6, 7, 8, 
				9, 10, 11, 12, 
				13, 14, 15, 16 };
			float v[ 4 ] = { 3, 5, 2, 1 };
			float v2[ 3 ];

			M34MulV4( M, v, v2 );

			AssertAreEqual( { 23.0f, 67.0f, 111.0f }, v2, 10e-5f );
		}

		TEST_METHOD( MultiplyM44_0 ) {
			float M[ 16 ] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			float v[ 4 ] = { 15, 12, 2, 1 };
			float v2[ 4 ];

			M44MulV4( M, v, v2 );

			AssertAreEqual( { 15.0f, 12.0f, 2.0f, 1.0f }, v2, 10e-5f );
		}

		TEST_METHOD( MultiplyM44_1 ) {
			float M[ 16 ] = {
				1, 2, 3, 4,
				5, 6, 7, 8,
				9, 10, 11, 12,
				13, 14, 15, 16 };
			float v[ 4 ] = { 3, 5, 2, 1 };
			float v2[ 4 ];

			M44MulV4( M, v, v2 );

			AssertAreEqual( { 23.0f, 67.0f, 111.0f, 155.0f }, v2, 10e-5f );
		}

	};

}