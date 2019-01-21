#pragma once

namespace BundlerTest {

	const int RANDOM_TEST_MAX_LOOPS = 100; // 1000;

	template < typename ExpectedException, typename Callable, typename ... ArgsTypes >
	void TestThrowsException( Callable callable, ArgsTypes ... args ) {
		bool thrown = false;
		try {
			callable( args ... );
		}
		catch ( ExpectedException e ) {
			thrown = true;
		}
		catch ( ... ) {
			Assert::IsTrue( false, L"Unexpected exception occured." );
		}

		Assert::IsTrue( thrown );
	}

	template < typename T >
	inline void TestIsInRange( const T& value, const T& refValue, const T& tolerance ) {
		T delta = abs( refValue - value );
		wchar_t deltaStr[256];
		swprintf_s( deltaStr, 256, L"Difference was too big: %f", delta );

		Assert::IsTrue( abs( refValue - value ) <= tolerance, deltaStr );
	}

	template < typename T >
	inline void AssertAreEqual( __in const size_t count, __in const T* pExpectedValues, __in const T* pActualValues ) {
		for ( size_t i = 0; i < count; i++ ) {
			Assert::AreEqual( *pExpectedValues, *pActualValues );
			pExpectedValues++;
			pActualValues++;
		}
	}

	template < typename T >
	inline void AssertAreEqual( __in const size_t count, __in const T* pExpectedValues, __in const T* pActualValues, __in const T tolerance ) {
		for ( size_t i = 0; i < count; i++ ) {
			Assert::AreEqual( *pExpectedValues, *pActualValues, tolerance );
			pExpectedValues++;
			pActualValues++;
		}
	}


}