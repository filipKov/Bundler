#include "stdafx.h"
#include <fstream>
#include "BundleImporter.h"

namespace Bundler { namespace Import {

#define BUNDLE_FILE_IDENTIFIER "# Bundle file "
#define BUNDLE_FILE_VER "v0.3"
#define BUNDLE_FILE_HEADER BUNDLE_FILE_IDENTIFIER BUNDLE_FILE_VER

#define ERR_INVALID_FILE "Stream does not contain valid Bundle file"

	void BundleImporter::Import( __in std::istream& stream, __out Bundle& bundle ) {
		m_pInputStream = &stream;
		m_pBundle = &bundle;

		ParseFile();

		m_pBundle = nullptr;
		m_pInputStream = nullptr;
	}

	void BundleImporter::Import( __in_z const char* pFilename, __out Bundle& bundle ) {
		std::ifstream inputStream = OpenStreamOnFile< std::ifstream >( pFilename );
		Import( inputStream, bundle );
	}

	void BundleImporter::ParseFile() {
		ParseAndCheckHeader();

		uint cameraCount = 0, pointCount = 0;
		ParseCountInformation( cameraCount, pointCount );
		InitializeBundle( cameraCount, pointCount );

		ParseCameras();
		ParsePoints();
	}

	void BundleImporter::ParseAndCheckHeader() {
		char header[256];

		m_pInputStream->getline( header, 256 );
		if ( strcmp( header, BUNDLE_FILE_HEADER ) != 0 ) {
			throw InvalidArgumentException( GET_VARIABLE_NAME( m_pInputStream ), ERR_INVALID_FILE );
		}
	}

	void BundleImporter::ParseCountInformation( __out uint& cameraCount, __out uint& pointCount ) {
		if ( !m_pInputStream->good() ) {
			throw WrongStateException( GET_VARIABLE_NAME( m_pInputStream ) );
		}

		(*m_pInputStream) >> cameraCount >> pointCount;
	}

	void BundleImporter::InitializeBundle( __in const uint cameraCount, __in const uint pointCount ) {
		m_pBundle->cameras.Allocate( cameraCount );
		m_pBundle->points.Allocate( pointCount );
	}

	void BundleImporter::ParseCameras() {
		uint cameraCount = (uint)m_pBundle->cameras.Length();

		for ( uint i = 0; i < cameraCount; i++ ) {
			if ( !m_pInputStream->good() ) {
				throw WrongStateException( GET_VARIABLE_NAME( m_pInputStream ) );
			}

			ParseCamera( i );
		}
	}

	void BundleImporter::ParseCamera( __in const uint cameraIndex ) {
		Camera& camera = m_pBundle->cameras[cameraIndex];
		Matrix3x3& rotMatrix = camera.r;
		Vector3& cameraPos = camera.t;

		( *m_pInputStream ) >> camera.k.fScale >> camera.k.k1 >> camera.k.k2;
		( *m_pInputStream ) >> rotMatrix[0][0] >> rotMatrix[0][1] >> rotMatrix[0][2]
							>> rotMatrix[1][0] >> rotMatrix[1][1] >> rotMatrix[1][2]
							>> rotMatrix[2][0] >> rotMatrix[2][1] >> rotMatrix[2][2];
		( *m_pInputStream ) >> cameraPos[0] >> cameraPos[1] >> cameraPos[2];
	}

	void BundleImporter::ParsePoints() {
		uint pointCount = (uint)m_pBundle->points.Length();

		for ( uint i = 0; i < pointCount; i++ ) {
			if ( !m_pInputStream->good() ) {
				throw WrongStateException( GET_VARIABLE_NAME( m_pInputStream ) );
			}

			ParsePoint( i );
		}

		m_pBundle->tracks.SetCopy( m_tempTracks );
		m_tempTracks.Clear();
	}

	void BundleImporter::ParsePoint( __in const uint pointIndex ) {
		Point& point = m_pBundle->points[pointIndex];

		Vector3& ptPos = point.position;
		
		uint r = 0, g = 0, b = 0;

		( *m_pInputStream ) >> ptPos[0] >> ptPos[1] >> ptPos[2];
		( *m_pInputStream ) >> r >> g >> b;

		point.color = 0xFF000000 | r << 16 | g << 8 | b;

		ParseTrack();
	}

	void BundleImporter::ParseTrack() {
		uint trackLength = 0;
		( *m_pInputStream ) >> trackLength;

		m_tempTracks.EnsureLength( m_tempTracks.Length() + trackLength );
		for ( uint trackI = 0; trackI < trackLength; trackI++ ) {
			Track& track = m_tempTracks[trackI];
			( *m_pInputStream ) >> track.cameraIndex >> track.pointIndex >> track.projectedPoint[0] >> track.projectedPoint[1];
		}
	}

} }