#pragma once

namespace Bundler { namespace Import {

	class IBundleImportWorker
	{
	public:

		static constexpr const char* ExpectedHeader() {
			return "UNKNOWN";
		}

		virtual bool SupportsImageList() const = 0;

		virtual HRESULT Parse(
			__in std::istream* pBundleStream,
			__out Bundle* pBundle,
			__out_opt BundleAdditionalPayload* pAdditionalData ) = 0;

		virtual HRESULT Parse(
			__in std::istream* pBundleStream,
			__in std::istream* pImageListStream,
			__out Bundle* pBundle,
			__out_opt BundleAdditionalPayload* pAdditionalData ) = 0;

	};

} }