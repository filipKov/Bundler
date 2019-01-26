#include "stdafx.h"

namespace NumericOptimizationTest {

	TEST_CLASS( MatrixStaticBaseTest ) {

		TEST_METHOD( DefaultConstructor ) {
			Matrix<float, 5, 2 > base;
			float *pElems = base.Elements();

			for ( int i = 0; i < 10; i++, pElems++ ) {
				Assert::AreEqual( 0.f, *pElems );
			}
		}

		TEST_METHOD( InitializerListConstructor ) {
			Matrix<float, 2, 3> base = { { 1,2,3}, {4,5,6} };
			float *pElems = base.Elements();

			for ( int i = 0; i < 6; i++, pElems++ ) {
				Assert::AreEqual( (float)(i+1), *pElems );
			}
		}

		TEST_METHOD( InitializerListConstructorThrowsRows ) {
			TestThrowsException<InvalidArgumentException>( [] {
				Matrix<float, 2, 3> base = { {1,2,3,4}, {5,6,7,8} };
			} );

			TestThrowsException<InvalidArgumentException>( [] {
				Matrix<float, 2, 3> base = { {1,2,3}, {1,2,4}, {1,2,5} };
			} );
		}

		TEST_METHOD( CopyConstructor ) {
			Matrix< int, 2, 3> base1 = { {1,2,3}, {4,5,6} };
			auto base2( base1 );

			int* pE1 = base1.Elements();
			int* pE2 = base2.Elements();

			for ( int i = 0; i < 6; i++, pE1++, pE2++ ) {
				Assert::AreEqual( *pE1, *pE2 );
			}
		}

		TEST_METHOD( Getter ) {
			Matrix<int, 2, 3> base = { {1,2,3}, {4,5,6} };

			for ( int r = 0; r < 2; r++ ) {
				for ( int c = 0; c < 3; c++ ) {
					Assert::AreEqual( r * 3 + c + 1, base.Get( r, c ) );
				}
			}
		}

		TEST_METHOD( GetterThrows ) {
			Matrix< int, 2, 3> base = { {1,2,3}, {4,5,6} };

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base.Get( 3, 0 );
			} );

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base.Get( 1, 14 );
			} );

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base.Get( 321, 14 );
			} );

		}

		TEST_METHOD( Setter ) {
			Matrix<int, 2, 3> base = { {1,2,3}, {4,5,6} };
			
			base.Set( 0, 1, 123 );
			base.Set( 1, 2, 456 );
			
			Assert::AreEqual( 123, base.Get( 0, 1 ) );
			Assert::AreEqual( 456, base.Get( 1, 2 ) );
		}

		TEST_METHOD( SetterThrows ) {
			Matrix<int, 2, 3> base = { { 1,2,3 }, { 4,5,6 } };

			TestThrowsException< IndexOutOfRangeException >( [&base] {
				base.Set( 4, 1, 9 );
			} );

			TestThrowsException< IndexOutOfRangeException >( [&base] {
				base.Set( 0, 12, 98 );
			} );

			TestThrowsException< IndexOutOfRangeException >( [&base] {
				base.Set( 87, 242, 987 );
			} );

		}

		TEST_METHOD( Accessor ) {
			Matrix<int, 2, 3> base = { { 1,2,3 }, { 4,5,6 } };

			Assert::AreEqual( 2, base[0][1] );
			Assert::AreEqual( 4, base[1][0] );

			base[1][2] = 12;
			base[0][0] = 34;

			Assert::AreEqual( 12, base[1][2] );
			Assert::AreEqual( 34, base[0][0] );
		}

		TEST_METHOD( AccessorThrows ) {
			Matrix<int, 2, 3> base = { { 1,2,3 }, { 4,5,6 } };

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base[4];
			} );

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base[21];
			} );

		}

		TEST_METHOD( AssignOperator ) {
			Matrix< int, 3, 3 > src = { { 1,2,3}, {4,5,6}, {7,8,9} };
			Matrix< int, 3, 3 > dst = { {9,8,7}, {6,5,4}, {3,2,1} };

			dst = src;

			for ( int r = 0; r < 3; r++ ) {
				for ( int c = 0; c < 3; c++ ) {
					Assert::AreEqual( src[r][c], dst[r][c] );
				}
			}
		}

		TEST_METHOD( ScalarMultiplicationAssignOperator ) {
			Matrix< int, 3, 3 > src = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };

			src *= 5;
			int* pEl = src.Elements();

			for ( int i = 0; i < 9; i++, pEl++ ) {
				Assert::AreEqual( ( i + 1 ) * 5, *pEl );
			}
		}

		TEST_METHOD( MatrixAdditionAssignOperator ) {
			Matrix< int, 3, 3 > src = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };
			Matrix< int, 3, 3 > dst = { { 9,8,7 }, { 6,5,4 }, { 3,2,1 } };

			src += dst;
			int* pElS = src.Elements();
			int* pElD = dst.Elements();
 
			for ( int i = 0; i < 9; i++, pElS++, pElD++ ) {
				Assert::AreEqual( 10, *pElS );
				Assert::AreEqual( 9 - i, *pElD );
			}
		}

		TEST_METHOD( MatrixSubtractAssignOperator ) {
			Matrix< int, 3, 3 > src = { { 10,9,8 }, { 7,6,5 }, { 4,3,2 } };
			Matrix< int, 3, 3 > dst = { { 9,8,7 }, { 6,5,4 }, { 3,2,1 } };

			src -= dst;
			int* pElS = src.Elements();
			int* pElD = dst.Elements();

			for ( int i = 0; i < 9; i++, pElS++, pElD++ ) {
				Assert::AreEqual( 1, *pElS );
				Assert::AreEqual( 9 - i, *pElD );
			}
		}

	};

}