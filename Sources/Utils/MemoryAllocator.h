#pragma once

#include <stdlib.h>

namespace Utils {

	namespace Internal {

	#pragma push_macro("new")
	#pragma push_macro("delete")

	#undef new
	#undef delete

	template < typename T > class MemoryAllocatorImplementation {
	private:
		struct AllocationHelper {
			T o;

			void* operator new ( size_t size )
			{
				return malloc ( size );
			}

			void* operator new[]( size_t size )
			{
				return malloc ( size );
			}

			void operator delete ( void *ptr )
			{
				free ( ptr );
			}

			void operator delete[]( void *ptr )
			{
				free ( ptr );
			}
		};

	public:

		static T* Allocate ( size_t size )
		{
			T* pMem = (T*)( new AllocationHelper[size] );
			if ( !pMem ) {
				throw OutOfMemoryException( size * sizeof( T ) );
			}

			return pMem;
		}

		static T* AllocateItem ()
		{
			T* pMem = (T*)( new AllocationHelper );
			if ( !pMem ) {
				throw OutOfMemoryException( sizeof( T ) );
			}
		
			return pMem;
		}

		// static HRESULT Allocate( size_t size, __deref_out_ecount( size ) T** ppItems )
		// {
		// 	*ppItems = (T*)( new AllocationHelper[size] );
		// 	if ( !(*ppItems) ) {
		// 		return E_OUTOFMEMORY;
		// 	}
		// 	return S_OK;
		// }
		// 
		// static HRESULT AllocateItem( __deref_out T** ppItem )
		// {
		// 	*ppItem = (T*)( new AllocationHelper );
		// 	if ( !(*ppItem) ) {
		// 		return E_OUTOFMEMORY;
		// 	}
		// 	return S_OK;
		// }

		static void Release ( T* buffer )
		{
			delete[] ( AllocationHelper* )( buffer );
		}

		static void ReleaseItem ( T* pItem )
		{
			delete ( AllocationHelper* )( pItem );
		}

	};

	#pragma pop_macro("new")
	#pragma pop_macro("delete")

	}

	template < typename T > class MemoryAllocator : public Internal::MemoryAllocatorImplementation< T > {};
}
