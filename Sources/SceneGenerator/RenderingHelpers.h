#pragma once

#include "UniformNames.h"

template < typename UniformFnc, typename ... DataTypes >
void LoadUniform( __in const GLuint shaderProgram, __in const char* pUniformName, __in UniformFnc fnc, __in const DataTypes ... data ) {
	int uniformLocation = glGetUniformLocation( shaderProgram, pUniformName );
	if ( uniformLocation > -1 ) {
		fnc( uniformLocation, data... );
	}
};

struct Viewport {
	int x;
	int y;
	uint width;
	uint height;

	float cameraFoV;
	float clipNear;
	float clipFar;
};

enum class SHADER_TYPE {
	ALBEDO,
	ALBEDO_POINT_CLOUD,
	SHADOW,
	AMBIENT_OCCLUSION,
	UNKNOWN
};

struct Shader {
	SHADER_TYPE type;
	GLuint id;
};

struct BoundingBox {
	Vector3f minVert;
	Vector3f maxVert;
};