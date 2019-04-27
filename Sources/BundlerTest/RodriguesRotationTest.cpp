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

			AssertAreEqual( { Scalar( 0.0 ), Scalar( 0.0 ), Scalar( 1.0 ) }, axis, Scalar( 10e-5 ) );
			Assert::AreEqual( Scalar( 1.5708 ), angle, Scalar( 10e-4 ) );
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

			AssertAreEqual( { Scalar( 0.23 ), Scalar( 0.54 ), Scalar( 0.85 ) }, axis, Scalar( 10e-3 ) );
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
				Assert::AreEqual( expected[ i ].GetFx(), M[ i ].GetFx(), Scalar( 10e-5 ) );
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

			AssertAreEqual( expected, M, Scalar( 10e-4 ) );
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

			AssertAreEqual( M1, M2, Scalar( 10e-4 ) );
		}

		template < typename T >
		void TestRodrigues( __in const T( &R )[ 9 ] )
		{
			T angle;
			T axis[ 3 ];
			RodriguesRotation::GetFromRotationMatrix< T >( R, &angle, axis );

			T R2[ 9 ];
			RodriguesRotation::GetRotation< T >( angle, axis, R2 );

			AssertAreEqual( R, R2, T( 10e-6 ) );
		}

		TEST_METHOD( InverseTransform1 )
		{
			Scalar R1[ 9 ] = {
				0.9992772826309979f, 2.162559242174209e-002f, 3.126093680646133e-002f,
				8.818622544508919e-003f, -0.9318492685749851f, 0.3627384354499577f,
				3.6974914658811e-002f, -0.3622005996801693f, -0.9313665128601626f
			};

			TestRodrigues( R1 );
		}

		TEST_METHOD( InverseTransform2 )
		{
			double R1[ 9 ] = {
				0.940419083939694, -8.336377630598748e-003, 0.3399153591263963,
				-0.2068876064360509, -0.8073739525624313, 0.552580147152319,
				0.2698322902567233, -0.5899811908287559, -0.7609945660798734,
			};

			TestRodrigues( R1 );
		}

		TEST_METHOD( InverseTransform3 )
		{
			double Rs[ 10 ][ 9 ] = {
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
				
				{0.9989144599305349 ,2.913179826467167e-002 ,-3.634886616600846e-002,
				4.09951899652015e-002 ,-0.9203207200150964 ,0.389010496915715,
				-2.212003936264499e-002 ,-0.3900783391073657 ,-0.9205159386006492 },
				
				{ 0.950802524080175, -0, 0.3097976116802196,
				-0.1334040385911328 ,-0.9025348860186331 ,0.4094314863403992,
				0.2796031521466518 ,-0.430616743194378 ,-0.8581324476969294 },

				{ 0.8544274515527239, -2.652851027855309e-002 ,0.5188930218990411,
				-0.2557417071932098 ,-0.8908103320196923 ,0.3755704082710302,
				0.4522719416844477 ,-0.4536002540887367 ,-0.7679172483123512 }
			};

			for ( uint i = 0; i < 10; i++ ) 
			{
				TestRodrigues( Rs[ i ] );
			}
		}

	};

}
