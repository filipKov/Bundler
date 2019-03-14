#pragma once

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

template < typename T, size_t N >
inline void AssertAreEqual( __in const T( &expected )[ N ], __in const T* actual, __in const T tolerance = T( 0 ) ) {
	const T* pExpected = expected;
	for ( size_t i = 0; i < N; i++ ) {
		Assert::AreEqual( *pExpected, *actual, tolerance );
		pExpected++;
		actual++;
	}
}

template < typename T >
inline void AssertAreEqual( __in const size_t N, __in const T* expected, __in const T* actual, __in const T tolerance = T( 0 ) ) {
	for ( size_t i = 0; i < N; i++ ) {
		Assert::AreEqual( *expected, *actual, tolerance );
		expected++;
		actual++;
	}
}

template < typename T >
inline T Random( __in const T minN, __in const T maxN )
{
	return ( T( rand() ) / RAND_MAX ) * ( maxN - minN ) + minN;
}

template < typename T >
inline T Random()
{
	return Random( T( 0 ), T( 1 ) );
}

template < typename T >
inline void RandomFill( __in const size_t cnt, __out_ecount( cnt ) T* pDst )
{
	for ( size_t i = 0; i < cnt; i++ ) 
	{
		*pDst = Random< T >();
		pDst++;
	}
}