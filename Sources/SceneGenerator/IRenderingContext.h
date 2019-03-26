#pragma once

class IRenderingContext {
public:

	virtual Shader GetShader() const = 0;

	virtual const Matrix4x4f& GetViewMatrix() const = 0;

	virtual const Matrix4x4f& GetProjectionMatrix() const = 0;

};