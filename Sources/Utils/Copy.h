#pragma once

#include "TypeUtils.h"

namespace Utils {	
	
	//	memcpy wrapper
	template < typename T > 
	__forceinline void ShallowCopy( __in_ecount( count ) const T *pSrc, __in const size_t count, __out_ecount( count ) T* pDst ) restrict(cpu) {
		memcpy( pDst, pSrc, count * sizeof( T ) );
	};

	// loop assign copy
	template < typename T > 
	__forceinline void AssignCopy( __in_ecount( count ) const T *pSrc, __in const size_t count, __out_ecount( count ) T* pDst ) restrict(cpu) {
		for ( size_t i = 0; i < count; i++ ) {
			*pDst = *pSrc;
			pDst++;
			pSrc++;
		}
	};

	// memmove wrapper
	template < typename T > 
	__forceinline void ShallowCopySafe( __in_ecount( count ) const T *pSrc, __in const size_t count, __out_ecount( count ) T* pDst ) restrict(cpu) {
		memmove( pDst, pSrc, count * sizeof( T ) );
	};

	// memset wrapper
	template < typename T >
	__forceinline void ByteFill( __in const byte value, __in const size_t count, __inout_ecount( count ) T* pDst ) restrict(cpu) {
		memset( pDst, value, count * sizeof(T) );
	}

	// loop assign fill
	template < typename T >
	__forceinline void AssignFill( __in DefaultItemType< T > value, __in const size_t count, __inout_ecount( count ) T* pDst ) restrict(cpu) {
		for ( size_t i = 0; i < count; i++ ) {
			*pDst = value;
			pDst++;
		}
	};

}