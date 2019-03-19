#include "stdafx.h"
#include <fstream>
#include "TextStreamReader.h"
#include "IBundleImportWorker.h"
#include "SnavelyBundleImportWorker.h"

namespace Bundler { namespace Import {

	bool SnavelyBundleImportWorker::SupportsImageList() const
	{
		return true;
	}

	HRESULT SnavelyBundleImportWorker::Parse(
		__in std::istream* pBundleStream,
		__out Bundle* pBundle,
		__out_opt BundleAdditionalPayload* pAdditionalData )
	{
		return Parse( pBundleStream, NULL, pBundle, pAdditionalData );
	}

	HRESULT SnavelyBundleImportWorker::Parse(
		__in std::istream* pBundleStream,
		__in_opt std::istream* pImageListStream,
		__out Bundle* pBundle,
		__out_opt BundleAdditionalPayload* pAdditionalData )
	{
		if ( !pBundleStream || !pBundle )
		{
			return E_INVALIDARG;
		}
		
		m_pBundleStream = pBundleStream;
		m_pImageListStream = pImageListStream;
		m_pBundleOut = pBundle;
		m_pAdditionalDataOut = pAdditionalData;

		HRESULT hr = ParseContent();

		m_pBundleStream = NULL;
		m_pImageListStream = NULL;
		m_pBundleOut = NULL;
		m_pAdditionalDataOut = NULL;

		return hr;
	}


	HRESULT SnavelyBundleImportWorker::ParseContent() 
	{
		uint cameraCount = 0, pointCount = 0;
		HRESULT hr = ParseCountInformation( cameraCount, pointCount );
		
		if ( SUCCEEDED( hr ) )
		{
			InitializeBundle( cameraCount, pointCount );

			hr = ParseCameras( cameraCount );
			if ( SUCCEEDED( hr ) )
			{
				hr = ParsePoints( pointCount );
			}
		}

		if ( SUCCEEDED( hr ) )
		{
			FinalizeBundle();
		}

		return hr;
	}
	
	HRESULT SnavelyBundleImportWorker::ParseCountInformation( __out uint& cameraCount, __out uint& pointCount ) 
	{
		HRESULT hr = TextStreamReader::ReadUint32( m_pBundleStream, &cameraCount );
		if ( SUCCEEDED( hr ) )
		{
			hr = TextStreamReader::ReadUint32( m_pBundleStream, &pointCount );
		}
		return hr;
	}
	
	void SnavelyBundleImportWorker::InitializeBundle( __in const uint cameraCount, __in const uint pointCount ) 
	{
		m_pBundleOut->cameras.Allocate( cameraCount );
		m_pBundleOut->points.Allocate( pointCount );
	
		if ( m_pAdditionalDataOut ) 
		{
			m_pAdditionalDataOut->pointColors.Allocate( pointCount );
			m_pAdditionalDataOut->cameraInformation.Allocate( cameraCount );
		}
		
		m_tempCameraInfo.Allocate( cameraCount );
	}
	
	HRESULT SnavelyBundleImportWorker::ParseCameras( __in const uint cameraCount ) 
	{
		CameraInfo dummyCamInfo;
		dummyCamInfo.width = 1;
		dummyCamInfo.height = 1;

		HRESULT hr = S_OK;
		for ( uint i = 0; i < cameraCount; i++ ) {
			Scalar scale = Scalar( 1 );
			
			if ( m_pImageListStream )
			{
				hr = ParseCameraInfo( i, &scale );
			}
			else
			{
				m_tempCameraInfo[ i ] = dummyCamInfo;
			}
	
			if ( SUCCEEDED( hr ) )
			{
				ParseCamera( i, scale );
			}

			if ( FAILED( hr ) )
			{
				break;
			}
		}

		return hr;
	}

