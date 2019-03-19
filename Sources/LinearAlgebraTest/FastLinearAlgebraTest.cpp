#include "stdafx.h"

namespace LinearAlgebraTest {

	TEST_CLASS( FastVector3Test ) {

		TEST_METHOD( BUILD_BREAK_TEST ) {
			float M[ 9 ];
			float v1[ 3 ];
			float v2[ 3 ];

			V3AddV3( v1, v2, v1 );
			auto d = V3Dot( v1, v2 );
			auto L = V3Length( v1 );
			M33MulV3( M, v1, v2 );
			V3MulC( v1, 10.0f, v2 );
			V3Normalize( v1, v1 );

			Assert::IsTrue( true );
		}

		TEST_METHOD( Addition0 ) {
			float v1[ 3 ] = { 1, 2, 3 };
			float v2[ 3 ] = { 0,0,0 };

			V3AddV3( v1, v2, v1 );

			AssertAreEqual( { 1.0f, 2.0f, 3.0f }, v1 );
		}

		TEST_METHOD( Addition1 ) {
			float v1[ 3 ] = { 1, 2, 3 };
			float v2[ 3 ] = { 5, 8, 12.3f };

			V3AddV3( v1, v2, v1 );

			AssertAreEqual( { 6.0f, 10.0f, 15.3f }, v1, 10e-5f );
		}

		TEST_METHOD( Dot0 ) {
			float v1[ 3 ] = { 1, 2, 3 };
			float v2[ 3 ] = { 0,0,0 };

			float dot = V3Dot( v1, v2 );
			Assert::AreEqual( 0.0f, dot );
		}

		TEST_METHOD( Dot1 ) {
			float v1[ 3 ] = { 1, 2, 3 };
			float v2[ 3 ] = { 2, 4, 7 };

			float dot = V3Dot( v1, v2 );
			Assert::AreEqual( 31.0f, dot );
		}

		TEST_METHOD( Length0 ) {
			float v[ 3 ] = { 0,0,0 };

			float l = V3Length( v );
			Assert::AreEqual( 0.0f, l );
		}

		TEST_METHOD( Length1 ) {
			float v[ 3 ] = { 0 };
			for ( uint i = 0; i < 3; i++ ) {
				v[ i ] = 1.0f;
				Assert::AreEqual( 1.0f, V3Length( v ) );
				v[ i ] = 0.0f;
			}
		}

		TEST_METHOD( MulByConst0 ) {
			float v[ 3 ] = { 0,0,0 };
			V3MulC( v, 0.0f, v );
			AssertAreEqual( { 0.0f,0.0f,0.0f }, v );
		}

		TEST_METHOD( MulByConst1 ) {
			float v[ 3 ] = { 0,0,0 };
			V3MulC( v, 1214.32423f, v );
			AssertAreEqual( { 0.0f,0.0f,0.0f }, v );
		}

		TEST_METHOD( MulByConst2 ) {
			float v[ 3 ] = { 5, 6, 1 };
			V3MulC( v, 3.0f, v );
			AssertAreEqual( { 15.0f, 18.0f, 3.0f }, v );
		}

		TEST_METHOD( Normalize0 ) {
			float v1[ 3 ] = { 0,0,0 };
			float v2[ 3 ] = { 0,0,0 };
			for ( uint i = 0; i < 3; i++ ) {
				v1[ i ] = 1.0f; v2[ i ] = 1.0f;
				V3Normalize( v1, v2 );

				AssertAreEqual( v2, v1 );
				Assert::AreEqual( 1.0f, V3Length( v1 ) );

				v1[ i ] = 0.0f; v2[ i ] = 0.0f;
			}
		}

		TEST_METHOD( Normalize1 ) {
			float v1[ 3 ] = { 34.32f, 23.121f, 1.434f };
			V3Normalize( v1, v1 );

			Assert::AreEqual( 1.0f, V3Length( v1 ), 10e-5f );
		}

		TEST_METHOD( Cross0 ) {
			float v1[ 3 ] = { 1, 0, 0 };
			float v2[ 3 ] = { 0, 1, 0 };
			float v3[ 3 ];

			V3Cross( v1, v2, v3 );
			
			AssertAreEqual( { 0.0f, 0.0f, 1.0f }, v3, 10e-5f );
			Assert::AreEqual( 0.0f, V3Dot( v1, v3 ), 10e-5f );
			Assert::AreEqual( 0.0f, V3Dot( v2, v3 ), 10e-5f );
		}

		TEST_METHOD( Cross1 ) {
			float v1[ 3 ] = { 3.85f, 5.21f, 12.43f };
			float v2[ 3 ] = { 1.213f, 9.23f, 6.764f };
			float v3[ 3 ];

			V3Cross( v1, v2, v3 );

			Assert::AreEqual( 0.0f, V3Dot( v1, v3 ), 10e-5f );
			Assert::AreEqual( 0.0f, V3Dot( v2, v3 ), 10e-5f );
		}

		TEST_METHOD( MultiplyM33_0 ) {
			float M[ 16 ] = {
				1, 0, 0,
				0, 1, 0,
				0, 0, 1 };
			float v[ 4 ] = { 15, 12, 2, 1 };
			float v2[ 4 ];

			M33MulV3( M, v, v2 );

			AssertAreEqual( { 15.0f, 12.0f, 2.0f }, v2, 10e-5f );
		}

		TEST_METHOD( MultiplyM33_1 ) {
			float M[ 9 ] = {
				1, 2, 3,
				4, 5, 6,
				7, 8, 9 };
			float v[ 3 ] = { 3, 5, 2 };
			float v2[ 3 ];

			M33MulV3( M, v, v2 );

			AssertAreEqual( { 19.0f, 49.0f, 79.0f }, v2, 10e-5f );
		}

	};

