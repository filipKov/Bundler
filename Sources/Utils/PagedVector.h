#pragma once

#include "stdafx.h"

namespace Utils { namespace Containers {

	template < typename T, size_t pageSizeExponent = 10, template < typename > class Allocator = MemoryAllocator >
	class PagedVector {
	protected:

		using ItemType = typename TypeResolver< T, sizeof( T ) >= 2 * sizeof( void* ) || __alignof( T ) >= 2 * sizeof( void* ) >::ItemType;

		using ItemAllocator = Allocator< T >;
		using PageAllocator = Allocator< T* >;

		static const size_t pageSize = 1 << pageSizeExponent;
		static const size_t pageSizeMask = pageSize - 1;

	public:

		PagedVector() {
			m_length = 0;
			m_capacity = 0;
			m_pages = NULL;
		}

		PagedVector( __in const PagedVector< T, pageSizeExponent >& source ) {
			SetCopy( source );
		}

		~PagedVector() {
			Clear();
		}

	public:

		// Capacity

		size_t Capacity() const {
			return m_capacity;
		}

		void EnsureCapacity( __in const size_t requestedCapacity ) {
			if ( m_capacity < requestedCapacity ) {
				GrowTo( requestedCapacity );
			}
		}

		size_t Length() const {
			return m_length;
		}

		void EnsureLength( __in const size_t requestedSize ) {
			if ( m_length < requestedSize ) {
				EnsureCapacity( requestedSize );
				m_length = requestedSize;
			}
		}

		void RestrictLength( __in const size_t newLength ) {
			_ASSERT_EXPR( newLength <= m_length, L"Can't restrict to longer length" );
			
			for ( size_t i = newLength; i < m_length; i++ ) {
				ItemReleaser< T >::Release( ( *this )[i] );
			}
			
			m_length = newLength;
		}

		// Content

		T& operator[]( __in const size_t index ) {
			_ASSERT_EXPR( index < m_length, L"Index is out of range" );
			return m_pages[index >> pageSizeExponent][index & pageSizeMask];
		}

		const T& operator[]( __in const size_t index ) const {
			_ASSERT_EXPR( index < m_length, L"Index is out of range" );
			return m_pages[index >> pageSizeExponent][index & pageSizeMask];
		}

		void Add( __in ItemType value ) {
			EnsureCapacity( m_length + 1 );
			m_pages[m_length >> pageSizeExponent][m_length & pageSizeMask] = value;
			m_length++;
		}

		void Add( __in const size_t count, __in_ecount( count ) const T* pArray ) {
			EnsureCapacity( m_length + count );
			
			for ( size_t i = 0; i < count; i++ ) {
				m_pages[m_length >> pageSizeExponent][m_length & pageSizeMask] = *pArray;
				m_length++;
				pArray++;
			}
		}

		void AddAt( __in const size_t index, __in ItemType value ) {
			_ASSERT_EXPR( index < m_length, L"Index is out of range " );

			EnsureCapacity( m_length + 1 );
			m_pages[m_length >> pageSizeExponent][m_length & pageSizeMask] = m_pages[index >> pageSizeExponent][index & pageSizeMask];
			m_pages[index >> pageSizeExponent][index & pageSizeMask] = value;

			m_length++;
		}

		
		T RemoveLast() {
			m_length--;

			T removedElement = m_pages[m_length >> pageSizeExponent][m_length & pageSizeMask];

			ItemReleaser< T >::Release( m_pages[m_length >> pageSizeExponent][m_length & pageSizeMask] );

			return removedElement;
		}

		T RemoveAt( __in const size_t index ) {
			m_length--;

			T removedElement = m_pages[index >> pageSizeExponent][index & pageSizeMask];

			m_pages[index >> pageSizeExponent][index & pageSizeMask] = m_pages[m_length >> pageSizeExponent][m_length & pageSizeMask];
			ItemReleaser<T>::Release( m_pages[m_length >> pageSizeExponent][m_length & pageSizeMask] );

			return removedElement;
		}

		void Fill( __in ItemType value ) {
			for ( size_t i = 0; i < m_length; i++ ) {
				m_pages[i >> pageSizeExponent][i & pageSizeMask] = value;
			}
		}

		void SetCopy( __in const size_t count, __in_ecount( count ) const T* pArray ) {
			if ( pArray == NULL ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( pArray ), ExceptionReasons::NULLPTR );
			}

			EnsureLength( count );

			for ( size_t i = 0; i < count; i++ ) {
				m_pages[i >> pageSizeExponent][i & pageSizeMask] = *pArray;
				pArray++;
			}

			RestrictLength( count );
		}

		template < class Container >
		void SetCopy( __in const Container& source ) {
			const size_t length = source.Length();

			EnsureLength( length );

			for ( size_t i = 0; i < length; i++ ) {
				m_pages[i >> pageSizeExponent][i & pageSizeMask] = source[i];
			}

			RestrictLength( length );
		}

		void Clear() {
			T** pages = m_pages;
			const size_t pageCount = m_capacity >> pageSizeExponent;

			m_pages = NULL;
			m_capacity = 0;
			m_length = 0;

			if ( pages ) {
				for ( size_t pageIx = 0; pageIx < pageCount; pageIx++ ) {
					ItemAllocator::Release( pages[pageIx] );
				}
				PageAllocator::Release( pages );
			}
		}

	protected:

		void GrowTo( __in const size_t requestedCapacity ) {
			const size_t pagesCurrentlyAllocated = m_capacity >> pageSizeExponent;

			while ( m_capacity < requestedCapacity ) {
				m_capacity += pageSize;
			}

			const size_t pagesNeeded = m_capacity >> pageSizeExponent;

			T** oldPages = m_pages;
			m_pages = PageAllocator::Allocate( pagesNeeded );

			if ( oldPages ) {
				ShallowCopy< T* >( oldPages, pagesCurrentlyAllocated, m_pages );
				PageAllocator::Release( oldPages );
			}

			for ( size_t i = pagesCurrentlyAllocated; i < pagesNeeded; i++ ) {
				m_pages[i] = ItemAllocator::Allocate( pageSize );
			}
		}

	protected:

		size_t m_length;
		size_t m_capacity;
		T ** m_pages;

	};

	// void AddAtKeepOrdering( __in const size_t index, __in ItemType value ) {
	// 	_ASSERT_EXPR( index < m_length, L"Index is out of range " );
	// 
	// 	EnsureCapacitySingle();
	// 	for ( size_t i = m_length; i > index; i-- ) {
	// 		m_pages[i >> pageSizeExponent][i & pageSizeMask] = m_pages[( i - 1 ) >> pageSizeExponent][( i - 1 ) & pageSizeMask];
	// 	}
	// 
	// 	m_pages[index >> pageSizeExponent][index & pageSizeMask] = value;
	// 	m_length++;
	// }
	// 
	// void RemoveAtKeepOrdering( __in const size_t index ) {
	// 	m_length--;
	// 	for ( size_t i = index; i < m_length; i++ ) {
	// 		m_pages[i >> pageSizeExponent][i & pageSizeMask] = m_pages[( i + 1 ) >> pageSizeExponent][( i + 1 ) & pageSizeMask];
	// 	}
	// 
	// 	ItemReleaser<T>::Release( m_pages[m_length >> pageSizeExponent][m_length & pageSizeMask] );
	// }

} }