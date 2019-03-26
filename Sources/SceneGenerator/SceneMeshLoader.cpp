#include "stdafx.h"

#include "ObjLoader.h"

#include "RenderingLib.h"

#include "Transformable.h"
#include "SceneObject.h"

#include "Texture.h"
#include "SceneMeshPart.h"
#include "SceneMesh.h"

#include "SceneMeshLoader.h"

using namespace Containers;

void ConvertVertexPositions( __in const size_t vertexCount, __in_ecount( vertexCount ) const objl::Vertex* pVertices, __out GlMesh& gpuMesh, __out MeshInfo& info ) {
	info.vertexCount = (uint)vertexCount;
	
	Utils::Containers::Buffer< objl::Vector3f > vertexCoords;
	vertexCoords.Allocate( vertexCount );

	objl::Vector3f* pDstVert = vertexCoords.Data();

	for ( uint i = 0; i < vertexCount; i++ ) {
		*pDstVert = pVertices->Position;
		pDstVert++;
		pVertices++;
	}

	glGenBuffers( 1, &gpuMesh.vertexBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, vertexCount * sizeof( objl::Vector3f ), vertexCoords.Data(), GL_STATIC_DRAW );
}

void GetAxisAlignedBoundingBox( __in const size_t vertexCount, __in_ecount( vertexCount ) const objl::Vertex* pVertices, __out BoundingBox& bbox ) {
	ShallowCopy( &pVertices->Position.X, 3, bbox.minVert.Elements() );
	bbox.maxVert = bbox.minVert;

	for ( uint i = 1; i < vertexCount; i++ ) {
		const objl::Vector3f& pos = pVertices[ i ].Position;

		if ( pos.X < bbox.minVert[ 0 ] ) {
			bbox.minVert[ 0 ] = pos.X;
		}
		else if ( pos.X > bbox.maxVert[ 0 ] ) {
			bbox.maxVert[ 0 ] = pos.X;
		}

		if ( pos.Y < bbox.minVert[ 1 ] ) {
			bbox.minVert[ 1 ] = pos.Y;
		}
		else if ( pos.Y > bbox.maxVert[ 1 ] ) {
			bbox.maxVert[ 1 ] = pos.Y;
		}

		if ( pos.Z < bbox.minVert[ 2 ] ) {
			bbox.minVert[ 2 ] = pos.Z;
		}
		else if ( pos.Z > bbox.maxVert[ 2 ] ) {
			bbox.maxVert[ 2 ] = pos.Z;
		}
	}
}

void ConvertVertexNormals( __in const size_t vertexCount, __in_ecount( vertexCount ) const objl::Vertex* pVertices, __out GlMesh& gpuMesh, __out MeshInfo& info ) {
	info.hasNormals = true;

	Utils::Containers::Buffer< objl::Vector3f > vertexNormals;
	vertexNormals.Allocate( vertexCount );

	objl::Vector3f* pDstVert = vertexNormals.Data();

	for ( uint i = 0; i < vertexCount; i++ ) {
		*pDstVert = pVertices->Normal;
		pDstVert++;
		pVertices++;
	}

	glGenBuffers( 1, &gpuMesh.vertexNormalBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexNormalBuffer );
	glBufferData( GL_ARRAY_BUFFER, vertexCount * sizeof( objl::Vector3f ), vertexNormals.Data(), GL_STATIC_DRAW );
}

void ConvertTextureCoordinates( __in const size_t vertexCount, __in_ecount( vertexCount ) const objl::Vertex* pVertices, __out GlMesh& gpuMesh, __out MeshInfo& info ) {
	info.hasTextureCoordinates = true;

	Utils::Containers::Buffer< objl::Vector2 > texCoords;
	texCoords.Allocate( vertexCount );

	objl::Vector2* pDstVert = texCoords.Data();

	for ( uint i = 0; i < vertexCount; i++ ) {
		*pDstVert = pVertices->TextureCoordinate;
		pDstVert++;
		pVertices++;
	}

	glGenBuffers( 1, &gpuMesh.textureCoordinateBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.textureCoordinateBuffer );
	glBufferData( GL_ARRAY_BUFFER, vertexCount * sizeof( objl::Vector2 ), texCoords.Data(), GL_STATIC_DRAW );

	Buffer< float > vertexColors;
	vertexColors.Allocate( 4 * vertexCount );
	ByteFill( 0, 4 * vertexCount, vertexColors.Data() );
	glGenBuffers( 1, &gpuMesh.vertexColorBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, gpuMesh.vertexColorBuffer );
	glBufferData( GL_ARRAY_BUFFER, 4 * vertexCount * sizeof( float ), vertexColors.Data(), GL_STATIC_DRAW );

	info.hasVertexColors = false;
}

