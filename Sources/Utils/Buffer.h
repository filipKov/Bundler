#pragma once

#include "stdafx.h"

namespace Utils { namespace Containers {

	/*
		Static array wrapper class ( no run-time index guarding, for performance sake )
	*/
	template < class T, class Allocator = MemoryAllocator<T> > 
	class Buffer {
	protected:

		using ItemType = typename DefaultItemType< T >;

	public:

		Buffer() {
			m_size = 0;
			m_pItems = NULL;
		};

		Buffer( __in const Buffer<T>& other ) : Buffer() {
			SetCopy( other );
		};

		~Buffer() {
			Clear();
		};

		Buffer& operator=( __in const Buffer< T >& source )
		{
			SetCopy( source );
			return *this;
		}

		// Capacity

		size_t Length() const {
			return m_size;
		}

		void Allocate( __in const size_t length ) {
			Clear();

			m_pItems = Allocator::Allocate( length );
			m_size = length;
		}

		template < bool keepData = false >
		void EnsureLength( __in const size_t length ) {
			if ( keepData ) {
				EnsureLengthKeepData( length );
			}
			else {
				EnsureLengthTossData( length );
			}
		}

		void RestrictLength( __in const size_t newLength ) {
			_ASSERT_EXPR( newLength <= m_size, L"Can't restrict to longer length" );
			
			for ( size_t i = newLength; i < m_size; i++ ) {
				ItemReleaser< T >::Release( m_pItems[i] );
			}

			m_size = newLength;
		}

		// Content

		const T& operator[] ( __in const size_t pos ) const {
			_ASSERT_EXPR( pos < m_size, L"Index is out of range" );
			return *( m_pItems + pos );
		}

		T& operator[] ( __in const size_t pos ) {
			_ASSERT_EXPR( pos < m_size, L"Index is out of range" );
			return *( m_pItems + pos );
		}

		const T* Data() const {
			return m_pItems;
		}

		T* Data() {
			return m_pItems;
		}

		void Fill( __in ItemType value ) {
			T *pDst = m_pItems;
			for ( size_t i = 0; i < m_size; i++ ) {
				*pDst = value;
				pDst++;
			}
		}

		void SetCopy( __in const size_t count, __in_ecount( count ) const T *pSrc ) {
			if ( pSrc == NULL ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( pSrc ), ExceptionReasons::NULLPTR );
			}

			EnsureLength< false >( count );
			AssignCopy< T >( pSrc, count, m_pItems );
			RestrictLength( count );
		}

		template < class Container >
		void SetCopy( __in const Container& source ) {
			const size_t newLength = source.Length();
			EnsureLength< false >( newLength );

			for ( size_t i = 0; i < newLength; i++ ) {
				m_pItems[i] = source[i];
			}

			RestrictLength( newLength );
		}

		void Clear() {
			// Detach, then release

			T* pTemp = m_pItems;
			m_pItems = NULL;
			m_size = 0;

			if ( pTemp ) {
				Allocator::Release( pTemp );
			}
		}

	protected:

		void EnsureLengthKeepData( __in const size_t length ) {
			if ( m_size < length ) {
				T* pTemp = m_pItems;
				m_pItems = Allocator::Allocate( length );

				if ( pTemp != NULL ) {
					SetCopy( m_size, pTemp );
					Allocator::Release( pTemp );
				}

				m_size = length;
			}
		}

		void EnsureLengthTossData( __in const size_t length ) {
			if ( m_size < length ) {
				Allocate( length );
			}
		}

	protected:

		size_t	m_size;
		T*		m_pItems;

	};

} }