#version 400

layout(location = 0) in vec3 vertexPosition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main () {
	vec4 outPosition =  viewMatrix  * vec4 ( vertexPosition, 1.0);
	gl_Position = projectionMatrix * outPosition;
}