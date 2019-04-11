#pragma once

namespace Utils { namespace Containers {

	// template< typename T >
	// __forceinline T& GetAt( __in T* pArray, __in const size_t ix ) {
	// 	return *( pArray + ix );
	// }
	// 
	// template< typename T >
	// __forceinline const T& GetAt( __in const T* pArray, __in const size_t ix ) {
	// 	return *( pArray + ix );
	// }

	#define ELEMENT( arrayPtr, index ) (*( arrayPtr + index ))

	template < typename T >
	struct ArrayUtils
	{

		static __forceinline void Copy( __in const T* pSrc, __in const uint count, __out T* pDst ) restrict( amp, cpu )
		{
			for ( uint i = 0; i < count; i++ )
			{
				*pDst = *pSrc;
				pDst++;
				pSrc++;
			}
		}

		template < uint count >
		static __forceinline void Copy( __in const T* pSrc, __out T* pDst ) restrict( amp, cpu )
		{
			for ( uint i = 0; i < count; i++ )
			{
				*pDst = *pSrc;
				pDst++;
				pSrc++;
			}
		}

		template < uint count >
		static __forceinline void Fill( __in const DefaultItemType< T > value, __out T* pDst ) restrict( amp, cpu )
		{
			for ( uint i = 0; i < count; i++ )
			{
				*pDst = value;
				pDst++;
			}
		}

		static __forceinline void Fill( __in const DefaultItemType< T > value, __in const uint count, __out T* pDst ) restrict( amp, cpu )
		{
			for ( uint i = 0; i < count; i++ )
			{
				*pDst = value;
				pDst++;
			}
		}

		static __forceinline T Sum( __in const uint count, __in_ecount( count ) const T* pValues )
		{
			T sum = T( 0 );
			for ( uint i = 0; i < count; i++ )
			{
				sum += *pValues;
				pValues++;
			}
			return sum;
		}
	};

} }