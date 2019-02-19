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

} }