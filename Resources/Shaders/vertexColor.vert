#version 400

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

uniform bool hasVertexColors;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec4 fragmentColor;

void main () {

	//compute the vertex position  in camera space
	vec4 cameraProjectedVertex = modelViewMatrix * vec4( vertexPosition, 1.0 );

	if ( hasVertexColors ) {
		fragmentColor = vec4( vertexColor, 1.0 );
	} else {
		fragmentColor = vec4( 1.0, 1.0, 1.0, 1.0 );
	}
	
	gl_Position = projectionMatrix * cameraProjectedVertex;
}