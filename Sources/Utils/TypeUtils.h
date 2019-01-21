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

}