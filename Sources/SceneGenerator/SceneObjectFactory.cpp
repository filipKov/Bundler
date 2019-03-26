#include "stdafx.h"

#include "RenderingLib.h"

#include "Transformable.h"
#include "SceneObject.h"

#include "Texture.h"
#include "TextureCache.h"

#include "SceneMeshPart.h"
#include "SceneMesh.h"
#include "SceneMeshLoader.h"

#include "ScenePointCloud.h"

#include "SceneObjectFactory.h"

SceneObject* LoadMesh( __in const MeshLoadParams& params, __inout TextureCache& textureCache ) {
	if ( params.pMeshFile == NULL ) {
		throw InvalidArgumentException( GET_VARIABLE_NAME( params.pMeshFile ), ExceptionReasons::NULLPTR );
	}

	SceneMesh* pMesh = new SceneMesh();
	SceneMeshLoader::Load( params.pMeshFile, pMesh );

	for ( uint i = 0; i < params.textureCount; i++ ) {
		const char* pTexPath = params.diffuseTextures[ i ];
		const char* pTextureName = Utils::FileSystem::GetFilenameStart( pTexPath );


		const Texture* pTex = nullptr;
		if ( !textureCache.Get( pTextureName, &pTex ) ) {
			Texture newTexture( pTexPath );
			textureCache.RegisterItem( pTextureName, newTexture );
			textureCache.Get( pTextureName, &pTex );
		}

		pMesh->GetPartPtr( i )->SetTexture( pTex );
	}

	return pMesh;
}