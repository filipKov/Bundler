#include "stdafx.h"
#include "AMPUtils.h"

namespace Bundler { namespace Async {

	uint GetAcceleratorCount( )
	{
		return uint( accelerator::get_all( ).size( ) );
	}

	bool IsGPU( __in const accelerator acc )
	{
		return ( !acc.is_emulated ) && ( acc.device_path.compare( accelerator::direct3d_warp ) != 0 );
	}

	void GetAccelerators( __in const uint count, __out accelerator* pAccelerators )
	{
		_ASSERT_EXPR( count == GetAcceleratorCount( ), "accelerator count does not match" );

		std::vector< accelerator > accelerators = accelerator::get_all( );
		AssignCopy( accelerators.data( ), count, pAccelerators );
	}

} }