void ConvertFaceIndices( __in const size_t indicesCnt, __in_ecount( faceCount ) const uint* pIndices, __out GlMesh& gpuMesh, __out MeshInfo& info ) {
	info.faceCount = (uint)indicesCnt / 3;

	glGenBuffers( 1, &gpuMesh.polygonIndexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gpuMesh.polygonIndexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indicesCnt * sizeof( uint ), pIndices, GL_STATIC_DRAW );
}

void CreateVertexArrayObject( __inout GlMesh& mesh ) {
	glGenVertexArrays( 1, &mesh.arrayObject );
	glBindVertexArray( mesh.arrayObject );
}

void ConvertMeshPart( __in const objl::Mesh* pInPart, __out SceneMeshPart* pOutPart ) {
	GlMesh gpuMesh;
	MeshInfo info;
	BoundingBox bbox;

	const size_t vertexCount = pInPart->Vertices.size();
	const objl::Vertex* pVertices = pInPart->Vertices.data();

	ConvertVertexPositions( vertexCount, pVertices, gpuMesh, info );
	GetAxisAlignedBoundingBox( vertexCount, pVertices, bbox );
	
	ConvertVertexNormals( vertexCount, pVertices, gpuMesh, info );
	ConvertTextureCoordinates( vertexCount, pVertices, gpuMesh, info );
	
	ConvertFaceIndices( pInPart->Indices.size(), pInPart->Indices.data(), gpuMesh, info );
	
	CreateVertexArrayObject( gpuMesh );

	pOutPart->SetMesh( gpuMesh, info, bbox );
}

void SceneMeshLoader::Load( __in const char* pFilename, __out IModifiableMesh* pMesh ) {
	// Assimp::Importer* pImporter = new Assimp::Importer();
	// auto pScene = pImporter->ReadFile( pFilename, 0 ); // aiProcessPreset_TargetRealtime_Fast );
	// 
	// if ( !pScene ) {
	// 	throw Exception( pImporter->GetErrorString() );
	// }
	// 
	// if ( pScene->HasMeshes() ) {
	// 	pMesh->SetPartsCount( pScene->mNumMeshes );
	// 	for ( uint meshI = 0; meshI < pScene->mNumMeshes; meshI++ ) {
	// 		ConvertMeshPart( pScene->mMeshes[ meshI ], pMesh->GetPartPtr( meshI ) );
	// 	}
	// }
	// 
	// pImporter->FreeScene();
	// delete pImporter;

	objl::Loader importer;
	if ( importer.LoadFile( std::string( pFilename ) ) && importer.LoadedMeshes.size() > 0 )
	{
		pMesh->SetPartsCount( (uint)importer.LoadedMeshes.size() );
		for ( size_t partI = 0; partI < importer.LoadedMeshes.size(); partI++ ) {
			::ConvertMeshPart( &importer.LoadedMeshes[ partI ], pMesh->GetPartPtr( (uint)partI ) );
		}
	}
	else {
		throw Exception( "Obj import failed" );
	}

}

