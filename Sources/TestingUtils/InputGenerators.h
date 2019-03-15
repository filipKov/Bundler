#pragma once

namespace TestingUtils { 

	template < typename T >
	struct Random
	{
		inline static void Seed( uint seed )
		{
			srand( seed );
		}

		inline static void Seed()
		{
			Seed( (uint)time( NULL ) );
		}

		inline static T Value( __in const T minN, __in const T maxN )
		{
			return ( T( rand() ) / RAND_MAX ) * ( maxN - minN ) + minN;
		}

		inline static T Value()
		{
			return Value( T( 0 ), T( 1 ) );
		}

		inline static void Fill( __in const T minVal, __in const T maxVal, __in const size_t cnt, __out_ecount( cnt ) T* pDst )
		{
			for ( size_t i = 0; i < cnt; i++ )
			{
				*pDst = Value( minVal, maxVal );
				pDst++;
			}
		}

		inline static void Fill( __in const size_t cnt, __out_ecount( cnt ) T* pDst )
		{
			Fill( T( 0 ), T( 1 ), cnt, pDst );
		}

		inline static void Matrix( __in const T minVal, __in const T maxVal, __in const size_t m, __in const size_t n, __out_ecount( m * n ) T* M )
		{
			Fill( minVal, maxVal, m * n, M );
		}

		inline static void Matrix( __in const size_t m, __in const size_t n, __out_ecount( m * n ) T* M )
		{
			Fill( T(0), T(1), m * n, M );
		}

	};

} 
