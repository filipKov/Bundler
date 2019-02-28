#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {

	TEST_CLASS( BasicJacobianProviderTest ) 
	{
		TEST_METHOD( Initialize0 )
		{
			constexpr const size_t camCount = 3;
			Camera cameras[ camCount ];
			
			constexpr const size_t ptCount = 4;
			Vector3 points[ ptCount ] = {
				Vector3( { 14, 21, 15 } ),
				Vector3( { 18, 9, 10 } ),
				Vector3( { 27, 32, 17 } ),
				Vector3( { 20, 40, 23 } ),
			};

			constexpr const size_t projectionCount = 9;
			Projection projections[ projectionCount ] = {
				{ 0, 0, Vector2( { 40, 20 } ) },
				{ 0, 1, Vector2( { 150, 80 } ) },

				{ 1, 0, Vector2( { 45, 83 } ) },
				{ 1, 1, Vector2( { 98, 126 } ) },
				{ 1, 2, Vector2( { 20, 53 } ) },
				{ 1, 3, Vector2( { 100, 40 } ) },

				{ 2, 1, Vector2( { 5, 10 } ) },
				{ 2, 2, Vector2( { 10, 30 } ) },
				{ 2, 3, Vector2( { 30, 50 } ) },
			};

			CameraModel6DoF< 10 > camModels[ camCount ];
			for ( uint i = 0; i < camCount; i++ ) {
				camModels[ i ].Initialize( &cameras[ i ] );
			}

			BasicJacobianProvider< CameraModel6DoF< 10 > > provider;

			provider.Initialize(
				camCount, camModels,
				ptCount, points,
				projectionCount, projections
			);

			Assert::AreEqual( camCount, provider.GetCameraCount() );
			Assert::AreEqual( size_t( 2 ), provider.GetCameraProjectionCount( 0 ) );
			Assert::AreEqual( size_t( 4 ), provider.GetCameraProjectionCount( 1 ) );
			Assert::AreEqual( size_t( 3 ), provider.GetCameraProjectionCount( 2 ) );

			Assert::AreEqual( ptCount, provider.GetPointCount() );
			Assert::AreEqual( size_t( 2 ), provider.GetPointProjectionCount( 0 ) );
			Assert::AreEqual( size_t( 3 ), provider.GetPointProjectionCount( 1 ) );
			Assert::AreEqual( size_t( 2 ), provider.GetPointProjectionCount( 2 ) );
			Assert::AreEqual( size_t( 2 ), provider.GetPointProjectionCount( 3 ) );
		}


	};

}

