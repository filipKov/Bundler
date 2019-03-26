#version 400

const uint PASS_DEPTH = 0;
const uint PASS_AO_MAP = 1;
const uint PASS_BLUR = 2;

// ------------------------------------------------
// Uniforms
// ------------------------------------------------
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;

uniform uint renderPass;

// ------------------------------------------------
// Inputs/Outputs
// ------------------------------------------------ 
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out VertexData {
    vec3 normal;
    vec4 position;
} outputVertex;

// ------------------------------------------------
// Functions
// ------------------------------------------------ 

void main () {

	outputVertex.position = modelViewMatrix * vec4( vertexPosition, 1.0 );
	gl_Position = projectionMatrix * outputVertex.position;

	if ( renderPass == PASS_DEPTH ) {
		outputVertex.normal = normalMatrix * vertexNormal;
	}
	
}