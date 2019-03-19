#pragma once

namespace Bundler { namespace Import {

	class SnavelyBundleImportWorker : public IBundleImportWorker
	{
	public:

		static constexpr const char* ExpectedHeader()
		{
			return "# Bundle file v0.3";
		}

		static constexpr bool SupportsImageList()
		{
			return true;
		}

		HRESULT Parse(
			__in std::istream* pBundleStream,
			__out Bundle* pBundle,
			__out_opt BundleAdditionalPayload* pAdditionalData ) override;

		HRESULT Parse(
			__in std::istream* pBundleStream,
			__in std::istream* pImageListStream,
			__out Bundle* pBundle,
			__out_opt BundleAdditionalPayload* pAdditionalData ) override;


	protected:

		HRESULT ParseContent();
		HRESULT ParseCountInformation( __out uint& cameraCount, __out uint& pointCount );
		void InitializeBundle( __in const uint cameraCount, __in const uint pointCount );

		HRESULT ParseCameras( __in const uint cameraCount );
		HRESULT ParseCameraInfo( __in const uint cameraIndex, __out Scalar* pScale );
		HRESULT ParseCamera( __in const uint cameraIndex, __in const Scalar scale );
		HRESULT ParseCameraCalibration( __in const uint cameraIndex, __in const Scalar scale );
		HRESULT ParseCameraRotation( __in const uint cameraIndex );
		HRESULT ParseCameraTranslation( __in const uint cameraIndex );

		HRESULT ParsePoints( __in const uint pointCount );
		HRESULT ParsePoint( __in const uint pointIndex );
		HRESULT ParsePointPosition( __in const uint pointIndex );
		HRESULT ParsePointColor( __in const uint pointIndex );
		HRESULT ParseTrack( __in const uint pointIndex );
		HRESULT ParseProjection( __in const uint pointIndex );

		void FinalizeBundle();

	protected:

		std::istream* m_pBundleStream;
		std::istream* m_pImageListStream;

		Bundle* m_pBundleOut;
		BundleAdditionalPayload* m_pAdditionalDataOut;

		Containers::Buffer< CameraInfo > m_tempCameraInfo;
		Containers::PagedVector< Projection, 16 > m_tempProjections;
	};

} }
