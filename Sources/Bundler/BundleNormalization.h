#pragma once

namespace Bundler { namespace Preprocess {

	void Normalize( 
		__in const Bundle* pBundle, 
		__out Bundle* pBundleOut,
		__out_ecount_opt( 3 ) Scalar* pMean, 
		__out_opt Scalar* pStdev );

} }
