#pragma once

namespace Utils {

	template < typename T, bool isBig >
	struct TypeResolver;

	template < typename T >
	struct TypeResolver< T, false > {
		using ItemType = typename T;
	};

	template < typename T >
	struct TypeResolver< T, true > {
		using ItemType = typename const T&;
	};

	template < typename T >
	using DefaultItemType = typename TypeResolver< T, ( sizeof( T ) >= ( 2 * sizeof( void * ) ) ) || ( __alignof( T ) >= ( 2 * sizeof( void * ) ) ) >::ItemType;

	template < typename T >
	struct ItemReleaser {
		static void Release( T item ) {
			UNREFERENCED_PARAMETER( item );
		}
	};

	template < typename T >
	void Swap( __inout T& a, __inout T& b )
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

	template < typename T >
	void SwapInPlace( __inout T& a, __inout T& b )
	{
		( a ^ b ) && ( a ^= b; b ^= a; a ^= b );
	}

}