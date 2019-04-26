#pragma once

namespace Bundler { namespace Preprocess {

	void Normalize( __inout Bundle* pBundle, __out_ecount_opt( 3 ) Scalar* pMean, __out_opt Scalar* pStdev );

} }