// void SceneMeshLoader::ConvertMeshPart( __in const aiMesh* pTmpMesh, __out SceneMeshPart* pPart ) {
// 	GlMesh gpuMesh;
// 	MeshInfo info;
// 	BoundingBox bbox;
// 
// 	if ( pTmpMesh->HasPositions() ) {
// 		LoadVertices( pTmpMesh, gpuMesh, info );
// 		GetAxisAlignedBoundingBox( pTmpMesh, bbox );
// 		// GetPivotPoint( pTmpMesh, mesh.mPivotPoint );
// 	}
// 
// 	if ( pTmpMesh->HasFaces() ) {
// 		LoadFaces( pTmpMesh, gpuMesh, info );
// 	}
// 
// 	if ( pTmpMesh->HasNormals() ) {
// 		LoadVertexNormals( pTmpMesh, gpuMesh, info );
// 	}
// 
// 	if ( pTmpMesh->HasTextureCoords( 0 ) ) {
// 		LoadTextureCoordinates( pTmpMesh, gpuMesh, info );
// 	}
// 
// 	CreateVertexArrayObject( gpuMesh );
// 
// 	pPart->SetMesh( gpuMesh, info, bbox );
// }
// 
// void SceneMeshLoader::LoadVertices( __in const aiMesh* pTmpMesh, __out GlMesh& mesh, __out MeshInfo& info ) {
// 	info.vertexCount = pTmpMesh->mNumVertices;
// 	glGenBuffers( 1, &mesh.vertexBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 3 * info.vertexCount * sizeof( float ), pTmpMesh->mVertices, GL_STATIC_DRAW );
// }
// 
// void SceneMeshLoader::GetPivotPoint( __in const aiMesh* pMesh, __out Vector3f& pivot ) {
// 	const aiVector3fD* pVertex = pMesh->mVertices;
// 	float& x = pivot[0];
// 	float& y = pivot[1];
// 	float& z = pivot[2];
// 
// 	for ( uint i = 0; i < pMesh->mNumVertices; i++ ) {
// 		x += pVertex->x;
// 		y += pVertex->y;
// 		z += pVertex->z;
// 
// 		// printf( "Vertex %u: [%f, %f, %f]\n", i, pVertex->x, pVertex->y, pVertex->z );
// 
// 		pVertex++;
// 	}
// 	pivot *= 1.0f / pMesh->mNumVertices;
// }
// 
// void SceneMeshLoader::GetAxisAlignedBoundingBox( __in const aiMesh* pMesh, __out BoundingBox& bbox ) {
// 	const aiVector3fD* pVertex = pMesh->mVertices;
// 	bbox.minVert = Vector3f( { pVertex->x, pVertex->y, pVertex->z } );
// 	bbox.maxVert = bbox.minVert;
// 	pVertex++;
// 
// 	float* pMinX = &bbox.minVert[ 0 ];
// 	float* pMinY = &bbox.minVert[ 1 ];
// 	float* pMinZ = &bbox.minVert[ 2 ];
// 
// 	float* pMaxX = &bbox.maxVert[ 0 ];
// 	float* pMaxY = &bbox.maxVert[ 1 ];
// 	float* pMaxZ = &bbox.maxVert[ 2 ];
// 
// 	for ( uint i = 1; i < pMesh->mNumVertices; i++ ) {
// 		const float x = pVertex->x;
// 		const float y = pVertex->y;
// 		const float z = pVertex->z;
// 
// 		if ( x < *pMinX ) {
// 			*pMinX = x;
// 		}
// 		else if ( x > *pMaxX ) {
// 			*pMaxX = x;
// 		}
// 
// 		if ( y < *pMinY ) {
// 			*pMinY = y;
// 		}
// 		else if ( y > *pMaxY ) {
// 			*pMaxY = y;
// 		}
// 
// 		if ( z < *pMinZ ) {
// 			*pMinZ = z;
// 		}
// 		else if ( z > *pMaxZ ) {
// 			*pMaxZ = z;
// 		}
// 
// 		pVertex++;
// 	}
// }
// 
// void SceneMeshLoader::LoadVertexNormals( __in const aiMesh* pTmpMesh, __out GlMesh& mesh, __out MeshInfo& info ) {
// 	info.hasNormals = true;
// 	glGenBuffers( 1, &mesh.vertexNormalBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexNormalBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 3 * info.vertexCount * sizeof( float ), pTmpMesh->mNormals, GL_STATIC_DRAW );
// }
// 
// void SceneMeshLoader::LoadTextureCoordinates( __in const aiMesh* pTmpMesh, __out GlMesh& mesh, __out MeshInfo& info ) {
// 	info.hasTextureCoordinates = true;
// 
// 	Buffer< float > uvBuffer;
// 	uvBuffer.Allocate( 2 * info.vertexCount );
// 
// 	auto* pSrc = pTmpMesh->mTextureCoords[0];
// 	float* pDst = uvBuffer.Data();
// 
// 	for ( uint vertI = 0; vertI < info.vertexCount; vertI++ ) {
// 		memcpy( pDst, pSrc, 2 * sizeof( float ) );
// 		pSrc++;
// 		pDst += 2;
// 	}
// 
// 	glGenBuffers( 1, &mesh.textureCoordinateBuffer );
// 	glBindBuffer( GL_ARRAY_BUFFER, mesh.textureCoordinateBuffer );
// 	glBufferData( GL_ARRAY_BUFFER, 2 * info.vertexCount * sizeof( float ), uvBuffer.Data(), GL_STATIC_DRAW );
// }
// 
// void SceneMeshLoader::LoadFaces( __in const aiMesh* pTmpMesh, __out GlMesh& mesh, __out MeshInfo& info ) {
// 	info.faceCount = pTmpMesh->mNumFaces;
// 
// 	Buffer< uint > indexBuffer;
// 	indexBuffer.Allocate( 3 * info.faceCount );
// 
// 	uint* pDst = indexBuffer.Data();
// 
// 	for ( uint faceI = 0; faceI < info.faceCount; faceI++ ) {
// 		const aiFace& face = pTmpMesh->mFaces[faceI];
// 		
// 		if ( face.mNumIndices != 3 ) {
// 			continue; // skip non-triangle faces
// 		}
// 
// 		memcpy( pDst, face.mIndices, 3 * sizeof( uint ) );
// 		pDst += 3;
// 	}
// 
// 	glGenBuffers( 1, &mesh.polygonIndexBuffer );
// 	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.polygonIndexBuffer );
// 	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 3 * info.faceCount * sizeof( uint ), indexBuffer.Data(), GL_STATIC_DRAW );
// }
// 
// void SceneMeshLoader::CreateVertexArrayObject( __inout GlMesh& mesh ) {
// 	glGenVertexArrays( 1, &mesh.arrayObject );
// 	glBindVertexArray( mesh.arrayObject );
// }
