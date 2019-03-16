#include "stdafx.h"
#include <fstream>
#include "BundleImporter.h"

namespace Bundler { namespace Import {

#define BUNDLE_FILE_IDENTIFIER "# Bundle file "
#define BUNDLE_FILE_VER_3 "v0.3"
#define BUNDLE_FILE_HEADER BUNDLE_FILE_IDENTIFIER BUNDLE_FILE_VER_3

#define ERR_INVALID_FILE "Stream does not contain valid Bundle file"

	void BundleImporter::Import( __in std::istream& stream, __out Bundler::Bundle* pBundle, __out_opt Bundler::BundleAdditionalPayload* pAdditionalData )
	{
		m_pInputStream = &stream;
		m_pOutBundle = pBundle;
		m_pAdditionalData = pAdditionalData;

		ParseFile();

		m_pOutBundle = nullptr;
		m_pAdditionalData = nullptr;
		m_pInputStream = nullptr;
	}

	void BundleImporter::Import( __in_z const char* pFilename, __out Bundler::Bundle* pBundle, __out_opt Bundler::BundleAdditionalPayload* pAdditionalData )
	{
		std::ifstream inputStream = OpenStreamOnFile< std::ifstream >( pFilename );
		Import( inputStream, pBundle, pAdditionalData );
	}

	void BundleImporter::ParseFile() 
	{
		ParseAndCheckHeader();

		uint cameraCount = 0, pointCount = 0;
		ParseCountInformation( cameraCount, pointCount );
		InitializeBundle( cameraCount, pointCount );

		ParseCameras( cameraCount );
		ParsePoints( pointCount );
	}

	void BundleImporter::ParseAndCheckHeader() 
	{
		char header[256];

		m_pInputStream->getline( header, 256 );
		if ( strcmp( header, BUNDLE_FILE_HEADER ) != 0 ) 
		{
			throw InvalidArgumentException( GET_VARIABLE_NAME( m_pInputStream ), ERR_INVALID_FILE );
		}
	}

	void BundleImporter::ParseCountInformation( __out uint& cameraCount, __out uint& pointCount ) 
	{
		if ( !m_pInputStream->good() ) 
		{
			throw WrongStateException( GET_VARIABLE_NAME( m_pInputStream ) );
		}

		(*m_pInputStream) >> cameraCount >> pointCount;
	}

	void BundleImporter::InitializeBundle( __in const uint cameraCount, __in const uint pointCount ) 
	{
		m_pOutBundle->cameras.Allocate( cameraCount );
		m_pOutBundle->points.Allocate( pointCount );

		if ( m_pAdditionalData ) 
		{
			m_pAdditionalData->pointColors.Allocate( pointCount );
		}
	}

	void BundleImporter::ParseCameras( __in const uint cameraCount ) 
	{
		for ( uint i = 0; i < cameraCount; i++ ) {
			if ( !m_pInputStream->good() ) {
				throw WrongStateException( GET_VARIABLE_NAME( m_pInputStream ) );
			}

			ParseCamera( i );
		}
	}

	void BundleImporter::ParseCamera( __in const uint cameraIndex ) {
		Camera& camera = m_pOutBundle->cameras[cameraIndex];

		Matrix3x3& rotMatrix = camera.r;
		Vector3& cameraPos = camera.t;

		( *m_pInputStream ) >> camera.k.fScale >> camera.k.k1 >> camera.k.k2;
		( *m_pInputStream ) >> rotMatrix[0][0] >> rotMatrix[0][1] >> rotMatrix[0][2]
							>> rotMatrix[1][0] >> rotMatrix[1][1] >> rotMatrix[1][2]
							>> rotMatrix[2][0] >> rotMatrix[2][1] >> rotMatrix[2][2];
		( *m_pInputStream ) >> cameraPos[0] >> cameraPos[1] >> cameraPos[2];
	}

	void BundleImporter::ParsePoints( __in const uint pointCount ) 
	{
		for ( uint i = 0; i < pointCount; i++ ) {
			if ( !m_pInputStream->good() ) {
				throw WrongStateException( GET_VARIABLE_NAME( m_pInputStream ) );
			}

			ParsePoint( i );
		}

		m_pOutBundle->projections.SetCopy( m_tempTracks );
		m_tempTracks.Clear();
	}

	void BundleImporter::ParsePoint( __in const uint pointIndex ) 
	{
		Vector3& point = m_pOutBundle->points[pointIndex];
		uint r = 0, g = 0, b = 0;

		( *m_pInputStream ) >> point[0] >> point[1] >> point[2];
		( *m_pInputStream ) >> r >> g >> b;

		if ( m_pAdditionalData ) 
		{
			m_pAdditionalData->pointColors[ pointIndex ] = 0xFF000000 | r << 16 | g << 8 | b;
		}

		ParseTrack( pointIndex );
	}

	void BundleImporter::ParseTrack( __in const uint pointIndex ) 
	{
		uint trackLength = 0;
		( *m_pInputStream ) >> trackLength;

		uint featureIndex = 0;

		m_tempTracks.EnsureCapacity( m_tempTracks.Length() + trackLength );
		for ( uint trackI = 0; trackI < trackLength; trackI++ ) 
		{
			Projection track;
			( *m_pInputStream ) >> track.cameraIndex >> featureIndex >> track.projectedPoint[0] >> track.projectedPoint[1];
			track.pointIndex = pointIndex;
			m_tempTracks.Add( track );
		}
	}

} }