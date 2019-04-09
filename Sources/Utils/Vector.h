#pragma once

#include "stdafx.h"

namespace Utils { namespace Containers {

	template < typename T, class Allocator = MemoryAllocator<T> >
	class Vector {
	protected:

		using ItemType = typename TypeResolver< T, sizeof( T ) >= 2 * sizeof( void* ) || __alignof( T ) >= 2 * sizeof( void* ) >::ItemType;

	public:

		Vector() {
			m_capacity = 0;
			m_size = 0;
			m_pItems = NULL;
		}
		
		Vector( __in const Vector<T>& other ) : Vector() {
			SetCopy( other );
		}

		~Vector() {
			Clear();
		}

		Vector& operator= ( __in const Vector<T>& other )
		{
			SetCopy( other );
			return *this;
		}
		
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
			return m_size;
		}
		

		void EnsureLength( __in const size_t requestedSize ) {
			if ( m_size < requestedSize ) {
				EnsureCapacity( requestedSize );
				m_size = requestedSize;
			}
		}

		void RestrictLength( __in const size_t newLength ) {
			_ASSERT_EXPR( newLength <= m_size, L"Can't restrict to longer length" );

			for ( size_t i = newLength; i < m_size; i++ ) {
				ItemReleaser< T >::Release( m_pItems[i] );
			}

			m_size = newLength;
		}

		void ShrinkToLength() {
			if ( m_size < m_capacity ) {
				T* pTemp = m_pItems;

				m_pItems = Allocator::Allocate( m_size );
				m_capacity = m_size;
				AssignCopy< T >( pTemp, m_size, m_pItems );

				if ( pTemp != NULL ) {
					Allocator::Release( pTemp );
				}
			}
		}

		// Content
		
		const T& operator[] ( __in const size_t index ) const {
			_ASSERT_EXPR( index < m_size, L"Index is out of range" );
			return *( m_pItems + index );
		}

		T& operator[] ( __in const size_t index ) {
			_ASSERT_EXPR( index < m_size, L"Index is out of range" );
			return *( m_pItems + index );
		}

		T& GetLast( )
		{
			const size_t index = ( m_length - 1 );
			return *( m_pItems + index );
		}

		const T* Data() const {
			return m_pItems;
		}

		T* Data() {
			return m_pItems;
		}

		void Add( __in ItemType element ) {
			EnsureCapacity( m_size + 1 );
			m_pItems[m_size++] = element;
		}

		void Add( __in const size_t count, __in_ecount( count ) const T* pArray ) {
			EnsureCapacity( m_size + count );

			for ( size_t i = 0; i < count; i++ ) {
				m_pItems[m_size++] = *pArray;
				pArray++;
			}
		}
 
		void AddAt( __in const size_t index, __in ItemType value ) {
			_ASSERT_EXPR( index < m_size, L"Index is out of range " );
			
			EnsureCapacity( m_size + 1 );
			m_pItems[m_size] = m_pItems[index];
			m_pItems[index] = value;
			m_size++;
		}

		T RemoveLast() {
			T element = m_pItems[--m_size];
			ItemReleaser< T >::Release( m_pItems[m_size] );
			return element;
		}

		T RemoveAt( __in const size_t index ) {
			T removedElement = m_pItems[index];

			m_pItems[index] = m_pItems[--m_size];
			ItemReleaser< T >::Release( m_pItems[m_size] );
			return removedElement;
		}

		void Fill( __in ItemType value ) {
			T* pDst = m_pItems;
			for ( size_t i = 0; i < m_size; i++ ) {
				*pDst = value;
				pDst++;
			}
		}

		void SetCopy( __in const size_t count, __in_ecount( count ) const T* pArray ) {
			if ( pArray == NULL ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( pArray ), ExceptionReasons::NULLPTR );
			}

			EnsureLength( count );
			AssignCopy< T >( pArray, count, m_pItems );
			RestrictLength( count );
		}

		template < typename Container >
		void SetCopy( __in const Container& source ) {
			const size_t newLength = source.Length();
			EnsureCapacity( newLength );

			for ( size_t i = 0; i < newLength; i++ ) {
				m_pItems[i] = source[i];
			}

			RestrictLength( newLength );
		}

		void Clear() {
			T* pTemp = m_pItems;

			m_pItems = NULL;
			m_capacity = 0;
			m_size = 0;

			if ( pTemp ) {
				Allocator::Release( pTemp );
			}
		}

	protected:

		void GrowTo( __in const size_t minCapacity ) {
			m_capacity += m_capacity;
			if ( m_capacity < minCapacity ) {
				m_capacity = 4;
			}

			while ( m_capacity < minCapacity ) {
				m_capacity += m_capacity;
			}

			T* pTemp = m_pItems;
			m_pItems = Allocator::Allocate( m_capacity );

			if ( pTemp != NULL ) {
				AssignCopy< T >( pTemp, m_size, m_pItems );
				Allocator::Release( pTemp );
			}
		}

	protected:

		T* m_pItems;
		size_t m_capacity;
		size_t m_size;

	};

} }