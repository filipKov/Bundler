#pragma once

namespace Bundler { namespace Import {

	class BundleImporter
	{
	public:
		
		static HRESULT Import( 
			__in std::istream* pBundleStream, 
			__out Bundler::Bundle* pBundle, 
			__out_opt Bundler::BundleAdditionalPayload* pAdditionalData );

		static HRESULT Import(
			__in std::istream* pBundleStream,
			__in std::istream* pImageListStream,
			__out Bundler::Bundle* pBundle,
			__out_opt Bundler::BundleAdditionalPayload* pAdditionalData );

		static HRESULT Import( 
			__in_z const char* pFilename, 
			__out Bundler::Bundle* pBundle, 
			__out_opt Bundler::BundleAdditionalPayload* pAdditionalData );

	protected:

		static void GetImageListPath( __in_z const char* pBundlePath, __out_ecount( 512 ) char* pImListPath );

		static HRESULT ParseHeader( __in std::istream* pStream, __out_z char( &header )[ 256 ] );

		static HRESULT SelectImporter( __in_z const char* pHeader, __deref_out IBundleImportWorker** ppImporter );


	};

} }