	HRESULT SnavelyBundleImportWorker::ParseCameraInfo( __in const uint cameraIndex, __out Scalar* pScale )
	{
		char imagePathTemp[ 512 ];
		HRESULT hr = TextStreamReader::ReadLine( m_pImageListStream, ' ', imagePathTemp );
		if ( SUCCEEDED( hr ) )
		{
			CameraInfo& info = m_tempCameraInfo[ cameraIndex ];

			hr = TextStreamReader::ReadUint32( m_pImageListStream, &info.width );
			if ( SUCCEEDED( hr ) )
			{
				hr = TextStreamReader::ReadUint32( m_pImageListStream, &info.height );
			}

			if ( SUCCEEDED( hr ) )
			{
				*pScale = (Scalar)max( info.width, info.height );
			}
		}

		return hr;
	}

	HRESULT SnavelyBundleImportWorker::ParseCamera( __in const uint cameraIndex, __in const Scalar scale ) 
	{
		HRESULT hr = ParseCameraCalibration( cameraIndex, scale );
		if ( SUCCEEDED( hr ) )
		{
			hr = ParseCameraRotation( cameraIndex );
			if ( SUCCEEDED( hr ) )
			{
				hr = ParseCameraTranslation( cameraIndex );
			}
		}

		return hr;
	}

	HRESULT SnavelyBundleImportWorker::ParseCameraCalibration( __in const uint cameraIndex, __in const Scalar scale )
	{
		Camera& camera = m_pBundleOut->cameras[ cameraIndex ];
		double tempVal;

		HRESULT hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
		if ( SUCCEEDED( hr ) )
		{
			camera.focalLength = (Scalar)tempVal / scale;
		}

		// Skip distortion coefficients
		if ( SUCCEEDED( hr ) )
		{
			hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
			if ( SUCCEEDED( hr ) )
			{
				hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
			}
		}

		return hr;
	}

	HRESULT SnavelyBundleImportWorker::ParseCameraRotation( __in const uint cameraIndex )
	{
		Camera& camera = m_pBundleOut->cameras[ cameraIndex ];
		double tempVal;
		HRESULT hr = S_OK;
		
		Scalar* pDst = camera.r.Elements();
		for ( uint i = 0; i < 9; i++ )
		{
			hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
			if ( SUCCEEDED( hr ) )
			{
				*pDst = (Scalar)tempVal;
				pDst++;
			}

			if ( FAILED( hr ) )
			{
				break;
			}
		}

		if ( SUCCEEDED( hr ) )
		{
									; camera.r[ 0 ][ 1 ] *= -1;
			camera.r[ 1 ][ 0 ] *= -1;						  ; camera.r[ 1 ][ 2 ] *= -1;
			camera.r[ 2 ][ 0 ] *= -1;						  ; camera.r[ 2 ][ 2 ] *= -1;

			Swap( camera.r[ 0 ][ 1 ], camera.r[ 0 ][ 2 ] );
			Swap( camera.r[ 1 ][ 1 ], camera.r[ 1 ][ 2 ] );
			Swap( camera.r[ 2 ][ 1 ], camera.r[ 2 ][ 2 ] );
		}
		
		return hr;
	}

	HRESULT SnavelyBundleImportWorker::ParseCameraTranslation( __in const uint cameraIndex )
	{
		Camera& camera = m_pBundleOut->cameras[ cameraIndex ];
		double tempVal;
		HRESULT hr = S_OK;

		Scalar* pDst = camera.t.Elements();
		for ( uint i = 0; i < 3; i++ )
		{
			hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
			if ( SUCCEEDED( hr ) )
			{
				*pDst = (Scalar)tempVal;
				pDst++;
			}

			if ( FAILED( hr ) )
			{
				break;
			}
		}

		if ( SUCCEEDED( hr ) )
		{
			camera.t[ 1 ] *= -1;
			camera.t[ 2 ] *= -1;
		}

		return hr;
	}

	
	HRESULT SnavelyBundleImportWorker::ParsePoints( __in const uint pointCount ) 
	{
		HRESULT hr = S_OK;
		for ( uint i = 0; i < pointCount; i++ ) {
			hr = ParsePoint( i );
			if ( FAILED( hr ) )
			{
				break;
			}
		}

		return hr;
	}
	
	HRESULT SnavelyBundleImportWorker::ParsePoint( __in const uint pointIndex ) 
	{
		HRESULT hr = ParsePointPosition( pointIndex );
		if ( SUCCEEDED( hr ) )
		{
			hr = ParsePointColor( pointIndex );
			if ( SUCCEEDED( hr ) )
			{
				hr = ParseTrack( pointIndex );
			}
		}

		return hr;
	}
	