	TEST_CLASS( FastVector4Test ) {

		TEST_METHOD( BUILD_BREAK_TEST ) {
			float M[ 16 ];
			float v1[ 4 ];
			float v2[ 4 ];

			V4AddV4( v1, v2, v1 );
			auto d = V4Dot( v1, v2 );
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

			float dot = V4Dot( v1, v2 );
			Assert::AreEqual( 0.0f, dot );
		}

		TEST_METHOD( Dot1 ) {
			float v1[ 4 ] = { 1, 2, 3, 4 };
			float v2[ 4 ] = { 2, 4, 7, 13 };

			float dot = V4Dot( v1, v2 );
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

	TEST_CLASS( FastMatrixMultiplicationTest )
	{
		TEST_METHOD( Correctness0 ) {
			float M[ 9 ] = {
				2, 4, 5,
				1, 6, 9,
				5, 7, 8
			};

			float v[ 3 ] = { 1, 2, 3 };

			float v1[ 3 ];
			float v2[ 3 ];

			MatrixMultiply<float, 3, 3, 1>( M, v, v1 );

			M33MulV3( M, v, v2 );

			AssertAreEqual( v2, v1, 10e-5f );
		}

		TEST_METHOD( Correctness1 ) {
			double M1[ 16 ] = {
				0.32, 1.54, 0.12, 0.55,
				0.03, 0.12, 0.64, 1.69,
				0.85, 0.44, 2.65, 0.35,
				0.60, 0.06, 1.33, 0.54,
			};

			double M2[ 16 ] = {
				0.65, 0.23, 1.54, 0.99,
				0.45, 0.56, 4.63, 6.32,
				0.24, 1.54, 0.65, 0.32,
				0.95, 0.64, 0.81, 1.47,
			};

			double M3[ 16 ];
			double M4[ 16 ];

			M44MulM44( M1, M2, M3 );
			MatrixMultiply<double, 4, 4, 4>( M1, M2, M4 );

			AssertAreEqual( M3, M4, 10e-10 );
		}

		TEST_METHOD( Correctness2 ) {
			double M0[ 3 ] = { 1, 2, 3 };
			double M1[ 2 ] = { 4, 5 };

			double M2[ 6 ];
			MatrixMultiply< double, 3, 1, 2 >( M0, M1, M2 );

			double expected[ 6 ] = {
				4, 5,
				8, 10,
				12, 15
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

		TEST_METHOD( AtBCorrectness0 ) {
			double M0[ 2 ] = { 4, 5 };
			double M1[ 3 ] = { 1, 2, 3 };

			double M2[ 6 ];
			MatrixMultiplyAtB< double, 1, 2, 3 >( M0, M1, M2 );

			double expected[ 6 ] = {
				4, 8, 12,
				5, 10, 15
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

		TEST_METHOD( AtBCorrectness1 ) {
			double M0[ 9 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			double M1[ 9 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			double M2[ 9 ];
			MatrixMultiplyAtB< double, 3, 3, 3 >( M0, M1, M2 );

			double expected[ 9 ] = {
				66, 78, 90,
				78, 93, 108,
				90, 108, 126
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

		TEST_METHOD( ABtCorrectness0 ) {
			double M0[ 2 ] = { 4, 5 };
			double M1[ 3 ] = { 1, 2, 3 };

			double M2[ 6 ];
			MatrixMultiplyABt< double, 2, 1, 3 >( M0, M1, M2 );

			double expected[ 6 ] = {
				4, 8, 12,
				5, 10, 15
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

		TEST_METHOD( ABtCorrectness1 ) {
			double M0[ 9 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			double M1[ 9 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			double M2[ 9 ];
			MatrixMultiplyABt< double, 3, 3, 3 >( M0, M1, M2 );

			double expected[ 9 ] = {
				14, 32, 50,
				32, 77, 122,
				50, 122, 194
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

		TEST_METHOD( AtBtCorrectness0 ) {
			double M0[ 2 ] = { 4, 5 };
			double M1[ 3 ] = { 1, 2, 3 };

			double M2[ 6 ];
			MatrixMultiplyAtBt< double, 1, 2, 3 >( M0, M1, M2 );

			double expected[ 6 ] = {
				4, 8, 12,
				5, 10, 15
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

		TEST_METHOD( AtBtCorrectness1 ) {
			double M0[ 9 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			double M1[ 9 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			double M2[ 9 ];
			MatrixMultiplyAtBt< double, 3, 3, 3 >( M0, M1, M2 );

			double expected[ 9 ] = {
				30, 66, 102,
				36, 81, 126,
				42, 96, 150
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

		TEST_METHOD( CorrectnesRand0 )
		{
			srand( (uint)time( NULL ) );

			const uint BATCH_SIZE = 100;

			Matrix< double, 4, 5 > M0;
			Matrix< double, 5, 2 > M1;

			for ( uint i = 0; i < BATCH_SIZE; i++ )
			{
				Random< double >::Fill( 20, M0.Elements() );
				Random< double >::Fill( 10, M1.Elements() );

				auto expected = M0 * M1;

				double M2[ 8 ];

				MatrixMultiply< double, 4, 5, 2 >( M0.Elements(), M1.Elements(), M2 );

				AssertAreEqual( 8, expected.Elements(), M2, 10e-10 );

			}
		}

		TEST_METHOD( AtBCorrectnesRand0 )
		{
			srand( (uint)time( NULL ) );

			const uint BATCH_SIZE = 100;

			Matrix< double, 5, 4 > M0;
			Matrix< double, 5, 2 > M1;

			for ( uint i = 0; i < BATCH_SIZE; i++ )
			{
				Random<double>::Fill( 20, M0.Elements() );
				Random<double>::Fill( 10, M1.Elements() );

				auto expected = M0.Transpose() * M1;

				double M2[ 8 ];

				MatrixMultiplyAtB< double, 5, 4, 2 >( M0.Elements(), M1.Elements(), M2 );

				AssertAreEqual( 8, expected.Elements(), M2, 10e-10 );

			}
		}

		TEST_METHOD( ABtCorrectnesRand0 )
		{
			srand( (uint)time( NULL ) );

			const uint BATCH_SIZE = 100;

			Matrix< double, 4, 5 > M0;
			Matrix< double, 2, 5 > M1;

			for ( uint i = 0; i < BATCH_SIZE; i++ )
			{
				Random<double>::Fill( 20, M0.Elements() );
				Random<double>::Fill( 10, M1.Elements() );

				auto expected = M0 * M1.Transpose();

				double M2[ 8 ];

				MatrixMultiplyABt< double, 4, 5, 2 >( M0.Elements(), M1.Elements(), M2 );

				AssertAreEqual( 8, expected.Elements(), M2, 10e-10 );

			}
		}

		TEST_METHOD( AtBtCorrectnesRand0 )
		{
			srand( (uint)time( NULL ) );

			const uint BATCH_SIZE = 100;

			Matrix< double, 5, 4 > M0;
			Matrix< double, 2, 5 > M1;

			for ( uint i = 0; i < BATCH_SIZE; i++ )
			{
				Random<double>::Fill( 20, M0.Elements() );
				Random<double>::Fill( 10, M1.Elements() );

				auto expected = M0.Transpose() * M1.Transpose();

				double M2[ 8 ];

				MatrixMultiplyAtBt< double, 5, 4, 2 >( M0.Elements(), M1.Elements(), M2 );

				AssertAreEqual( 8, expected.Elements(), M2, 10e-10 );

			}
		}
	};

	TEST_CLASS( FastMatrixAddtitonTest ) {

		TEST_METHOD( MatrixAddCorrectness0 )
		{
			double a = 0.21;
			double b = 0.85;

			double res = 0;
			MatrixAdd< double, 1, 1 >( &a, &b, &res );

			Assert::AreEqual( 1.06, res );
		}

		TEST_METHOD( MatrixAddCorrectness1 )
		{
			double M1[ 4 ] = { 0.5, 0.1, 0.22, 0.68 };
			double M2[ 4 ] = { 0.3, 0.25, 0.2, 1.54 };

			double res[ 4 ];
			MatrixAdd< double, 2, 2 >( M1, M2, res );

			AssertAreEqual( { 0.8, 0.35, 0.42, 2.22 }, res, 10e-10 );
		}

		TEST_METHOD( MatrixAddCorrectness2 )
		{
			Matrix< double, 3, 3 > M1( { 1, 2, 3, 4, 5, 6, 7, 8, 9 } );
			Matrix< double, 3, 3 > M2( { 10, 20, 30, 40, 50, 60, 70, 80, 90 } );

			double res[ 9 ];
			MatrixAdd< double, 3, 3 >( M1.Elements(), M2.Elements(), res );

			Matrix< double, 3, 3 > expected = M1 + M2;
			AssertAreEqual( 9, expected.Elements(), res );
		}

		TEST_METHOD( MatrixInvertCorrectness0 )
		{
			Matrix< double, 3, 3 > M( { 1, 2, 3, 0, 1, 4, 5, 6, 0 } );
			
			Matrix< double, 3, 3 > M1 = M;
			Matrix< double, 3, 3 > IM;

			MatrixInvertGJ< double, 3 >( M1.Elements(), IM.Elements() );

			double identity[ 9 ];
			M33Identity( identity );

			double expectedMMulInv[ 9 ];
			M33MulM33( M.Elements(), IM.Elements(), expectedMMulInv );

			AssertAreEqual( identity, expectedMMulInv, 10e-10 );
		}

	};

}