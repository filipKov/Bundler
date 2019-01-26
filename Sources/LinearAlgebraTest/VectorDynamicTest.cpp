#include "stdafx.h"

namespace NumericOptimizationTest {

	TEST_CLASS( VectorDynamicTest ) {

		TEST_METHOD( UnaryMinus ) {
			LinearAlgebra::Vector<int> v = { 1,2,3,4 };
			auto v2 = -v;

			for ( int i = 0; i < 4; i++ ) {
				Assert::AreEqual( -( v[i] ), v2[i] );
			}
		}

		TEST_METHOD( BinaryScalarMultiplication ) {
			LinearAlgebra::Vector<int> v = { 1,2,3,4 };

			auto v2 = 5 * v;
			auto v3 = v * 8;

			for ( int i = 0; i < 4; i++ ) {
				Assert::AreEqual( 5 * v[i], v2[i] );
				Assert::AreEqual( 8 * v[i], v3[i] );
			}
		}

		TEST_METHOD( BinaryVectorAddition ) {
			LinearAlgebra::Vector<int> v1 = { 1,2,3,4 };
			LinearAlgebra::Vector<int> v2 = { 9,8,7,6 };

			auto v3 = v1 + v2;

			for ( int i = 0; i < 4; i++ ) {
				Assert::AreEqual( 10, v3[i] );
			}
		}

		TEST_METHOD( BinaryVectorAdditionThrows ) {
			LinearAlgebra::Vector<int> v1 = { 1,2,3,4 };
			LinearAlgebra::Vector<int> v2 = { 9,8,7,6,5 };
			LinearAlgebra::Vector<int> v3 = { 7,6,5 };


			TestThrowsException<InvalidDimensionsException>( [&] {
				auto v3 = v1 + v2;
			} );

			TestThrowsException<InvalidDimensionsException>( [&] {
				auto v4 = v1 + v3;
			} );
		}

		TEST_METHOD( BinaryVectorSubtraction ) {
			LinearAlgebra::Vector<int> v1 = { 19,18,17,16 };
			LinearAlgebra::Vector<int> v2 = { 9,8,7,6 };

			auto v3 = v1 - v2;

			for ( int i = 0; i < 4; i++ ) {
				Assert::AreEqual( 10, v3[i] );
			}
		}

		TEST_METHOD( BinaryVectorSubtractionThrows ) {
			LinearAlgebra::Vector<int> v1 = { 1,2,3,4 };
			LinearAlgebra::Vector<int> v2 = { 9,8,7,6,5 };
			LinearAlgebra::Vector<int> v3 = { 7,6,5 };


			TestThrowsException<InvalidDimensionsException>( [&] {
				auto v3 = v1 - v2;
			} );

			TestThrowsException<InvalidDimensionsException>( [&] {
				auto v4 = v1 - v3;
			} );
		}

		TEST_METHOD( MatrixVectorMultiplication ) {
			LinearAlgebra::Matrix<int> M = { {1,2,3}, {4,5,6}, {7,8,9} };
			LinearAlgebra::Vector<int> v = { 1,2,3 };

			auto v2 = M * v;

			Assert::AreEqual( 3u, v2.GetDimension() );
			Assert::AreEqual( 14, v2[0] );
			Assert::AreEqual( 14+18, v2[1] ); 
			Assert::AreEqual( 7+16+27, v2[2] );
		}

		TEST_METHOD( MatrixVectorMultiplicationThrows ) {
			LinearAlgebra::Matrix<int> M = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };
			LinearAlgebra::Vector<int> v = { 1,2,3,4 };
			LinearAlgebra::Vector<int> v2 = { 1,2 };

			TestThrowsException<InvalidDimensionsException>( [&] {
				M * v;
			} );

			TestThrowsException<InvalidDimensionsException>( [&] {
				M * v2;
			} );;
		}
	};

}