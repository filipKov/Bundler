#include "stdafx.h"

namespace NumericOptimizationTest {

	TEST_CLASS( VectorDynamicBaseTest ) {

		TEST_METHOD( DimensionConstructor ) {
			Vector<int> base( 13 );

			int* pEl = base.Elements();

			Assert::AreEqual( 13u, base.GetDimension() );
			for ( int i = 0; i < 13; i++, pEl++ ) {
				Assert::AreEqual( 0, *pEl );
			}
		}

		TEST_METHOD( InitializerListConstructor ) {
			Vector<int> base = { 1,2,3,4,5,6 };
			
			int* pEl = base.Elements();

			Assert::AreEqual( 6u, base.GetDimension() );
			for ( int i = 0; i < 6; i++, pEl++ ) {
				Assert::AreEqual( i+1, *pEl );
			}
		}

		TEST_METHOD( CopyConstructor ) {
			Vector<int> base = { 1,2,3,4,5,6 };
			auto base2( base );

			int* pEl1 = base.Elements();
			int* pEl2 = base2.Elements();

			Assert::AreEqual( base.GetDimension(), base2.GetDimension() );
			for ( int i = 0; i < 6; i++, pEl1++, pEl2++ ) {
				Assert::AreEqual( *pEl1, *pEl2 );
			}
		}

		TEST_METHOD( MoveConstructor ) {
			Vector<int> base = { 1,2,3,4,5,6 };
			auto base2( std::move( base ) );

			Assert::AreEqual( 6u, base2.GetDimension() );
			Assert::AreEqual( 0u, base.GetDimension() );
			Assert::IsNull( base.Elements() );

			int* pEl = base2.Elements();
			for ( int i = 0; i < 6; i++, pEl++ ) {
				Assert::AreEqual( i + 1, *pEl );
			}
		}

		TEST_METHOD( AccessOperator ) {
			Vector<int> base = { 1,2,3 };

			Assert::AreEqual( 1, base[0] );
			Assert::AreEqual( 2, base[1] );
			Assert::AreEqual( 3, base[2] );
		}

		TEST_METHOD( AccessOperatorThrows ) {
			Vector<int> base = { 1,2,3 };

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base[4];
			} );

			TestThrowsException<IndexOutOfRangeException>( [&base] {
				base[133246];
			} );
		}

		TEST_METHOD( SetTo ) {
			Vector<int> base = { 1,2,3 };
			Containers::Buffer<int> buff;
			buff.Allocate( 5 );
			buff[0] = 12;
			buff[1] = 15;
			buff[2] = 11;
			buff[3] = 13;
			buff[4] = 14;

			base.SetTo( (uint)buff.Length(), buff.Data() );

			Assert::AreEqual( (uint)buff.Length(), base.GetDimension() );
			
			for ( int i = 0; i < 5; i++ ) {
				Assert::AreEqual( buff[i], base[i] );
			}
		}

		TEST_METHOD( AssignOperator ) {
			Vector<int> base = { 1,2,3 };
			Vector<int> base2 = { 5,6,7,8 };
			base = base2;

			Assert::AreEqual( base2.GetDimension(), base.GetDimension() );
			for ( int i = 0; i < 4; i++ ) {
				Assert::AreEqual( base2[i], base[i] );
			}
		}

		TEST_METHOD( DotProduct ) {
			Vector<int> base = { 1,2,3,4 };
			Vector<int> base2 = { 5,4,3,2 };

			int dot = base.Dot( base2 );

			Assert::AreEqual( 5 + 8 + 9 + 8, dot );
		}

		TEST_METHOD( DotProductThrows ) {
			Vector<int> base = { 1,2,3 };
			Vector<int> base2 = { 1,2,3,4 };
			Vector<int> base3 = { 1,2 };

			TestThrowsException<InvalidDimensionsException>( [&] {
				base.Dot( base2 );
			} );

			TestThrowsException<InvalidDimensionsException>( [&] {
				base.Dot( base3 );
			} );
		}

		TEST_METHOD( Length ) {
			Vector<int> base = { 1,2,3,4 };

			int len = base.Length();

			Assert::AreEqual( (int)sqrt( 1 + 4 + 9 + 16 ), len );
		}
	};

}