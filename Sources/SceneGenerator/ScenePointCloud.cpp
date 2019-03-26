#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"
#include "RenderablePointCloud.h"

#include "Transformable.h"
#include "SceneObject.h"

#include "ScenePointCloud.h"


ScenePointCloud::ScenePointCloud()
{
}


ScenePointCloud::~ScenePointCloud()
{
}

IRenderable* ScenePointCloud::GetRenderable() {
	return this;
}

Matrix<float, 4, 4> ScenePointCloud::GetWorldTransform() const {
	return GetTransform();
}
