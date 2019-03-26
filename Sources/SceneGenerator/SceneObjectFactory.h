#pragma once

struct MeshLoadParams {
	const char* pMeshFile;
	
	uint textureCount;
	const char* diffuseTextures[MAX_COMPONENT_COUNT];
};

SceneObject* LoadMesh( __in const MeshLoadParams& params, __inout TextureCache& textureCache );

