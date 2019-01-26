#include "stdafx.h"

namespace NumericOptimizationTest {

	TEST_CLASS( MatrixDynamicBaseTests ) {

		TEST_METHOD( DimensionConstructor ) {
			Matrix<int> base(3, 3);

			uint r = 0, c = 0;
			base.GetDimensions( r, c );
			int* pEl = base.Elements();

			Assert::AreEqual( 3u, r );
			Assert::AreEqual( 3u, c );

			for ( int i = 0; i < 9; i++, pEl++ ) {
				Assert::AreEqual( 0, *pEl );
			}

		}

		TEST_METHOD( InitalizerListConstructor ) {
			Matrix<int> base = { {1,2,3}, {4,5,6} };

			uint r = 0, c = 0;
			base.GetDimensions( r, c );
			int* pEl = base.Elements();

			Assert::AreEqual( 2u, r );
			Assert::AreEqual( 3u, c );

			for ( int i = 0; i < 6; i++, pEl++ ) {
				Assert::AreEqual( i+1, *pEl );
			}
		}

		TEST_METHOD( CopyConstructor ) {
			Matrix<int> base = { { 1,2,3 }, { 4,5,6 } };
			auto base2( base );
			
			uint r = 0, c = 0;
			uint r2 = 0, c2 = 0;
			base.GetDimensions( r, c );
			base2.GetDimensions( r2, c2 );

			Assert::AreEqual( r, r2 );
			Assert::AreEqual( c, c2 );

			int* pEl1 = base.Elements();
			int* pEl2 = base2.Elements();

			for ( int i = 0; i < 6; i++, pEl1++, pEl2++ ) {
				Assert::AreEqual( *pEl1, *pEl2 );
			}
		}

		TEST_METHOD( MoveConstructor ) {
			Matrix<int> base = { { 1,2,3 }, { 4,5,6 } };
			auto base2( std::move(base) );

			uint r = 0, c = 0;
			uint r2 = 0, c2 = 0;
			base.GetDimensions( r, c );
			base2.GetDimensions( r2, c2 );

			Assert::AreEqual( 2u, r2 );
			Assert::AreEqual( 3u, c2 );
			Assert::AreEqual( 0u, r );
			Assert::AreEqual( 0u, c );

			Assert::IsNull( base.Elements() );
			for ( int i = 0; i < 6; i++ ) {
				Assert::AreEqual( i + 1, *( base2.Elements() + i ) );
			}

		}

		TEST_METHOD( Getter ) {
			Matrix<int> base = { { 1,2,3 }, { 4,5,6 } };

			Assert::AreEqual( 3, base.Get( 0, 2 ) );
			Assert::AreEqual( 4, base.Get( 1, 0 ) );
		}

		TEST_METHOD( GetterThrows ) {
			Matrix<int> base = { { 1,2,3 }, { 4,5,6 } };

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base.Get( 3, 1 );
			} );

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base.Get( 0, 12 );
			} );

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base.Get( 39, 21 );
			} );
		}

		TEST_METHOD( Setter ) {
			Matrix<int> base = { { 1,2,3 }, { 4,5,6 } };

			base.Set( 0, 1, 123 );
			base.Set( 1, 2, 456 );

			Assert::AreEqual( 123, base.Get( 0, 1 ) );
			Assert::AreEqual( 456, base.Get( 1, 2 ) );
		}

		TEST_METHOD( SetterThrows ) {
			Matrix<int> base = { { 1,2,3 }, { 4,5,6 } };

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
			Matrix<int> base = { { 1,2,3 }, { 4,5,6 } };

			Assert::AreEqual( 2, base[0][1] );
			Assert::AreEqual( 4, base[1][0] );

			base[1][2] = 12;
			base[0][0] = 34;

			Assert::AreEqual( 12, base[1][2] );
			Assert::AreEqual( 34, base[0][0] );
		}

		TEST_METHOD( AccessorThrows ) {
			Matrix<int> base = { { 1,2,3 }, { 4,5,6 } };

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base[4];
			} );

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base[21];
			} );

		}

		TEST_METHOD( HasEqualDimensions ) {
			Matrix<int> b1( 5, 8 );
			Matrix<int> b2( 1234, 321 );
			Matrix<int> b3( 5, 8 );

			Assert::IsFalse( b1.HasEqualDimensions( b2 ) );
			Assert::IsTrue( b1.HasEqualDimensions( b3 ) );
			Assert::IsFalse( b2.HasEqualDimensions( b3 ) );
		}

		TEST_METHOD( MultiplicationDimensionsCheck ) {
			Matrix<int> b1( 5, 8 );
			Matrix<int> b2( 32, 12 );
			Matrix<int> b3( 8, 99 );
			Matrix<int> b4( 12, 12 );

			Assert::IsFalse( b1.CheckMultiplicationDimensions( b1 ) );

			Assert::IsFalse( b1.CheckMultiplicationDimensions( b2 ) );
			Assert::IsTrue( b1.CheckMultiplicationDimensions( b3 ) );
			Assert::IsFalse( b1.CheckMultiplicationDimensions( b4 ) );

			Assert::IsFalse( b2.CheckMultiplicationDimensions( b3 ) );
			Assert::IsTrue( b2.CheckMultiplicationDimensions( b4 ) );

			Assert::IsFalse( b3.CheckMultiplicationDimensions( b4 ) );
			Assert::IsFalse( b4.CheckMultiplicationDimensions( b2 ) );
		}

		TEST_METHOD( AssignOperator ) {
			Matrix< int > src = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };
			Matrix< int > dst = { { 9,8,7 }, { 6,5,4 }, { 3,2,1 } };

			dst = src;

			for ( int r = 0; r < 3; r++ ) {
				for ( int c = 0; c < 3; c++ ) {
					Assert::AreEqual( src[r][c], dst[r][c] );
				}
			}
		}

		TEST_METHOD( ScalarMultiplicationAssignOperator ) {
			Matrix< int > src = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };

			src *= 5;
			int* pEl = src.Elements();

			for ( int i = 0; i < 9; i++, pEl++ ) {
				Assert::AreEqual( ( i + 1 ) * 5, *pEl );
			}
		}

		TEST_METHOD( MatrixAdditionAssignOperator ) {
			Matrix< int > src = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };
			Matrix< int > dst = { { 9,8,7 }, { 6,5,4 }, { 3,2,1 } };

			src += dst;
			int* pElS = src.Elements();
			int* pElD = dst.Elements();

			for ( int i = 0; i < 9; i++, pElS++, pElD++ ) {
				Assert::AreEqual( 10, *pElS );
				Assert::AreEqual( 9 - i, *pElD );
			}
		}

		TEST_METHOD( MatrixAdditionAssignOperatorThrows ) {
			Matrix< int > a( 3, 3 );
			Matrix< int > b( 3, 2 );
			Matrix< int > c( 12, 3 );
			Matrix< int > d( 243, 122 );

			TestThrowsException<InvalidDimensionsException>( [&] {
				a += b;
			} );

			TestThrowsException<InvalidDimensionsException>( [&] {
				a += c;
			} );

			TestThrowsException<InvalidDimensionsException>( [&] {
				a += d;
			} );
		}

		TEST_METHOD( MatrixSubtractAssignOperator ) {
			Matrix< int > src = { { 10,9,8 }, { 7,6,5 }, { 4,3,2 } };
			Matrix< int > dst = { { 9,8,7 }, { 6,5,4 }, { 3,2,1 } };

			src -= dst;
			int* pElS = src.Elements();
			int* pElD = dst.Elements();

			for ( int i = 0; i < 9; i++, pElS++, pElD++ ) {
				Assert::AreEqual( 1, *pElS );
				Assert::AreEqual( 9 - i, *pElD );
			}
		}

		TEST_METHOD( MatrixSubtractAssignOperatorThrows ) {
			Matrix< int > a( 3, 3 );
			Matrix< int > b( 3, 2 );
			Matrix< int > c( 12, 3 );
			Matrix< int > d( 243, 122 );

			TestThrowsException<InvalidDimensionsException>( [&] {
				a -= b;
			} );

			TestThrowsException<InvalidDimensionsException>( [&] {
				a -= c;
			} );

			TestThrowsException<InvalidDimensionsException>( [&] {
				a -= d;
			} );
		}

		TEST_METHOD( MatrixAddBig ) {
			Matrix < int > b1( 500, 500 );
			Matrix < int > b2( 500, 500 );

			int* pB1 = b1.Elements();
			int* pB2 = b2.Elements();

			for ( int i = 0; i < 250000; i++, pB1++, pB2++ ) {
				*pB1 = i;
				*pB2 = 250000 - i;
			}

			b1 += b2;

			pB1 = b1.Elements();

			for ( int i = 0; i < 250000; i++, pB1++ ) {
				Assert::AreEqual( 250000, *pB1 );
			}
		}

		TEST_METHOD( DiagMultiply ) {
			Matrix< int > m = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
			int mulNum = 14;

			m.MultiplyDiag( mulNum );

			for ( uint r = 0; r < 4; r++ ) {
				for ( uint c = 0; c < 4; c++ ) {
					int expected = 4 * r + c + 1;
					if ( r == c ) {
						expected *= mulNum;
					}

					Assert::AreEqual( expected, m[r][c] );
				}
			}
		}

		TEST_METHOD( DiagMultiplyNonSquare ) {
			Matrix< int > m = { { 1,2,3,4 }, { 9,10,11,12 }, { 13,14,15,16 } };
			Matrix< float > m2 = { { 1,2,3 }, { 9,10,11 }, { 13,14,15 }, {543,2,12}, {475,323,2343} };

			TestThrowsException<InvalidDimensionsException>( [&m] {
				m.MultiplyDiag( 24 );
			} );

			TestThrowsException<InvalidDimensionsException>( [&m2] {
				m2.MultiplyDiag( 54.9654f );
			} );
		}

	};

}