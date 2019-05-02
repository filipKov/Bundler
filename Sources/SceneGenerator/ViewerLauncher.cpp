#include "stdafx.h"

#include "RenderingLib.h"

#include "Texture.h"
#include "TextureCache.h"

#include "Transformable.h"
#include "SceneObject.h"
#include "ScenePointCloud.h"
#include "SceneMeshPart.h"
#include "SceneMesh.h"
#include "Scene.h"

#include "Mouse.h"
#include "KeyMacros.h"
#include "CameraController.h"

#include "ApplicationExceptions.h"
#include "ProgressBar.h"
#include "ApplicationSectionStrings.h"

#include "ViewerApplication.h"

#include "ViewerLauncher.h"

using namespace ::Utils::Containers;
using namespace Bundler;

class ColorUtils
{
public:

	static void argbToFloat( __in const uint argb, __out_ecount( 4 ) float* vector )
	{
		constexpr const float rgbMaxInv = 1.0f / 255.0f;
		ELEMENT( vector, 0 ) = ( ( argb >> 16 ) & 0xFF ) * rgbMaxInv;
		ELEMENT( vector, 1 ) = ( ( argb >> 8 ) & 0xFF ) * rgbMaxInv;
		ELEMENT( vector, 2 ) = ( ( argb >> 0 ) & 0xFF ) * rgbMaxInv;
		ELEMENT( vector, 3 ) = ( ( argb >> 24  ) & 0xFF ) * rgbMaxInv;
	}

	static void argbToFloat( 
		__in const uint count, 
		__in_ecount( count ) const uint* pArgbVals, 
		__out_ecount( 4 * count ) float* pFloatVals )
	{
		for ( uint i = 0; i < count; i++ )
		{
			argbToFloat( *pArgbVals, pFloatVals );
			pArgbVals++;
			pFloatVals += 4;
		}
	}

protected:

};

template < typename T, typename ElementFnc >
void V3GetByElements( __in_ecount( 3 ) const T* v1, __in_ecount( 3 ) const T* v2, __out_ecount( 3 ) T* v3, __in ElementFnc fnc )
{
	ELEMENT( v3, 0 ) = fnc( ELEMENT( v1, 0 ), ELEMENT( v2, 0 ) );
	ELEMENT( v3, 1 ) = fnc( ELEMENT( v1, 1 ), ELEMENT( v2, 1 ) );
	ELEMENT( v3, 2 ) = fnc( ELEMENT( v1, 2 ), ELEMENT( v2, 2 ) );
}

template < typename T >
T GetMin( __in const T val1, __in const T val2 )
{
	return min( val1, val2 );
}

template < typename T >
T GetMax( __in const T val1, __in const T val2 )
{
	return max( val1, val2 );
}

void GetBoundingBox(
	__in const uint count,
	__in_ecount( count ) const Bundler::Vector3* pVertices,
	__out BoundingBox* pBBox,
	__out_ecount_opt( 4 ) float* pPivot )
{
	Scalar minVert[ 3 ] = { FLT_MAX, FLT_MAX, FLT_MAX };
	Scalar maxVert[ 3 ] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	
	for ( uint i = 0; i < count; i++ )
	{
		V3GetByElements( minVert, pVertices->Elements( ), minVert, GetMin< Scalar > );
		V3GetByElements( maxVert, pVertices->Elements( ), maxVert, GetMax< Scalar > );
		pVertices++;
	}

	MatrixCast< Scalar, float, 3, 1 >( minVert, pBBox->minVert.Elements( ) );
	MatrixCast< Scalar, float, 3, 1 >( maxVert, pBBox->maxVert.Elements( ) );

	if ( pPivot )
	{
		V3AddV3( pBBox->maxVert.Elements( ), pBBox->minVert.Elements( ), pPivot );
		V3MulC( pPivot, 0.5f, pPivot );
		ELEMENT( pPivot, 3 ) = 1.0f;
	}
}

template < typename Scalar >
void CopyVerticesToGL(
	__in const uint vertexCount,
	__in_ecount( vertexCount ) const Vector3* pVertices,
	__in GLuint vertexBufferId )
{
	static_assert( false, "Conversion not yet implemented, use Scalar == float to show point cloud" );
}