	HRESULT SnavelyBundleImportWorker::ParsePointPosition( __in const uint pointIndex )
	{
		Vector3& point = m_pBundleOut->points[ pointIndex ];
		double tempVal;

		HRESULT hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
		if ( SUCCEEDED( hr ) )
		{
			point[ 0 ] = (Scalar)tempVal;
			hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
			if ( SUCCEEDED( hr ) )
			{
				point[ 1 ] = (Scalar)tempVal;
				hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
				if ( SUCCEEDED( hr ) )
				{
					point[ 2 ] = (Scalar)tempVal;
				}
			}
		}

		if ( SUCCEEDED( hr ) )
		{
			point[ 1 ] *= -1;
			Swap( point[ 1 ], point[ 2 ] );
		}

		return hr;
	}

	HRESULT SnavelyBundleImportWorker::ParsePointColor( __in const uint pointIndex )
	{
		uint r = 0, g = 0, b = 0;

		HRESULT hr = TextStreamReader::ReadUint32( m_pBundleStream, &r );
		if ( SUCCEEDED( hr ) )
		{
			hr = TextStreamReader::ReadUint32( m_pBundleStream, &g );
			if ( SUCCEEDED( hr ) )
			{
				hr = TextStreamReader::ReadUint32( m_pBundleStream, &b );
			}
		}

		if ( SUCCEEDED( hr ) && m_pAdditionalDataOut )
		{
			m_pAdditionalDataOut->pointColors[ pointIndex ] = 0xFF000000 | r << 16 | g << 8 | b;
		}

		return hr;
	}

	HRESULT SnavelyBundleImportWorker::ParseTrack( __in const uint pointIndex ) 
	{
		uint trackLength = 0;
		HRESULT hr = TextStreamReader::ReadUint32( m_pBundleStream, &trackLength );
		if ( SUCCEEDED( hr ) )
		{
			m_tempProjections.EnsureCapacity( m_tempProjections.Length() + trackLength );

			for ( uint projectionI = 0; projectionI < trackLength; projectionI++ )
			{
				hr = ParseProjection( pointIndex );
				if ( FAILED( hr ) )
				{
					break;
				}
			}
		}

		return hr;
	}

	HRESULT SnavelyBundleImportWorker::ParseProjection( __in const uint pointIndex )
	{
		uint64 ftIndexSink = 0;
		Projection projection;
		projection.pointIndex = pointIndex;
		HRESULT hr = TextStreamReader::ReadUint32( m_pBundleStream, &projection.cameraIndex );
		if ( SUCCEEDED( hr ) )
		{
			hr = TextStreamReader::ReadUint64( m_pBundleStream, &ftIndexSink );
			if ( SUCCEEDED( hr ) )
			{
				double tempVal;
				hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
				if ( SUCCEEDED( hr ) )
				{
					projection.projectedPoint[ 0 ] = (Scalar)tempVal;
					hr = TextStreamReader::ReadFloat64( m_pBundleStream, &tempVal );
					if ( SUCCEEDED( hr ) )
					{
						projection.projectedPoint[ 1 ] = (Scalar)tempVal;
					}
				}
			}
		}

		if ( SUCCEEDED( hr ) )
		{
			CameraInfo& camInfo = m_tempCameraInfo[ projection.cameraIndex ];
			Scalar scaleInv = 1 / (Scalar)max( camInfo.width, camInfo.height );
			projection.projectedPoint *= scaleInv;

			projection.projectedPoint[ 1 ] *= -1;
		}

		if ( SUCCEEDED( hr ) )
		{
			m_tempProjections.Add( projection );
		}

		return hr;
	}

	void SnavelyBundleImportWorker::FinalizeBundle()
	{
		if ( m_pAdditionalDataOut )
		{
			m_pAdditionalDataOut->cameraInformation.SetCopy( m_tempCameraInfo );
		}

		m_pBundleOut->projections.SetCopy( m_tempProjections );

		m_tempCameraInfo.Clear();
		m_tempProjections.Clear();
	}

} }