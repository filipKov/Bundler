#include "stdafx.h"

#include "RenderingLib.h"

#include "Texture.h"
#include "TextureCache.h"

#include "Transformable.h"
#include "SceneObject.h"

#include "Scene.h"

Scene::Scene() {
	pSkybox = nullptr;
}

Scene::~Scene() {
	Clear();
}

void Scene::Clear() {
	for ( auto it = objects.begin(); it != objects.end(); it++ ) {
		if ( *it ) {
			delete ( *it );
		}
	}

	for ( auto it = lights.begin(); it != lights.end(); it++ ) {
		if ( *it ) {
			delete ( *it );
		}
	}

	if ( pSkybox ) {
		delete pSkybox;
	}

	if ( pGrid ) {
		delete pGrid;
	}

	camera = Camera();
}

Light* Scene::CreateLight( __in const LIGHT_TYPE type ) {
	Light* pLight = nullptr;
	
	switch ( type ) {
	case LIGHT_TYPE::DIRECTIONAL:
		pLight = new DirectionalLight();
		break;
	case LIGHT_TYPE::POINT:
		pLight = new PointLight();
		break;
	}
	
	if ( pLight ) {
		lights.push_back( pLight );
	}

	return pLight;
}