#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( CameraModel6DoFTest ) {

		TEST_METHOD( BUILD_BREAK_TEST ) {
			CameraModel6DoF< 25 > camModel;
		}

	};

}