template <>
void CopyVerticesToGL< float >(
	__in const uint vertexCount,
	__in_ecount( vertexCount ) const Vector3* pVertices,
	__in GLuint vertexBufferId )
{
	glBindBuffer( GL_ARRAY_BUFFER, vertexBufferId );
	glBufferData( GL_ARRAY_BUFFER, vertexCount * sizeof( Vector3 ), pVertices, GL_STATIC_DRAW );
}

void CopyVertexColorsToGL(
	__in const uint vertexCount,
	__in_ecount_opt( vertexCount ) const uint* pVertexColors,
	__in GLuint vertexColorBufferId )
{
	Buffer< float > vertexColorBuffer;
	vertexColorBuffer.Allocate( 4 * vertexCount );

	if ( pVertexColors )
	{
		ColorUtils::argbToFloat( vertexCount, pVertexColors, vertexColorBuffer.Data( ) );
	}
	else
	{
		vertexColorBuffer.Fill( 1.0f );
	}

	glBindBuffer( GL_ARRAY_BUFFER, vertexColorBufferId );
	glBufferData( GL_ARRAY_BUFFER, 4 * vertexCount * sizeof( float ), vertexColorBuffer.Data( ), GL_STATIC_DRAW );
}

void InitializeScenePointCloud(
	__in const uint vertexCount,
	__in_ecount( vertexCount ) const Vector3* pVertices,
	__in_ecount_opt( vertexCount ) const uint* pVertexColors,
	__out ScenePointCloud* pPointCloud )
{
	GlPointCloud glPc;
	glGenVertexArrays( 1, &( glPc.arrayObject ) );
	glGenBuffers( 2, &( glPc.vertexBuffer ) );

	CopyVerticesToGL< Scalar >( vertexCount, pVertices, glPc.vertexBuffer );
	CopyVertexColorsToGL( vertexCount, pVertexColors, glPc.vertexColorBuffer );
 
	float pivot[4];
	BoundingBox bbox;
	GetBoundingBox( vertexCount, pVertices, &bbox, pivot );

	pPointCloud->SetPointCloud( vertexCount, glPc, bbox );
	pPointCloud->SetPivot( pivot );
}

void InitializeScenePointCloud( 
	__in const uint vertexCount,
	__in_ecount( vertexCount ) const Vector3* pVertices,
	__out ScenePointCloud* pPointCloud )
{
	InitializeScenePointCloud( vertexCount, pVertices, NULL, pPointCloud );
}

void ViewerLauncher::ShowBundle( 
	__in const Bundle* pBundle,
	__in_opt const BundleAdditionalPayload* pMetadata,
	__in_ecount_opt( 3 ) float* pPivot )
{
	ViewerApplication* pViewer = ViewerApplication::GetApp();

	ProgressBar& progressBar = pViewer->GetProgressBar();
	progressBar.BeginSection( AlgID_ViewerDataConversion, 1.0f );
	
	ScenePointCloud* pViewerPointCloud = new ScenePointCloud();

	const uint pointCount = (uint)pBundle->points.Length( );
	if ( pMetadata )
	{
		InitializeScenePointCloud( pointCount, pBundle->points.Data( ), pMetadata->pointColors.Data( ), pViewerPointCloud );
	}
	else
	{
		InitializeScenePointCloud( pointCount, pBundle->points.Data( ), pViewerPointCloud );
	}

	if ( pPivot )
	{
		float pivot4[4];
		ShallowCopy( pPivot, 3, pivot4 );
		pivot4[3] = 1.0f;

		pViewerPointCloud->SetPivot( pivot4 );
	}

	progressBar.EndSection( S_OK, NULL );
	
	pViewer->ShowPointCloud( pViewerPointCloud );
	pViewerPointCloud = NULL;
}

void ViewerLauncher::InitializeEmpty( )
{
	ViewerApplication* pViewer = ViewerApplication::GetApp( );
	pViewer->Initialize( 0, NULL );
}

void ViewerLauncher::LaunchViewer() {
	ViewerApplication::GetApp()->Start();
}