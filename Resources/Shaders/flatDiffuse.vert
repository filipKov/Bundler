#version 400

const uint MAX_LIGHTS = 4;

struct Material {
	vec4 diffuseColor;
	bool isTextured;
	bool hasVertexColors;
	sampler2D diffuseTexture;

	vec4 specularColor;
	float specularIntensity;
};

// ------------------------------------------------
// Uniforms
// ------------------------------------------------
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

uniform Material material;

// ------------------------------------------------
// Inputs/Outputs
// ------------------------------------------------ 
layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec2 vertexTextureCoord;

out VertexData {
    vec4 position;
	vec4 vertexColor;
	vec2 textureCoord;
} outputVertex;

// ------------------------------------------------
// Functions
// ------------------------------------------------
 
void main () {
	//compute the vertex position  in camera space
	outputVertex.position = modelViewMatrix * vec4 (vertexPosition, 1.0);
	vec4 projectedPosition = projectionMatrix * outputVertex.position;
	
	if ( material.isTextured ) {
		outputVertex.textureCoord = vertexTextureCoord;
	} else {
		outputVertex.textureCoord = vec2( 0.0, 0.0 );
	}

	if ( material.hasVertexColors ) {
		outputVertex.vertexColor = vertexColor;
	} else {
		outputVertex.vertexColor = vec4( 0.5, 0.5, 0.5, 1.0 );
	}
	
	gl_Position = projectedPosition;
}