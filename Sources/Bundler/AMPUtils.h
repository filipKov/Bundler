#pragma once

namespace Bundler { namespace Async {

	uint GetAcceleratorCount( );

	template < typename Predicate >
	uint GetAcceleratorCount( __in Predicate pred )
	{
		std::vector < accelerator > accelerators = accelerator::get_all( );
		uint count = 0;
		for ( auto it = accelerators.begin( ); it != accelerators.end( ); it++ )
		{
			if ( pred( *it ) )
			{
				count++;
			}
		}
		return count;
	}

	bool IsGPU( __in const accelerator acc );

	void GetAccelerators( __in const uint count, __out accelerator* pAccelerators );

	template < typename Predicate >
	void GetAccelerators( __in const uint count, __out accelerator* pAccelerators, __in Predicate pred )
	{
		_ASSERT_EXPR( count == GetAcceleratorCount( pred ), "accelerator count does not match" );
		
		std::vector < accelerator > accelerators = accelerator::get_all( );

		for ( auto it = accelerators.begin( ); it != accelerators.end( ); it++ )
		{
			if ( pred( *it ) )
			{
				*pAccelerators = *it;
				pAccelerators++;
			}
		}
	}

	template < typename Predicate >
	void ListAccelerators( __in Predicate pred )
	{
		std::vector < accelerator > accelerators = accelerator::get_all( );

		for ( auto it = accelerators.begin( ); it != accelerators.end( ); it++ )
		{
			if ( pred( *it ) )
			{
				wprintf_s( L"Accelerator: %s\n", it->get_description( ).c_str( ) );
				wprintf_s( L"\tDevice path: %s\n", it->get_device_path( ).c_str( ) );
				wprintf_s( L"\tDedicated memory: %zuKB\n", it->get_dedicated_memory( ) );
				wprintf_s( L"\tHas attached display: %s\n", it->get_has_display( ) ? L"True" : L"False" );
				wprintf_s( L"\tIs emulated: %s\n", it->get_is_emulated( ) ? L"True" : L"False" );
				wprintf_s( L"\tSupports shared memory: %s\n", it->get_supports_cpu_shared_memory( ) ? L"True" : L"False" );
				wprintf_s( L"\tSupports double precision: %s\n", it->get_supports_double_precision( ) ? L"True" : L"False" );
				wprintf_s( L"\tSupports limited double precision: %s\n", it->get_supports_limited_double_precision( ) ? L"True" : L"False" );
				wprintf_s( L"\tVersion: %u\n", it->get_version( ) );
				wprintf_s( L"\n" );
			}
		}
	}

} }