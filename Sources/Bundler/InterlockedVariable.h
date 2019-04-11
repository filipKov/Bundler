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
			AtomicOperation<>( value, Add );
		}

		void operator-= ( __in const T value ) noexcept
		{
			AtomicOperation<>( value, Sub );
		}

	protected:

		template < typename BinaryOp >
		void AtomicOperation( const T value, BinaryOp op ) noexcept
		{
			T old = GetValue( );
			T desired = op( old, value );
			while ( !compare_exchange_weak( old, desired ) )
			{
				desired = op( old, value );
			}
		}

		static T Add( __in const T v1, __in const T v2 ) noexcept
		{
			return v1 + v2;
		}

		static T Sub( __in const T v1, __in const T v2 ) noexcept
		{
			return v1 - v2;
		}
	};

} }