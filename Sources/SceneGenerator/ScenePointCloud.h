#pragma once

class ScenePointCloud : public SceneObject, public RenderablePointCloud
{
public:
	
	ScenePointCloud();
	~ScenePointCloud();

	IRenderable* GetRenderable() override;

protected:

	Matrix<float, 4, 4> GetWorldTransform() const override;

};

