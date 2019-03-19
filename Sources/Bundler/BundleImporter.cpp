#include "stdafx.h"
#include <fstream>

#include <cstring>
#include "TextStreamReader.h"

#include "IBundleImportWorker.h"
#include "SnavelyBundleImportWorker.h"
#include "BundleImporter.h"

using namespace Utils::FileSystem;

namespace Bundler { namespace Import {

#define IMAGE_LIST_EXTENSION ".iml"

	HRESULT BundleImporter::Import(
		__in std::istream* pBundleStream,
		__out Bundler::Bundle* pBundle,
		__out_opt Bundler::BundleAdditionalPayload* pAdditionalData )
	{
		char header[ 256 ];
		HRESULT hr = ParseHeader( pBundleStream, header );
		if ( SUCCEEDED( hr ) )
		{
			IBundleImportWorker* pImporter = NULL;
			hr = SelectImporter( header, &pImporter );
			if ( SUCCEEDED( hr ) )
			{
				hr = pImporter->Parse( pBundleStream, pBundle, pAdditionalData );
			}

			if ( pImporter )
			{
				delete pImporter;
			}
		}

		return hr;
	}

	HRESULT BundleImporter::Import(
		__in std::istream* pBundleStream,
		__in std::istream* pImageListStream,
		__out Bundler::Bundle* pBundle,
		__out_opt Bundler::BundleAdditionalPayload* pAdditionalData )
	{
		char header[ 256 ];
		HRESULT hr = ParseHeader( pBundleStream, header );
		if ( SUCCEEDED( hr ) )
		{
			IBundleImportWorker* pImporter = NULL;
			hr = SelectImporter( header, &pImporter );
			if ( SUCCEEDED( hr ) )
			{
				if ( pImporter->SupportsImageList() )
				{
					hr = pImporter->Parse( pBundleStream, pImageListStream, pBundle, pAdditionalData );
				}
				else
				{
					hr = pImporter->Parse( pBundleStream, pBundle, pAdditionalData );
				}
			}

			if ( pImporter )
			{
				delete pImporter;
			}
		}

		return hr;
	}

	HRESULT BundleImporter::Import(
		__in_z const char* pFilename,
		__out Bundler::Bundle* pBundle,
		__out_opt Bundler::BundleAdditionalPayload* pAdditionalData )
	{
		HRESULT hr = FileExists( pFilename ) ? S_OK : E_INVALIDARG;
		if ( SUCCEEDED( hr ) )
		{
			std::ifstream bundleStream = OpenStreamOnFile< std::ifstream >( pFilename );

			char imageListPath[ 512 ];
			GetImageListPath( pFilename, imageListPath );
			if ( FileExists( imageListPath ) )
			{
				std::ifstream imageListStream = OpenStreamOnFile< std::ifstream >( pFilename );
				hr = Import( &bundleStream, &imageListStream, pBundle, pAdditionalData );

				imageListStream.close();
			}
			else
			{
				hr = Import( &bundleStream, pBundle, pAdditionalData );
			}

			bundleStream.close();
		}

		return hr;
	}

	void BundleImporter::GetImageListPath( __in_z const char* pBundlePath, __out_ecount( 512 ) char* pImListPath )
	{
		const char* pBundleName = GetFilenameStart( pBundlePath );
		const char* pBundleExt = GetFileExtension( pBundlePath );
		const size_t nameLength = pBundleExt - pBundleName;
		strncpy_s( pImListPath, 512, pBundleName, nameLength );
		sprintf_s( pImListPath + nameLength, 512 - nameLength, IMAGE_LIST_EXTENSION );
	}

	HRESULT BundleImporter::ParseHeader( __in std::istream* pStream, __out_z char( &header )[ 256 ] )
	{
		return TextStreamReader::ReadLine( pStream, header );
	}

	HRESULT BundleImporter::SelectImporter( __in_z const char* pHeader, __deref_out IBundleImportWorker** ppImporter )
	{
		HRESULT hr = E_INVALIDARG;
		if ( strncmp( SnavelyBundleImportWorker::ExpectedHeader(), pHeader, 256 ) == 0 ) {
			hr = S_OK;
			*ppImporter = new SnavelyBundleImportWorker();
		}

		return hr;
	}

} }