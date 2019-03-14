#include "stdafx.h"

namespace NumericOptimizationTest {

	TEST_CLASS( MatrixDynamicTest ) {

		TEST_METHOD( Transpose ) {
			Matrix<int> matrix = { {1,2,3}, {4,5,6} };
			Matrix<int> mT = matrix.Transpose();

			for ( int r = 0; r < 2; r++ ) {
				for ( int c = 0; c < 3; c++ ) {
					Assert::AreEqual( matrix[r][c], mT[c][r] );
				}
			}
		}

		TEST_METHOD( UnaryMinusOperator ) {
			Matrix<int> matrix = { {1,2,3}, {4,5,6}, {7,8,9} };
			Matrix<int> negM = -matrix;

			int* pEl = negM.Elements();

			for ( int i = 0; i < 9; i++, pEl++ ) {
				Assert::AreEqual( -( i + 1 ), *pEl );
			}
		}

		TEST_METHOD( MatrixBinaryPlusOperator ) {
			Matrix<int> m1 = { {1,2,3}, {4,5,6} };
			Matrix<int> m2 = { {9,8,7}, {6,5,4} };
			
			auto m3 = m1 + m2;
			auto pEl = m3.Elements();

			for ( int i = 0; i < 6; i++, pEl++ ) {
				Assert::AreEqual( 10, *pEl );
			}
		}

		TEST_METHOD( MatrixBinaryPlusOperatorThrows ) {
			Matrix<int> m1 = { { 1,2,3 }, { 4,5,6 } };
			Matrix<int> m2 = { { 9,8,7 }, { 6,5,4 }, {3, 2, 1} };

			TestThrowsException<InvalidDimensionsException>( [&] {
				auto m3 = m1 + m2;
			} );
		}

		TEST_METHOD( MatrixBinaryMinusOperator ) {
			Matrix<int> m1 = { { 10,9,8 }, { 7,6,5 } };
			Matrix<int> m2 = { { 9,8,7 }, { 6,5,4 } };

			auto m3 = m1 - m2;
			auto pEl = m3.Elements();

			for ( int i = 0; i < 6; i++, pEl++ ) {
				Assert::AreEqual( 1, *pEl );
			}
		}

		TEST_METHOD( MatrixBinaryMinusOperatorThrows ) {
			Matrix<int> m1 = { { 1,2,3 }, { 4,5,6 } };
			Matrix<int> m2 = { { 9,8,7 }, { 6,5,4 }, {3, 2, 1} };

			TestThrowsException<InvalidDimensionsException>( [&] {
				auto m3 = m1 - m2;
			} );
		}

		TEST_METHOD( MatrixBinaryMultiplicationOperator ) {
			Matrix<int> m1 = { {1,2,3}, {4,5,6} };
			
			auto m3 = m1 * m1.Transpose();

			uint dimR, dimC;
			m3.GetDimensions( dimR, dimC );
			Assert::AreEqual( 2u, dimR );
			Assert::IsTrue( dimR == dimC );

			Assert::AreEqual( 1+4+9, m3[0][0] );
			Assert::AreEqual( 4+10+18, m3[0][1] );
			Assert::AreEqual( 4+10+18, m3[1][0] );
			Assert::AreEqual( 16+25+36, m3[1][1] );
	
		}

		TEST_METHOD( MatrixBinaryMultiplicationOperatorThrows ) {
			Matrix<int> m1 = { { 1,2,3}, {4,5,6} };
			Matrix<int> m2 = { {1,2,3,4} };

			TestThrowsException<InvalidDimensionsException>( [&] {
				auto m3 = m1 * m2;
			} );
		}

		TEST_METHOD( MatrixMultiplyAtB_0 )
		{
			Matrix< double > M0( { { 4, 5 } } );
			Matrix< double > M1( { { 1, 2, 3 } } );

			double M2[ 6 ];
			MatrixMultiplyAtB< double >( 1, 2, 3, M0.Elements(), M1.Elements(), M2 );

			double expected[ 6 ] = {
				4, 8, 12,
				5, 10, 15
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

		TEST_METHOD( MatrixMultiplyAtB_1 ) 
		{
			Matrix< double > M0( { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } } );
			Matrix< double > M1( { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } } );

			double M2[ 9 ];
			MatrixMultiplyAtB< double >( 3, 3, 3, M0.Elements(), M1.Elements(), M2 );

			double expected[ 9 ] = {
				66, 78, 90,
				78, 93, 108,
				90, 108, 126
			};

			AssertAreEqual( expected, M2, 10e-10 );
		}

	};

}