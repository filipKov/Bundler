#pragma once

namespace Bundler { namespace Import {

	class BundleImporter
	{
	public:
		
		void Import( __in std::istream& stream, __out Bundler::Bundle& bundle );
		void Import( __in_z const char* pFilename, __out Bundler::Bundle& bundle );

	protected:

		void ParseFile();
		void ParseAndCheckHeader();
		void ParseCountInformation( __out uint& cameraCount, __out uint& pointCount );
		void InitializeBundle( __in const uint cameraCount, __in const uint pointCount );
		void ParseCameras();
		void ParseCamera( __in const uint cameraIndex );
		void ParsePoints();
		void ParsePoint( __in const uint pointIndex );
		void ParseTrack();


	protected:

		std::istream* m_pInputStream;

		Bundler::Bundle* m_pBundle;

		Containers::PagedVector< Bundler::Projection, 12 > m_tempTracks;

	};

} }
