#pragma once

namespace Bundler { namespace Preprocess {

	void GetMean( __in const Bundle* pBundle, __out_ecount( 3 ) double* pMean );

	void GetStdev( __in const Bundle* pBundle, __in_ecount_opt( 3 ) double* pMean, __out double* pStdev );	

	void Normalize( 
		__in const Bundle* pBundle, 
		__out Bundle* pBundleOut,
		__out_ecount_opt( 3 ) Scalar* pMean, 
		__out_opt Scalar* pStdev );

} }
