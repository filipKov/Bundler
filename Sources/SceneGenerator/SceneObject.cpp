// #include "stdafx.h"
// #include <vector>
// #include <map>
// 
// #include <glew.h>
// 
// #include "AlgebraLib.h"
// #include "glStructs.h"
// 
// #include "SceneMesh.h"
// #include "Texture.h"
// #include "TextureCache.h"
// #include "ObjectComponent.h"
// 
// #include "FileSystemHelpers.h"
// #include "SceneMeshLoader.h"
// 
// #include "Transformable.h"
// #include "SceneObject.h"
// 
// void SceneObject::Load( __in ObjectLoadParams& params, __inout TextureCache& texCache ) {
// 	SceneMeshLoader importer;
// 	importer.Load( params.pMeshFile, mComponents );
// 	
// 	for ( uint i = 0; i < params.textureCount; i++ ) {
// 		const char* pTexPath = params.diffuseTextures[i];
// 		const char* pTextureName = GetFilenameStart( pTexPath );
// 
// 
// 		const Texture* pTex = nullptr;
// 		if ( !texCache.Get( pTextureName, &pTex ) ) {
// 			Texture newTexture( pTexPath );
// 			texCache.RegisterItem( pTextureName, newTexture );
// 			texCache.Get( pTextureName, &pTex );
// 		}
// 		mComponents[i].SetTexture( *pTex );
// 	}
// 
// 	for ( uint i = 0; i < mComponents.size(); i++ ) {
// 		mGlobalPivot += mComponents[i].GetPivotPoint();
// 	}
// 	mGlobalPivot *= 1.0f / mComponents.size();
// }
// 
// const Vector4f& SceneObject::GetPivotPoint() const {
// 	return mGlobalPivot;
// }
// 
// const uint SceneObject::GetComponentCount() const {
// 	return (uint)mComponents.size();
// }
// 
// const ObjectComponent& SceneObject::GetComponent( __in const uint ix ) const {
// 	return mComponents[ix];
// }
// 
// // Manipulation
// void SceneObject::Move( __in const Vector3f& translation ) {
// 	float dx = translation[0];
// 	float dy = translation[1];
// 	float dz = translation[2];
// 
// 	mGlobalPivot += Vector4f( { dx, dy, dz, 0.0f } );
// 	for ( auto componentIt = mComponents.begin(); componentIt != mComponents.end(); componentIt++ ) {
// 		componentIt->Move( translation );
// 	}
// }
// 
// void SceneObject::MoveTo( __in const Vector3f& position ) {
// 	Vector3f translation( { position[0] - mGlobalPivot[0], position[1] - mGlobalPivot[1], position[2] - mGlobalPivot[2] } );
// 	Move( translation );
// }
// 
// void SceneObject::Rotate( __in const float angle, __in const Vector3f& rotVector ) {
// 	float px = mGlobalPivot[0];
// 	float py = mGlobalPivot[1];
// 	float pz = mGlobalPivot[2];
// 
// 	Matrix4x4f transformMatrix;
// 	transformMatrix.SetIdentity();
// 	transformMatrix.Translate( -px, -py, -pz );
// 	transformMatrix.Rotate( angle, rotVector[0], rotVector[1], rotVector[2] );
// 	transformMatrix.Translate( px, py, pz );
// 
// 	mGlobalPivot = transformMatrix * mGlobalPivot;
// 	for ( auto componentIt = mComponents.begin(); componentIt != mComponents.end(); componentIt++ ) {
// 		componentIt->Transform( transformMatrix );
// 	}
// }
// 
// void SceneObject::Scale( __in const Vector3f& scale ) {
// 	float px = mGlobalPivot[0];
// 	float py = mGlobalPivot[1];
// 	float pz = mGlobalPivot[2];
// 
// 	Matrix4x4f transformMatrix;
// 	transformMatrix.SetIdentity();
// 	transformMatrix.Translate( -px, -py, -pz );
// 	transformMatrix.Scale( scale[0], scale[1], scale[2] );
// 	transformMatrix.Translate( px, py, pz );
// 
// 	for ( auto componentIt = mComponents.begin(); componentIt != mComponents.end(); componentIt++ ) {
// 		componentIt->Transform( transformMatrix );
// 	}
// }
// 
// // Rendering + Shaders
// void SceneObject::RegisterGlobalShader( __in const GLuint shaderId ) {
// 	for ( auto componentIt = mComponents.begin(); componentIt != mComponents.end(); componentIt++ ) {
// 		componentIt->RegisterShader( shaderId );
// 	}
// }

#include "stdafx.h"

#include "RenderingLib.h"

#include "Transformable.h"
#include "SceneObject.h"
