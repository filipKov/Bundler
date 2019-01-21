#pragma once

#include "stdafx.h"

namespace Utils { namespace Containers {

	template < size_t pageSizeExponent = 16, template < typename > class Allocator = MemoryAllocator >
	class MemoryHeap {
	protected:

		using PageAllocator = MemoryAllocator< byte * >;
		using ItemAllocator = MemoryAllocator< byte >;

		static const size_t pageSize = 1 << pageSizeExponent;
		static const size_t pageSizeMask = pageSize - 1;

	public:

		MemoryHeap() {
			m_capacity = 0;
			m_offset = 0;
			m_pages = NULL;
		};

		~MemoryHeap() {
			Clear();
		}

		void Allocate( __in const size_t size, __deref_out void** ppDestination ) {
			_ASSERT_EXPR( size > 0, L"cannot allocate 0 bytes" );

			if ( size > pageSize ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( size ), "requested memory is too big" );
			}

			size_t requiredCapacity = m_offset + size;

			size_t pageIndex = m_offset >> pageSizeExponent;
			size_t pageOffset = m_offset & pageSizeMask;
			if ( ( pageOffset + size ) > pageSize ) {
				requiredCapacity += pageSize - pageOffset;
				pageOffset = 0;
				pageIndex++;
			}

			EnsureCapacity( requiredCapacity );
			m_offset = requiredCapacity;

			*ppDestination = (void*)( m_pages[pageIndex] + pageOffset );
		}


		template< typename T >
		T* AllocateArray( __in const size_t elementCount ) {
			_ASSERT_EXPR( elementCount > 0, L"cannot allocate 0 elements" );

			T* pTempDst = NULL;
			AllocateRaw( elementCount * sizeof( T ), (void**)( &pTempDst ) );
			return pTempDst;
		}

		void Clear() {
			byte** pages = m_pages;
			const size_t pageCount = m_capacity >> pageSizeExponent;

			m_pages = NULL;
			m_capacity = 0;
			m_offset = 0;

			if ( pages ) {
				for ( uint i = 0; i < pageCount; i++ ) {
					ItemAllocator::Release( pages[i] );
				}

				PageAllocator::Release( pages );
			}
		}

	protected:

		void EnsureCapacity( __in const size_t requestedCapacity ) {
			if ( m_capacity < requestedCapacity ) {
				_ASSERT( ( requestedCapacity - m_capacity ) <= pageSize );
				GrowOne();
			}
		}

		void GrowOne() {
			const size_t pageCount = m_capacity >> pageSizeExponent;

			byte** oldPages = m_pages;
			m_pages = PageAllocator::Allocate( pageCount + 1 );

			if ( oldPages ) {
				ShallowCopy( oldPages, pageCount, m_pages );
				PageAllocator::Release( oldPages );
			}

			m_pages[pageCount] = ItemAllocator::Allocate( pageSize );
			m_capacity += pageSize;
		}

	protected:

		byte** m_pages;

		size_t m_offset;
		size_t m_capacity;

	};

} }