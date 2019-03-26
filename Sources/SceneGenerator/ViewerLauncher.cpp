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

const float rgbMaxInv = 1.0f / 255.0f;

// void GetBox( __in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox, __out BoundingBox& aabb, __out Vector4f& pivot ) {
// 	aabb.minVert = Vector3f( { (float)bbox.position.x, (float)bbox.position.y, (float)bbox.position.z } );
// 	aabb.maxVert = aabb.minVert;
// 
// 	Vector3f delta = Vector3f( { (float)bbox.extents.x, (float)bbox.extents.y, (float)bbox.extents.z } );
// 	aabb.minVert -= delta;
// 	aabb.maxVert += delta;
// 
// 	pivot = Vector4f( { (float)bbox.position.x, (float)bbox.position.y, (float)bbox.position.z, 1.0f } );
// }
// 
// void GetBox(
// 	__in const uint vertexCount,
// 	__in_ecount( 3 * vertexCount ) const float* pCoords,
// 	__out BoundingBox& bbox,
// 	__out Vector4f& pivot )
// {
// 	bbox.minVert = Vector3f( { pCoords[ 0 ], pCoords[ 1 ], pCoords[ 2 ] } );
// 	bbox.maxVert = bbox.minVert;
// 
// 	for ( uint i = 1; i < vertexCount; i++ ) {
// 		bbox.minVert[ 0 ] = min( pCoords[ 3 * i + 0 ], bbox.minVert[ 0 ] );
// 		bbox.minVert[ 1 ] = min( pCoords[ 3 * i + 1 ], bbox.minVert[ 1 ] );
// 		bbox.minVert[ 2 ] = min( pCoords[ 3 * i + 2 ], bbox.minVert[ 2 ] );
// 
// 		bbox.maxVert[ 0 ] = max( pCoords[ 3 * i + 0 ], bbox.maxVert[ 0 ] );
// 		bbox.maxVert[ 1 ] = max( pCoords[ 3 * i + 1 ], bbox.maxVert[ 1 ] );
// 		bbox.maxVert[ 2 ] = max( pCoords[ 3 * i + 2 ], bbox.maxVert[ 2 ] );
// 	}
// 
// 	Vector3f p = bbox.maxVert + bbox.minVert * 0.5f;
// 	pivot = Vector4f( { p[ 0 ], p[ 1 ], p[ 2 ], 1.0f } );
// }
// 
// void ConvertToBuffers(
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const uint* pVertexIndices,
// 	__in const SfmReconstructionPoint* pVertices,
// 	__out_ecount( vertexCount * 3 ) float* pCoords,
// 	__out_ecount( vertexCount * 4 ) float* pColors )
// {
// 	for ( uint i = 0; i < vertexCount; i++ ) {
// 		const SfmReconstructionPoint* pPt = pVertices + ( *pVertexIndices );
// 		
// 		pCoords[ 0 ] = (float)( pPt->X.x );
// 		pCoords[ 1 ] = (float)( pPt->X.y );
// 		pCoords[ 2 ] = (float)( pPt->X.z );
// 		pCoords += 3;
// 
// 		pColors[ 0 ] = (float)( ( pPt->color >> 0 ) & 255 )  * rgbMaxInv;
// 		pColors[ 1 ] = (float)( ( pPt->color >> 8 ) & 255 )  * rgbMaxInv;
// 		pColors[ 2 ] = (float)( ( pPt->color >> 16 ) & 255 ) * rgbMaxInv;
// 		pColors[ 3 ] = (float)( ( pPt->color >> 24 ) & 255 ) * rgbMaxInv;
// 		pColors += 4;
// 
// 		pVertexIndices++;
// 	}
// }
// 
// void ConvertVertices(
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
// 	__in_ecount( vertexCount ) const DirectX::XMVECTOR* pVertexColors,
// 	__out_ecount( 3*vertexCount ) float* pCoords,
// 	__out_ecount( 4*vertexCount ) float* pColors )
// {
// 	for ( int i = 0; i < (int)vertexCount; i++ ) {
// 		pCoords[ 0 ] = pVertices->pt.x;
// 		pCoords[ 1 ] = pVertices->pt.y;
// 		pCoords[ 2 ] = pVertices->pt.z;
// 		pCoords += 3;
// 		pVertices++;
// 		
// 		pColors[ 0 ] = pVertexColors->m128_f32[2] * rgbMaxInv;
// 		pColors[ 1 ] = pVertexColors->m128_f32[1] * rgbMaxInv;
// 		pColors[ 2 ] = pVertexColors->m128_f32[0] * rgbMaxInv;
// 		pColors[ 3 ] = 1.0f;
// 		pColors += 4;
// 		pVertexColors++;
// 	}
// }
// 
// void GetVertexCoords(
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
// 	__out_ecount( 3 * vertexCount ) float* pCoords )
// {
// 	for ( uint i = 0; i < vertexCount; i++ ) {
// 		pCoords[ 0 ] = pVertices->pt.x;
// 		pCoords[ 1 ] = pVertices->pt.y;
// 		pCoords[ 2 ] = pVertices->pt.z;
// 
// 		pCoords += 3;
// 		pVertices++;
// 	}
// }
// 
// void GetVertexNormals(
// 	__in const uint vertexCount,
// 	__in_ecount( 3 * vertexCount ) const float* pVertices,
// 	__in const uint faceCount,
// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
// 	__out_ecount( 3 * vertexCount ) float* pNormals )
// {
// 	Buffer< uint > vertexFaceCnt;
// 	vertexFaceCnt.Allocate( vertexCount );
// 	ByteFill( 0, vertexCount, vertexFaceCnt.Data() );
// 	ByteFill( 0, 3 * vertexCount, pNormals );
// 
// 	float a[ 3 ];
// 	float b[ 3 ];
// 
// 	for ( uint i = 0; i < faceCount; i++ ) {
// 		const UINT* vertIx = pFaces[ i ].vertices;
// 
// 		const float* v0 = &pVertices[ 3 * vertIx[ 0 ] ];
// 		const float* v1 = &pVertices[ 3 * vertIx[ 1 ] ];
// 		const float* v2 = &pVertices[ 3 * vertIx[ 2 ] ];
// 
// 		a[ 0 ] = v2[ 0 ] - v0[ 0 ];
// 		a[ 1 ] = v2[ 1 ] - v0[ 1 ];
// 		a[ 2 ] = v2[ 2 ] - v0[ 2 ];
// 
// 		b[ 0 ] = v1[ 0 ] - v0[ 0 ];
// 		b[ 1 ] = v1[ 1 ] - v0[ 1 ];
// 		b[ 2 ] = v1[ 2 ] - v0[ 2 ];
// 
// 		Vector3f faceNormal = CrossProduct( b, a );
// 		faceNormal.Normalize();
// 
// 
// 		pNormals[ 3 * vertIx[ 0 ] + 0 ] += faceNormal[ 0 ];
// 		pNormals[ 3 * vertIx[ 0 ] + 1 ] += faceNormal[ 1 ];
// 		pNormals[ 3 * vertIx[ 0 ] + 2 ] += faceNormal[ 2 ];
// 
// 		pNormals[ 3 * vertIx[ 1 ] + 0 ] += faceNormal[ 0 ];
// 		pNormals[ 3 * vertIx[ 1 ] + 1 ] += faceNormal[ 1 ];
// 		pNormals[ 3 * vertIx[ 1 ] + 2 ] += faceNormal[ 2 ];
// 
// 		pNormals[ 3 * vertIx[ 2 ] + 0 ] += faceNormal[ 0 ];
// 		pNormals[ 3 * vertIx[ 2 ] + 1 ] += faceNormal[ 1 ];
// 		pNormals[ 3 * vertIx[ 2 ] + 2 ] += faceNormal[ 2 ];
// 
// 		vertexFaceCnt[ vertIx[ 0 ] ]++;
// 		vertexFaceCnt[ vertIx[ 1 ] ]++;
// 		vertexFaceCnt[ vertIx[ 2 ] ]++;
// 	}
// 
// 	for ( uint i = 0; i < vertexCount; i++ ) {
// 		pNormals[ 3 * i + 0 ] /= (float)vertexFaceCnt[ i ];
// 		pNormals[ 3 * i + 1 ] /= (float)vertexFaceCnt[ i ];
// 		pNormals[ 3 * i + 2 ] /= (float)vertexFaceCnt[ i ];
// 	}
// }
// 
// void InitializePointCloud(
// 	__in const uint indexCount,
// 	__in_ecount( indexCount ) const uint* pVertexIndices,
// 	__in const CapturingReality::Sfm::SfmReconstructionPoint* pVertices,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox,
// 	__out ScenePointCloud* pViewerPointCloud )
// {
// 	Buffer< float > vertexCoords;
// 	Buffer< float > vertexColors;
// 	vertexCoords.Allocate( indexCount * 3 );
// 	vertexColors.Allocate( indexCount * 4 );
// 
// 	ConvertToBuffers( indexCount, pVertexIndices, pVertices, vertexCoords.Data(), vertexColors.Data() );
// 
// 	GlPointCloud glPc;
// 	glGenVertexArrays( 1, &( glPc.arrayObject ) );
// 
// 	glGenBuffers( 2, &( glPc.vertexBuffer ) );
// 	glBindBuffer( GL_ARRAY_BUFFER, glPc.vertexBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 3 * indexCount * sizeof( float ), vertexCoords.Data(), GL_STATIC_DRAW );
// 
// 	glBindBuffer( GL_ARRAY_BUFFER, glPc.vertexColorBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 4 * indexCount * sizeof( float ), vertexColors.Data(), GL_STATIC_DRAW );
// 
// 	BoundingBox aabb;
// 	Vector4f pivot;
// 	GetBox( bbox, aabb, pivot );
// 
// 	pViewerPointCloud->SetPointCloud( indexCount, glPc, aabb );
// 	pViewerPointCloud->SetPivot( pivot.Elements() );
// }
// 
// void InitializePointCloud( 
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
// 	__in_ecount( vertexCount ) const DirectX::XMVECTOR* pVertexColors,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox,
// 	__out ScenePointCloud* pViewerPointCloud )
// {
// 	Buffer< float > vertices;
// 	Buffer< float > vertexColors;
// 	
// 	vertices.Allocate( 3*vertexCount );
// 	vertexColors.Allocate( 4*vertexCount );
// 
// 	ConvertVertices( vertexCount, pVertices, pVertexColors, vertices.Data(), vertexColors.Data() );
// 
// 	GlPointCloud glPc;
// 	glGenVertexArrays( 1, &( glPc.arrayObject ) );
// 
// 	glGenBuffers( 2, &( glPc.vertexBuffer ) );
// 	glBindBuffer( GL_ARRAY_BUFFER, glPc.vertexBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 3 * vertexCount * sizeof( float ), vertices.Data(), GL_STATIC_DRAW );
// 
// 	glBindBuffer( GL_ARRAY_BUFFER, glPc.vertexColorBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 4 * vertexCount * sizeof( float ), vertexColors.Data(), GL_STATIC_DRAW );
// 
// 	BoundingBox aabb;
// 	Vector4f pivot;
// 	GetBox( bbox, aabb, pivot );
// 
// 	pViewerPointCloud->SetPointCloud( vertexCount, glPc, aabb );
// 	pViewerPointCloud->SetPivot( pivot.Elements() );
// }
// 
// void InitializeMeshPart(
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
// 	__in const uint faceCount,
// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox,
// 	__out SceneMeshPart* pPart )
// {
// 	UNREFERENCED_PARAMETER( bbox );
// 
// 	Buffer< float > vertexCoords;
// 	vertexCoords.Allocate( 3 * vertexCount );
// 	GetVertexCoords( vertexCount, pVertices, vertexCoords.Data() );
// 
// 	Buffer< float > vertexNormals;
// 	vertexNormals.Allocate( 3 * vertexCount );
// 	GetVertexNormals( vertexCount, vertexCoords.Data(), faceCount, pFaces, vertexNormals.Data() );
// 
// 	Buffer <float> texCoords;
// 	texCoords.Allocate( 2 * vertexCount );
// 	ByteFill( 0, 2 * vertexCount, texCoords.Data() );
// 
// 	Buffer< float > vertexColors;
// 	vertexColors.Allocate( 4 * vertexCount );
// 	for ( uint i = 0; i < vertexCount; i++ ) {
// 		for ( uint j = 0; j < 3; j++ ) {
// 			vertexColors[ 4 * i + j ] = 0.5f;
// 		}
// 		vertexColors[ 4 * i + 3 ] = 1.0f;
// 	}
// 
// 	GlMesh gpuMesh;
// 	glGenVertexArrays( 1, &gpuMesh.arrayObject );
// 
// 	glGenBuffers( 1, &gpuMesh.vertexBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * 3 * sizeof( float ), vertexCoords.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &gpuMesh.vertexNormalBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexNormalBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * 3 * sizeof( float ), vertexNormals.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &gpuMesh.vertexColorBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexColorBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * 4 * sizeof( float ), vertexColors.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &gpuMesh.textureCoordinateBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.textureCoordinateBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * 2 * sizeof( float ), texCoords.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &gpuMesh.polygonIndexBuffer );
// 	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gpuMesh.polygonIndexBuffer );
// 	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 3 * faceCount * sizeof( uint ), pFaces, GL_STATIC_DRAW );
// 
// 	BoundingBox aabb;
// 	Vector4f pivot;
// 	GetBox( vertexCount, vertexCoords.Data(), aabb, pivot );
// 
// 	MeshInfo info;
// 	info.faceCount = faceCount;
// 	info.vertexCount = vertexCount;
// 	info.hasNormals = true;
// 	info.hasTextureCoordinates = false;
// 	info.hasVertexColors = true;
// 
// 	pPart->SetMesh( gpuMesh, info, aabb );
// 	pPart->SetPivot( pivot.Elements() );
// }
// 
// void InitializeMeshPart(
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
// 	__in_ecount( vertexCount ) const DirectX::XMVECTOR* pVertexColors,
// 	__in const uint faceCount,
// 	__in_ecount( faceCount ) const Triangle* pTriangles,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox,
// 	__out SceneMeshPart* pPart )
// {
// 	UNREFERENCED_PARAMETER( bbox );
// 
// 	Buffer< float > vertices;
// 	Buffer< float > vertexColors;
// 	Buffer< float > vertexNormals;
// 	Buffer <float> texCoords;
// 
// 	vertices.Allocate( 3 * vertexCount );
// 	vertexColors.Allocate( 4 * vertexCount );
// 	vertexNormals.Allocate( 3 * vertexCount );
// 	texCoords.Allocate( 2 * vertexCount );
// 
// 	ConvertVertices( vertexCount, pVertices, pVertexColors, vertices.Data(), vertexColors.Data() );
// 	GetVertexNormals( vertexCount, vertices.Data(), faceCount, pTriangles, vertexNormals.Data() );
// 	ByteFill( 0, 2 * vertexCount, texCoords.Data() );
// 
// 	GlMesh glMesh;
// 	glGenVertexArrays( 1, &( glMesh.arrayObject ) );
// 
// 	glGenBuffers( 1, &( glMesh.vertexBuffer ) );
// 	glBindBuffer( GL_ARRAY_BUFFER, glMesh.vertexBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 3 * vertexCount * sizeof( float ), vertices.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &glMesh.vertexNormalBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, glMesh.vertexNormalBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 3 * vertexCount * sizeof( float ), vertexNormals.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &glMesh.vertexColorBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, glMesh.vertexColorBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 4 * vertexCount * sizeof( float ), vertexColors.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &glMesh.textureCoordinateBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, glMesh.textureCoordinateBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * 2 * sizeof( float ), texCoords.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &glMesh.polygonIndexBuffer );
// 	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, glMesh.polygonIndexBuffer );
// 	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 3 * faceCount * sizeof( uint ), pTriangles, GL_STATIC_DRAW );
// 
// 	BoundingBox aabb;
// 	Vector4f pivot;
// 	GetBox( vertexCount, vertices.Data(), aabb, pivot );
// 
// 	MeshInfo info;
// 	info.faceCount = faceCount;
// 	info.vertexCount = vertexCount;
// 	info.hasNormals = true;
// 	info.hasTextureCoordinates = false;
// 	info.hasVertexColors = true;
// 
// 	pPart->SetMesh( glMesh, info, aabb );
// 	pPart->SetPivot( pivot.Elements() );
// }
// 
// void ConvertModelGeometry( 
// 	__in const uint vertexCount,
// 	__in_ecount( 3 * vertexCount ) const float* pVertices,
// 	__in_ecount( vertexCount ) const TextureCoordinate* pVertexTextureCoords,
// 	__in const uint faceCount,
// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox,
// 	__out SceneMeshPart* pPart ) 
// {
// 	UNREFERENCED_PARAMETER( bbox );
// 
// 	Buffer< float > vertexNormals;
// 	vertexNormals.Allocate( 3 * vertexCount );
// 	GetVertexNormals( vertexCount, pVertices, faceCount, pFaces, vertexNormals.Data() );
// 
// 	Buffer< float > vertexColors;
// 	vertexColors.Allocate( 4 * vertexCount );
// 	ByteFill( 0, 4 * vertexCount, vertexColors.Data() );
// 
// 	GlMesh gpuMesh;
// 	glGenVertexArrays( 1, &gpuMesh.arrayObject );
// 
// 	glGenBuffers( 1, &gpuMesh.vertexBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * 3 * sizeof( float ), pVertices, GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &gpuMesh.vertexNormalBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexNormalBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * 3 * sizeof( float ), vertexNormals.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &gpuMesh.vertexColorBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexColorBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * 4 * sizeof( float ), vertexColors.Data(), GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &gpuMesh.textureCoordinateBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.textureCoordinateBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, vertexCount * sizeof( TextureCoordinate ), pVertexTextureCoords, GL_STATIC_DRAW );
// 
// 	glGenBuffers( 1, &gpuMesh.polygonIndexBuffer );
// 	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gpuMesh.polygonIndexBuffer );
// 	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 3 * faceCount * sizeof( uint ), pFaces, GL_STATIC_DRAW );
// 
// 	BoundingBox aabb;
// 	Vector4f pivot;
// 	GetBox( vertexCount, pVertices, aabb, pivot );
// 
// 	MeshInfo info;
// 	info.hasNormals = true;
// 	info.hasTextureCoordinates = true;
// 	info.hasVertexColors = false;
// 	info.vertexCount = vertexCount;
// 	info.faceCount = faceCount;
// 
// 	pPart->SetMesh( gpuMesh, info, aabb );
// 	pPart->SetPivot( pivot.Elements() );
// }
// 
// void ConvertModelTexture(
// 	__in CapturingReality::IImageBuffer* pTexture,
// 	__out SceneMeshPart* pPart )
// {
// 	Texture* pViewerTexture = NULL;
// 	ViewerApplication::GetApp()->GetNewTexture( &pViewerTexture );
// 
// 	GLuint glTex = 0;
// 	glGenTextures( 1, &glTex );
// 	glBindTexture( GL_TEXTURE_2D, glTex );
// 	glTexImage2D( GL_TEXTURE_2D, 0, GL_SRGB, pTexture->GetWidth(), pTexture->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, pTexture->GetBuffer() );
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
// 
// 	pViewerTexture->SetGlTexture( glTex );
// 	pPart->SetTexture( pViewerTexture );
// }
// 
// // --------------------------------------------------------------------------------------------------------------------
// // --------------------------------------------------------------------------------------------------------------------
// 
// void ViewerLauncher::InitializeSparsePointCloud(
// 	__in const uint indexCount,
// 	__in_ecount( indexCount ) const uint* pVertexIndices,
// 	__in const CapturingReality::Sfm::SfmReconstructionPoint* pVertices,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox )
// {
// 	ViewerApplication* pViewer = ViewerApplication::GetApp();
// 	pViewer->Initialize( 0, NULL );
// 
// 	ProgressBar& progressBar = pViewer->GetProgressBar();
// 	progressBar.BeginSection( AlgID_ViewerDataConversion, 1.0f );
// 
// 	ScenePointCloud* pViewerPointCloud = new ScenePointCloud();
// 	InitializePointCloud( indexCount, pVertexIndices, pVertices, bbox, pViewerPointCloud );
// 
// 	progressBar.EndSection( S_OK, NULL );
// 
// 	pViewer->ShowPointCloud( pViewerPointCloud );
// 	pViewerPointCloud = NULL;
// }
// 
// 
// void ViewerLauncher::InitializeDensePointCloud(
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
// 	__in_ecount( vertexCount ) const DirectX::XMVECTOR* pVertexColors,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox )
// {
// 	ViewerApplication* pViewer = ViewerApplication::GetApp();
// 	pViewer->Initialize( 0, NULL );
// 
// 	ProgressBar& progressBar = pViewer->GetProgressBar();
// 	progressBar.BeginSection( AlgID_ViewerDataConversion, 1.0f );
// 
// 	ScenePointCloud* pPointCloud = new ScenePointCloud();
// 	InitializePointCloud( vertexCount, pVertices, pVertexColors, bbox, pPointCloud );
// 
// 	progressBar.EndSection( S_OK, NULL );
// 
// 	pViewer->ShowPointCloud( pPointCloud );
// 	pPointCloud = NULL;
// }
// 
// void ViewerLauncher::InitializeUntexturedModel(
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
// 	__in const uint faceCount,
// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox )
// {
// 	ViewerApplication* pViewer = ViewerApplication::GetApp();
// 	pViewer->Initialize( 0, NULL );
// 
// 	ProgressBar& progressBar = pViewer->GetProgressBar();
// 	progressBar.BeginSection( AlgID_ViewerDataConversion, 1.0f );
// 
// 	SceneMesh* pViewerMesh = new SceneMesh();
// 	pViewerMesh->SetPartsCount( 1 );
// 
// 	SceneMeshPart* pPart = pViewerMesh->GetPartPtr( 0 );
// 	InitializeMeshPart( vertexCount, pVertices, faceCount, pFaces, bbox, pPart );
// 	pPart->MoveTo( Vector3f( { 0,0,0 } ) );
// 
// 	progressBar.EndSection( S_OK, NULL );
// 
// 	pViewer->ShowMesh( pViewerMesh );
// 	pViewerMesh = NULL;
// }
// 
// void ViewerLauncher::InitializeColorizedModel(
// 	__in const uint vertexCount,
// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
// 	__in_ecount( vertexCount ) const DirectX::XMVECTOR* pVertexColors,
// 	__in const uint faceCount,
// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox )
// {
// 	ViewerApplication* pViewer = ViewerApplication::GetApp();
// 	pViewer->Initialize( 0, NULL );
// 
// 	ProgressBar& progressBar = pViewer->GetProgressBar();
// 	progressBar.BeginSection( AlgID_ViewerDataConversion, 1.0f );
// 
// 	SceneMesh* pViewerMesh = new SceneMesh();
// 	pViewerMesh->SetPartsCount( 1 );
// 
// 	SceneMeshPart* pPart = pViewerMesh->GetPartPtr( 0 );
// 	InitializeMeshPart( vertexCount, pVertices, pVertexColors, faceCount, pFaces, bbox, pPart );
// 	pPart->MoveTo( Vector3f( { 0,0,0 } ) );
// 
// 	progressBar.EndSection( S_OK, NULL );
// 
// 	pViewer->ShowMesh( pViewerMesh );
// 	pViewerMesh = NULL;
// }
// 
// void ViewerLauncher::InitializeTexturedModel(
// 	__in const uint vertexCount,
// 	__in_ecount( 3 * vertexCount ) const float* pVertices,
// 	__in_ecount( vertexCount ) const TextureCoordinate* pVertexTextureCoords,
// 	__in const uint faceCount,
// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
// 	__in CapturingReality::IImageBuffer* pTexture,
// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox )
// {
// 	ViewerApplication* pViewer = ViewerApplication::GetApp();
// 	pViewer->Initialize( 0, NULL );
// 
// 	ProgressBar& progressBar = pViewer->GetProgressBar();
// 	progressBar.BeginSection( AlgID_ViewerDataConversion, 1.0f );
// 
// 	SceneMesh* pViewerMesh = new SceneMesh();
// 	pViewerMesh->SetPartsCount( 1 );
// 
// 	SceneMeshPart* pPart = pViewerMesh->GetPartPtr( 0 );
// 	ConvertModelGeometry( vertexCount, pVertices, pVertexTextureCoords, faceCount, pFaces, bbox, pPart );
// 	ConvertModelTexture( pTexture, pPart );
// 	pPart->MoveTo( Vector3f( { 0,0,0 } ) );
// 
// 	progressBar.EndSection( S_OK, NULL );
// 
// 	pViewer->ShowMesh( pViewerMesh );
// 	pViewerMesh = NULL;
// }

void ViewerLauncher::InitializeEmpty( )
{
	ViewerApplication* pViewer = ViewerApplication::GetApp( );
	pViewer->Initialize( 0, NULL );
}

void ViewerLauncher::LaunchViewer() {
	ViewerApplication::GetApp()->Start();
}