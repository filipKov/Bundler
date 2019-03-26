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

using namespace Utils::Containers;

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

template < typename ElementFnc >
Vector3f GetByElements( __in ElementFnc fnc, __in const Vector3f v1, __in const Vector3f v2 )
{
	return Vector3f( { fnc( v1[0], v2[0] ), fnc( v1[1], v2[1] ), fnc( v1[2], v2[2] ) } );
}

float GetMin( __in const float val1, __in const float val2 )
{
	return min( val1, val2 );
}

float GetMax( __in const float val1, __in const float val2 )
{
	return max( val1, val2 );
}

void GetBoundingBox(
	__in const uint count,
	__in_ecount( count ) const Bundler::Vector3* pVertices,
	__out BoundingBox* pBBox,
	__out_ecount_opt( 4 ) float* pPivot )
{
	pBBox->minVert = { FLT_MAX, FLT_MAX, FLT_MAX };
	pBBox->maxVert = -pBBox->minVert;

	for ( uint i = 0; i < count; i++ )
	{
		pBBox->minVert = GetByElements( GetMin, pBBox->minVert, *pVertices );
		pBBox->maxVert = GetByElements( GetMax, pBBox->maxVert, *pVertices );
		pVertices++;
	}

	if ( pPivot )
	{
		V3SubV3( pBBox->maxVert.Elements( ), pBBox->minVert.Elements( ), pPivot );
		V3MulC( pPivot, 0.5f, pPivot );
		ELEMENT( pPivot, 3 ) = 1.0f;
	}
}

void InitializeScenePointCloud(
	__in const uint pointCount,
	__in_ecount( pointCount ) const Bundler::Vector3* pPoints,
	__in_ecount_opt( pointCount ) const uint* pPointColors,
	__out ScenePointCloud* pViewerPointCloud )
{
	GlPointCloud glPc;
	glGenVertexArrays( 1, &( glPc.arrayObject ) );
	glGenBuffers( 2, &( glPc.vertexBuffer ) );

	glBindBuffer( GL_ARRAY_BUFFER, glPc.vertexBuffer );
	if ( sizeof( Bundler::Scalar ) == sizeof( float ) )
	{
		glBufferData( GL_ARRAY_BUFFER, pointCount * sizeof( Bundler::Vector3 ), pPoints, GL_STATIC_DRAW );
	}
	else
	{
		// TODO
	}

	glBindBuffer( GL_ARRAY_BUFFER, glPc.vertexColorBuffer );

	Buffer< float > vertexColors;
	vertexColors.Allocate( 4 * pointCount );

	if ( pPointColors )
	{
		ColorUtils::argbToFloat( pointCount, pPointColors, vertexColors.Data( ) );
	}
	else
	{
		vertexColors.Fill( 0.75f );
	}
	
	glBufferData( GL_ARRAY_BUFFER, 4 * pointCount * sizeof( float ), vertexColors.Data( ), GL_STATIC_DRAW );

	float pivot[4];
	BoundingBox bbox;
	GetBoundingBox( pointCount, pPoints, &bbox, pivot );

	pViewerPointCloud->SetPointCloud( pointCount, glPc, bbox );
	pViewerPointCloud->SetPivot( pivot );
}

void ViewerLauncher::InitializePointCloud (
	__in const uint pointCount,
	__in_ecount( pointCount ) const Bundler::Vector3* pPoints,
	__in_ecount_opt( pointCount ) const uint* pPointColors )
{
	ViewerApplication* pViewer = ViewerApplication::GetApp();
	pViewer->Initialize( 0, NULL );
	
	ProgressBar& progressBar = pViewer->GetProgressBar();
	progressBar.BeginSection( AlgID_ViewerDataConversion, 1.0f );
	
	ScenePointCloud* pViewerPointCloud = new ScenePointCloud();
	InitializeScenePointCloud( pointCount, pPoints, pPointColors, pViewerPointCloud );
	
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