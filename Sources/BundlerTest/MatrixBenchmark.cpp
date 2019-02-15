#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include <ctime>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( _Benchmark_Matrix ) {

		TEST_METHOD( MulM33V3Bench ) {
			const uint BATCH_SIZE = 1000;

			srand( (uint)time(NULL) );

			Matrix3x3 mat;
			Vector3 vect;
			Vector3 sink;

			Timer timer;
			auto tgFast = timer.GetTimeGroup( "benchFast" );

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
			}

			double fastLaTime = tgFast.GetAverage();


			auto tgStruct = timer.GetTimeGroup( "benchStruct" );

			for ( uint i = 0; i < BATCH_SIZE; i++ ) {
				for ( uint j = 0; j < 3; j++ ) {
					for ( uint k = 0; k < 3; k++ ) {
						mat[ j ][ k ] = ( (float)rand() ) / RAND_MAX;
					}
					vect[ j ] = ( (float)rand() ) / RAND_MAX;
				}

				tgStruct.Start();
				sink = mat * vect;
				tgStruct.SaveElapsed();
			}

			double matrixStructTime = tgStruct.GetAverage();


			char msg[ 256 ];
			sprintf_s( msg, "FastLA average time: %fms", fastLaTime );
			Logger::WriteMessage( msg );
			sprintf_s( msg, "Struct average time: %fms", matrixStructTime );
			Logger::WriteMessage( msg );

			
			Assert::IsTrue( fastLaTime < matrixStructTime );
		}

	};

}
