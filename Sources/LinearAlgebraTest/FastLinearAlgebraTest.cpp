#include "stdafx.h"

namespace LinearAlgebraTest {

	TEST_CLASS( FastVector4Test ) {

		TEST_METHOD( BUILD_BREAK_TEST ) {
			float M[ 16 ];
			float v1[ 4 ];
			float v2[ 4 ];

			AddV4V4( v1, v2, v1 );
			auto d = DotV4V4( v1, v2 );
			auto L = LengthV4( v1 );
			MulM34V4( M, v1, v2 );
			MulM44V4( M, v1, v2 );
			MulV4C( v1, 10.0f, v2 );
			NormalizeV4( v1, v1 );

			Assert::IsTrue( true );
		}

	};

}