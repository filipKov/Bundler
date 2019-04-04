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
		_ASSERT_EXPR( count == GetAcceleratorCount< Predicate >( ), "accelerator count does not match" );
		
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

} }