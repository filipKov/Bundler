#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include <ctime>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( _Benchmark_Matrix ) {

		TEST_METHOD( MulM33V3Bench ) {
			Logger::WriteMessage( "---- MulM33V3Bench Start ----" );

			const uint BATCH_SIZE = 10000;

			srand( (uint)time(NULL) );

			Matrix3x3 mat;
			Vector3 vect;
			Vector3 sink;

			Timer timer;
			auto tgFast = timer.GetTimeGroup( "benchFast" );
			auto tgStruct = timer.GetTimeGroup( "benchStruct" );

			for ( uint i = 0; i < BATCH_SIZE; i++ ) {

				for ( uint j = 0; j < 3; j++ ) {
					for ( uint k = 0; k < 3; k++ ) {
						mat[ j ][ k ] = ( (float)rand() ) / RAND_MAX;
					}
					vect[ j ] = ( (float)rand() ) / RAND_MAX;
				}

				tgFast.Start();
				MulM33V3( mat.Elements(), vect.Elements(), sink.Elements() );
				tgFast.SaveElapsed();

				tgStruct.Start();
				sink = mat * vect;
				tgStruct.SaveElapsed();
			}

			double fastLaTime = tgFast.GetAverage();
			double matrixStructTime = tgStruct.GetAverage();


			char msg[ 256 ];
			sprintf_s( msg, "\tFastLA average time: %fms", fastLaTime );
			Logger::WriteMessage( msg );
			sprintf_s( msg, "\tStruct average time: %fms", matrixStructTime );
			Logger::WriteMessage( msg );

			
			Assert::IsTrue( fastLaTime < matrixStructTime );
		}


		TEST_METHOD( Mat33Bench ) {
			Logger::WriteMessage( "---- Mat33Bench Start ----" );


			auto structFnc = [] ( __inout Matrix3x3& R, __in Scalar angle, __in_ecount(3) Scalar* pAxis ) -> void {
				Matrix3x3 K( {
					0,						-ELEMENT( pAxis, 2 ),	ELEMENT( pAxis, 1 ),
					ELEMENT( pAxis, 2 ),	0,						-ELEMENT( pAxis, 0 ),
					-ELEMENT( pAxis, 1 ),	ELEMENT( pAxis, 0 ),	0
				} );

				R.SetIdentity();
				R += ( K * sin( angle ) ) + ( K * K * ( Scalar( 1 ) - cos( angle ) ) );
			};

			auto fastLaFunc = [] ( __inout_ecount( 9 ) Scalar* R, __in Scalar angle, __in_ecount( 3 ) Scalar* pAxis ) -> void {
				Scalar K[ 9 ] = {
					0,						-ELEMENT( pAxis, 2 ),	ELEMENT( pAxis, 1 ),
					ELEMENT( pAxis, 2 ),	0,						-ELEMENT( pAxis, 0 ),
					-ELEMENT( pAxis, 1 ),	ELEMENT( pAxis, 0 ),	0
				};
				Scalar K2[ 9 ];

				MulM33M33( K, K, K2 );
				MulM33C( K2, ( Scalar( 1 ) - cos( angle ) ), K2 );

				MulM33C( K, sin( angle ), K );

				AddM33M33( K, K2, K );

				IdentityM33( R );
				AddM33M33( R, K, R );
			};

			const uint BATCH_SIZE = 10000;

			Matrix3x3 R1;
			Scalar R2[ 9 ];

			{
				Scalar angle = ( (Scalar)rand() ) / RAND_MAX;
				Scalar axis[ 3 ] = { ( (Scalar)rand() ) / RAND_MAX, ( (Scalar)rand() ) / RAND_MAX, ( (Scalar)rand() ) / RAND_MAX };

				fastLaFunc( R2, angle, axis );
				structFnc( R1, angle, axis );

				AssertAreEqual( 9, R2, R1.Elements() );
			}

			Timer timer;
			auto tgFastLa = timer.GetTimeGroup( "benchFast" );
			auto tgStruct = timer.GetTimeGroup( "benchStruct" );

			for ( uint i = 0; i < BATCH_SIZE; i++ ) {
				Scalar angle = ( (Scalar)rand() ) / RAND_MAX;
				Scalar axis[ 3 ] = { ( (Scalar)rand() ) / RAND_MAX, ( (Scalar)rand() ) / RAND_MAX, ( (Scalar)rand() ) / RAND_MAX };

				tgFastLa.Start();
				fastLaFunc( R2, angle, axis );
				tgFastLa.SaveElapsed();

				tgStruct.Start();
				structFnc( R1, angle, axis );
				tgStruct.SaveElapsed();
			}

			double fastLaTime = tgFastLa.GetAverage();
			double structTime = tgStruct.GetAverage();


			char msg[ 256 ];
			sprintf_s( msg, "\tFastLA average time: %fms", fastLaTime );
			Logger::WriteMessage( msg );
			sprintf_s( msg, "\tStruct average time: %fms", structTime );
			Logger::WriteMessage( msg );

			Assert::IsTrue( fastLaTime < structTime );

		}

	};

}
