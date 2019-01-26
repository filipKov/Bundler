#include "stdafx.h"

namespace NumericOptimizationTest {

	TEST_CLASS( MatrixStaticTest ) {


		TEST_METHOD( UnaryMinusOperator ) {
			Matrix<int,3,3> matrix = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };
			Matrix<int,3,3> negM = -matrix;

			int* pEl = negM.Elements();

			for ( int i = 0; i < 9; i++, pEl++ ) {
				Assert::AreEqual( -( i + 1 ), *pEl );
			}
		}

		TEST_METHOD( BinaryScalarMultiplicationLeft ) {
			Matrix<int, 3, 3> matrix = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };

			auto m2 = 5 * matrix;
			auto pEl = m2.Elements();

			for ( int i = 0; i < 9; i++, pEl++ ) {
				Assert::AreEqual( 5*(i+1), *pEl );
			}
		}

		TEST_METHOD( BinaryScalarMultiplicationRight ) {
			Matrix<int, 3, 3> matrix = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };

			auto m2 = matrix * 48;
			auto pEl = m2.Elements();

			for ( int i = 0; i < 9; i++, pEl++ ) {
				Assert::AreEqual( 48 * ( i + 1 ), *pEl );
			}
		}

		TEST_METHOD( MatrixBinaryPlusOperator ) {
			Matrix<int,2,3> m1 = { { 1,2,3 }, { 4,5,6 } };
			Matrix<int,2,3> m2 = { { 9,8,7 }, { 6,5,4 } };

			auto m3 = m1 + m2;
			auto pEl = m3.Elements();

			for ( int i = 0; i < 6; i++, pEl++ ) {
				Assert::AreEqual( 10, *pEl );
			}
		}


		TEST_METHOD( MatrixBinaryMinusOperator ) {
			Matrix<int,2,3> m1 = { { 10,9,8 }, { 7,6,5 } };
			Matrix<int,2,3> m2 = { { 9,8,7 }, { 6,5,4 } };

			auto m3 = m1 - m2;
			auto pEl = m3.Elements();

			for ( int i = 0; i < 6; i++, pEl++ ) {
				Assert::AreEqual( 1, *pEl );
			}
		}

	};

}