#pragma once

namespace Bundler { namespace Async {

	template < typename T >
	class InterlockedVariable : std::atomic< T >
	{
	public:

		InterlockedVariable( ) { };

		InterlockedVariable( __in const T value )
		{
			SetValue( value );
		}

		InterlockedVariable( __in const InterlockedVariable< T >& other ) = delete; // no copying
		InterlockedVariable< T >& operator=( __in const InterlockedVariable< T >& other ) = delete; // no copying
		
		__forceinline void operator=( __in const T value )
		{
			store( value, std::memory_order_seq_cst );
		}
		
		__forceinline void SetValue( __in const T value )
		{
			store( value, std::memory_order_seq_cst );
		}

		__forceinline T GetValue( ) const
		{
			return load( std::memory_order_seq_cst );
		}

		__forceinline operator T ( ) const
		{
			return GetValue( );
		}
		
		void operator+= ( __in const T value ) noexcept
		{
			fetch_add( value, std::memory_order_relaxed );
		}

		void operator-= ( __in const T value ) noexcept
		{
			fetch_sub( value, std::memory_order_relaxed );
		}

	};

} }