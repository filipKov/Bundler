#pragma once

class Scene {

public:

	Scene();
	~Scene();

	void Clear();

	Light* CreateLight( __in const LIGHT_TYPE type );

	Camera camera;
	std::vector< SceneObject* > objects;
	std::vector< Light* > lights;
	
	SceneObject* pSkybox;

	Grid3D* pGrid;

};