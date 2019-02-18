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

	};

}