#pragma once

namespace Bundler { namespace Preprocess {
	
	struct FilteringStatistics
	{
		size_t pointsBefore;
		size_t pointsAfter;

		size_t projectionsBefore;
		size_t projectionsAfter;
	};

	void FilterAroundCenter(
		__in const Bundle* pBundle,
		__in_ecount( 3 ) const Scalar* pCenter,
		__in const Scalar radius,
		__out Bundle* pOutBundle,
		__in_opt const BundleAdditionalPayload* pMedatada,
		__out_opt BundleAdditionalPayload* pOutMedatada,
		__out_opt FilteringStatistics* pStats );

} }
