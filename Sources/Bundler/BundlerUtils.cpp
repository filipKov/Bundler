#include "stdafx.h"
#include "BundlerUtils.h"

namespace Bundler { namespace Utils {

	void CopyBundle( __in const Bundle* pInBundle, __out Bundle* pOutBundle )
	{
		pOutBundle->cameras.SetCopy( pInBundle->cameras );
		pOutBundle->points.SetCopy( pInBundle->points );
		pOutBundle->projections.SetCopy( pInBundle->projections );
	}

} }
