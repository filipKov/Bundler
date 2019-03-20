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

		template < typename T, size_t n >
		void TestInverse( __in const T( &M )[n * n] )
		{
			T M1[n*n];
			ShallowCopy<T>( M, n*n, M1 );

			T MInv[n*n];
			MatrixInvertGJ< T, n >( M1, MInv );

			MatrixMultiply< T, n, n, n >( M, MInv, M1 );
			T identity[n * n];
			MatrixIdentity< T, n >( identity );

			AssertAreEqual( identity, M1, T( 10e-10 ) );
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

			// AssertAreEqual( identity, M1.Elements( ), 10e-10 );
			AssertAreEqual( identity, expectedMMulInv, 10e-10 );
		}

		TEST_METHOD( MatrixInvertCorrectness1 )
		{
			double M[9] = { 1, 0, 0, -5, 1, 0, 0, 0, 1 };
			
			TestInverse< double, 3 >( M );
		}

		TEST_METHOD( MatrixInvertCorrectness2 )
		{
			double Rs[10][9] = {
				{ 0.9992772826309979, 2.162559242174209e-002, 3.126093680646133e-002,
				8.818622544508919e-003, -0.9318492685749851, 0.3627384354499577,
				3.6974914658811e-002, -0.3622005996801693, -0.9313665128601626 },

				{ 0.9985183271100719, -9.539353230636045e-003, 5.357379177587521e-002,
				-4.662157092442542e-002, -0.6577063404285033, 0.7518302992595369,
				2.806384773765799e-002, -0.7532140270202667, -0.6571765744075717 },

				{ 0.6911197278838587, 7.602013747312243e-002, 0.7187311461376283,
				-0.4893847357414589, -0.6825640689906438, 0.5427788427584463,
				0.5318421778620079, -0.7268612181300521, -0.4345303987357996 },

				{ 0.9323145243327324, 2.553064308068534e-002, -0.3607461905302916,
				0.2825921204108305, -0.6739029169345524, 0.6826394011693235,
				-0.2256796871673245, -0.7383786595040437, -0.6355042360119011 },

				{ 0.9865912235755037, -2.636849948902404e-002, -0.1610666315488342,
				8.567105332873502e-002, -0.7563075805299301, 0.6485825423602677,
				-0.1389180628476211, -0.6536845920378273, -0.7439074041485295 },

				{ 0.940419083939694, -8.336377630598748e-003, 0.3399153591263963,
				-0.2068876064360509,-0.8073739525624313, 0.552580147152319,
				0.2698322902567233 ,-0.5899811908287559 ,-0.7609945660798734 },

				{ 0.644167886742871 ,-0 ,0.7648841308912244,
				-0.4775565781683741 ,-0.7811434988038593 ,0.4021872063163856,
				0.5974842661839201 ,-0.6243515310115491 ,-0.5031875568674143 },

				{ 0.9989144599305349 ,2.913179826467167e-002 ,-3.634886616600846e-002,
				4.09951899652015e-002 ,-0.9203207200150964 ,0.389010496915715,
				-2.212003936264499e-002 ,-0.3900783391073657 ,-0.9205159386006492 },

				{ 0.950802524080175, -0, 0.3097976116802196,
				-0.1334040385911328 ,-0.9025348860186331 ,0.4094314863403992,
				0.2796031521466518 ,-0.430616743194378 ,-0.8581324476969294 },

				{ 0.8544274515527239, -2.652851027855309e-002 ,0.5188930218990411,
				-0.2557417071932098 ,-0.8908103320196923 ,0.3755704082710302,
				0.4522719416844477 ,-0.4536002540887367 ,-0.7679172483123512 }
			};

			for ( size_t i = 0; i < _countof( Rs ); i++ )
			{
				TestInverse< double, 3 >( Rs[i] );
			}
		}

		TEST_METHOD( MatrixInvertCorrectness3 )
		{
			double M[25] = {
				0.281129, 0.0515451, 0.0538386, 0.476397, 0.930885,
				0.391529, 0.272349, 0.407691, 0.384763, 0.752375,
				0.121093, 0.15536, 0.412755, 0.593424, 0.937535,
				0.651417, 0.39179, 0.96768, 0.0696437, 0.756864,
				0.0534638, 0.452781, 0.156085, 0.901915, 0.66643 };

			TestInverse< double, 5 >( M );
		}

		TEST_METHOD( MatrixInvertCorrectness4 )
		{
			double M[225] = {
				0.67219, 0.431556, 0.548418, 0.459355, 0.375244, 0.933047, 0.863162, 0.419339, 0.943765, 0.916051, 0.567261, 0.231972, 0.520518, 0.080906, 0.450535,
				0.520529, 0.0419026, 0.120874, 0.867017, 0.225223, 0.889283, 0.676534, 0.645125, 0.967331, 0.228988, 0.845189, 0.639063, 0.640531, 0.675048, 0.783442,
				0.346028, 0.705634, 0.528874, 0.937721, 0.202212, 0.0921036, 0.565858, 0.992098, 0.97562, 0.372634, 0.541497, 0.783646, 0.146707, 0.392988, 0.898215,
				0.384583, 0.56943, 0.36355, 0.912329, 0.838693, 0.633199, 0.772127, 0.539562, 0.423699, 0.0588238, 0.0483419, 0.978413, 0.781683, 0.544086, 0.799255,
				0.520243, 0.892997, 0.32155, 0.405067, 0.18148, 0.306025, 0.529341, 0.390496, 0.0450547, 0.93322, 0.939477, 0.442592, 0.621706, 0.147868, 0.418038,
				0.88073, 0.393383, 0.734741, 0.844615, 0.882656, 0.653346, 0.655771, 0.664687, 0.329197, 0.667175, 0.651493, 0.135165, 0.884429, 0.293578, 0.653433,
				0.23246, 0.583973, 0.906222, 0.0945135, 0.922639, 0.057166, 0.142523, 0.657068, 0.450706, 0.418247, 0.57942, 0.90408, 0.778943, 0.509216, 0.257237,
				0.770707, 0.732303, 0.483652, 0.17853, 0.173404, 0.838485, 0.216611, 0.326746, 0.595986, 0.661147, 0.877362, 0.0948094, 0.517894, 0.579482, 0.552097,
				0.376115, 0.527328, 0.0692514, 0.362947, 0.487509, 0.540797, 0.361675, 0.533513, 0.290157, 0.902327, 0.550597, 0.484785, 0.149027, 0.943378, 0.732788,
				0.977385, 0.315414, 0.423663, 0.526972, 0.0805049, 0.837276, 0.16381, 0.965259, 0.806627, 0.670333, 0.658179, 0.0249021, 0.759124, 0.630351, 0.998076,
				0.769282, 0.120088, 0.416487, 0.0341666, 0.269319, 0.26454, 0.549528, 0.785957, 0.662297, 0.372399, 0.539952, 0.926946, 0.839168, 0.731259, 0.735875,
				0.960481, 0.581888, 0.945319, 0.0162846, 0.729576, 0.315953, 0.243705, 0.862265, 0.818337, 0.271577, 0.508597, 0.38796, 0.895098, 0.562349, 0.967854,
				0.602474, 0.817964, 0.540852, 0.171982, 0.375202, 0.398605, 0.103252, 0.690953, 0.951618, 0.946147, 0.985627, 0.223559, 0.891844, 0.829765, 0.430479,
				0.90108, 0.503162, 0.136101, 0.887904, 0.137655, 0.754898, 0.0955911, 0.633311, 0.706143, 0.111076, 0.708357, 0.298013, 0.545843, 0.170495, 0.21194,
				0.624084, 0.659628, 0.361984, 0.637069, 0.661946, 0.756888, 0.0115227, 0.457312, 0.819894, 0.194213, 0.688813, 0.928327, 0.0136789, 0.626151, 0.812698 };

			TestInverse< double, 15 >( M );
		}


	};

}