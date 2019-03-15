#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include <ctime>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( _Benchmark_Matrix ) {

		TEST_METHOD( M33MulV3Bench ) {
			Logger::WriteMessage( "---- M33MulV3Bench Start ----" );

			srand( (uint)time(NULL) );

			Matrix3x3 mat;
			Vector3 vect;

			Scalar sink0[ 3 ];
			Vector3 sink1;

			HighResolutionClock stopwatchFast;
			HighResolutionClock stopwatchStruct;

			for ( uint i = 0; i < Settings::BENCHMARK_BATCH_SIZE; i++ ) {

				Random< Scalar >::Matrix( 3, 3, mat.Elements() );
				Random< Scalar >::Matrix( 3, 1, vect.Elements() );

				stopwatchFast.Start();
				M33MulV3( mat.Elements(), vect.Elements(), sink0 );
				stopwatchFast.Stop();

				stopwatchStruct.Start();
				sink1 = mat * vect;
				stopwatchStruct.Stop();

				AssertAreEqual( sink0, sink1.Elements() );
			}

			double fastLaTime = stopwatchFast.GetTotalTime< TimeUnits::Nanoseconds >() / Settings::BENCHMARK_BATCH_SIZE;
			double matrixStructTime = stopwatchStruct.GetTotalTime< TimeUnits::Nanoseconds >() / Settings::BENCHMARK_BATCH_SIZE;

			char msg[ 256 ];
			sprintf_s( msg, "\tFastLA average time: %fns", fastLaTime );
			Logger::WriteMessage( msg );
			sprintf_s( msg, "\tStruct average time: %fns", matrixStructTime );
			Logger::WriteMessage( msg );

			
			Assert::IsTrue( true );
		}


		TEST_METHOD( Mat33Bench ) {
			Logger::WriteMessage( "---- Mat33Bench Start ----" );


			auto structFnc = [] ( __inout Matrix3x3& R, __in Scalar angle, __in_ecount(3) Scalar* pAxis ) -> void {
				Matrix3x3 K( {
					Scalar( 0 ),			-ELEMENT( pAxis, 2 ),	ELEMENT( pAxis, 1 ),
					ELEMENT( pAxis, 2 ),	Scalar( 0 ),			-ELEMENT( pAxis, 0 ),
					-ELEMENT( pAxis, 1 ),	ELEMENT( pAxis, 0 ),	Scalar( 0 )
				} );

				Scalar angleCosine = cos( angle );
				
				K *= sin( angle );

				Matrix3x3 K2;
				V3OuterProduct( pAxis, pAxis, K2.Elements() );
				K2 *= Scalar( 1 ) - angleCosine;

				R.SetIdentity();
				R *= angleCosine;
				R += K + K2;
			};

			auto fastLaFunc = [] ( __inout_ecount( 9 ) Scalar* R, __in Scalar angle, __in_ecount( 3 ) Scalar* pAxis ) -> void {
				Scalar K[ 9 ] = {
					Scalar( 0 ),			-ELEMENT( pAxis, 2 ),	ELEMENT( pAxis, 1 ),
					ELEMENT( pAxis, 2 ),	Scalar( 0 ),			-ELEMENT( pAxis, 0 ),
					-ELEMENT( pAxis, 1 ),	ELEMENT( pAxis, 0 ),	Scalar( 0 )
				};

				Scalar K2[ 9 ];
				Scalar angleCosine = cos( angle );

				M33MulC( K, sin( angle ), K );

				V3OuterProduct( pAxis, pAxis, K2 );
				M33MulC( K2, ( Scalar( 1 ) - angleCosine ), K2 );

				M33Identity( R );
				M33MulC( R, angleCosine, R );

				M33AddM33( K, K2, K );
				M33AddM33( R, K, R );
			};

			Matrix3x3 R1;
			Scalar R2[ 9 ];

			HighResolutionClock stopwatchFast;
			HighResolutionClock stopwatchStruct;

			for ( uint i = 0; i < Settings::BENCHMARK_BATCH_SIZE; i++ ) {
				Scalar angle = Random< Scalar >::Value();
				Scalar axis[ 3 ];
				Random<Scalar>::Matrix( 3, 1, axis );

				stopwatchFast.Start();
				fastLaFunc( R2, angle, axis );
				stopwatchFast.Stop();

				stopwatchStruct.Start();
				structFnc( R1, angle, axis );
				stopwatchStruct.Stop();

				AssertAreEqual( R2, R1.Elements() );
			}

			double fastLaTime = stopwatchFast.GetTotalTime< TimeUnits::Nanoseconds >() / Settings::BENCHMARK_BATCH_SIZE;
			double structTime = stopwatchStruct.GetTotalTime< TimeUnits::Nanoseconds >() / Settings::BENCHMARK_BATCH_SIZE;


			char msg[ 256 ];
			sprintf_s( msg, "\tFastLA average time: %fns", fastLaTime );
			Logger::WriteMessage( msg );
			sprintf_s( msg, "\tStruct average time: %fns", structTime );
			Logger::WriteMessage( msg );

			Assert::IsTrue( true );

		}

		__declspec( noinline ) void Bench1( HighResolutionClock& stopwatch, double* m1, double* m2, double* sink )
		{
			stopwatch.Start();
			M44MulM44( m1, m2, sink );
			stopwatch.Stop();
		}

		__declspec( noinline ) void Bench2( HighResolutionClock& stopwatch, double* m1, double* m2, double* sink )
		{
			stopwatch.Start();
			MatrixMultiply<double, 4, 4, 4>( m1, m2, sink );
			stopwatch.Stop();
		}

		TEST_METHOD( M44MulM44Bench ) {
			Logger::WriteMessage( "---- M44MulM44Bench Start ----" );

			Matrix< double, 4, 4 > m1;
			Matrix< double, 4, 4 > m2;

			Matrix< double, 4, 4 > sink1;
			Matrix< double, 4, 4 > sink2;
			Matrix< double, 4, 4 > sink3;

			double* pM1 = m1.Elements();
			double* pM2 = m2.Elements();

			HighResolutionClock stopwatchFast;
			HighResolutionClock stopwatchUnrolled;
			HighResolutionClock stopwatchStruct;

			Random<Scalar>::Seed();

			for ( uint batchI = 0; batchI < Settings::BENCHMARK_BATCH_SIZE; batchI++ )
			{
				Random< double >::Fill( 16, pM1 );
				Random< double >::Fill( 16, pM2 );

				Bench1( stopwatchFast, pM1, pM2, sink1.Elements() );
				
				Bench2( stopwatchUnrolled, pM1, pM2, sink2.Elements() );
				
				stopwatchStruct.Start();
				sink3 = m1 * m2;
				stopwatchStruct.Stop();
				
				AssertAreEqual( 16, sink1.Elements(), sink2.Elements() );
				AssertAreEqual( 16, sink2.Elements(), sink3.Elements() );
			}

			double fastTime = stopwatchFast.GetTotalTime< TimeUnits::Nanoseconds >() / Settings::BENCHMARK_BATCH_SIZE;
			double unrolledTime = stopwatchUnrolled.GetTotalTime< TimeUnits::Nanoseconds >() / Settings::BENCHMARK_BATCH_SIZE;
			double structTime = stopwatchStruct.GetTotalTime< TimeUnits::Nanoseconds >() / Settings::BENCHMARK_BATCH_SIZE;


			char msg[ 256 ];
			sprintf_s( msg, "\tFastLA average time: %fns", fastTime );
			Logger::WriteMessage( msg );
			sprintf_s( msg, "\tUnrolled average time: %fns", unrolledTime );
			Logger::WriteMessage( msg );
			sprintf_s( msg, "\tStruct average time: %fns", structTime );
			Logger::WriteMessage( msg );

			Assert::IsTrue( true );

		}
	};

}
