#version 400

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 tex_coord;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;

out vec3 normal;
out vec4 ecPos;
out vec2 texcoord;

void main () {

	//compute the vertex position  in camera space
	ecPos =  modelViewMatrix  * vec4 (vertex_position, 1.0);
	normal = normalMatrix * vertex_normal;
	texcoord = tex_coord;
	
	gl_Position = projectionMatrix * ecPos;
}