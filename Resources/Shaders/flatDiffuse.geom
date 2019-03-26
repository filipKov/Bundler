#version 400

// ------------------------------------------------
// Inputs/Outputs
// ------------------------------------------------ 
layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;
 
in VertexData {
	vec4 position;
	vec4 vertexColor;
	vec2 textureCoord;
}  inputVertices[3];
 
out VertexData {
    vec3 normal;
    vec4 position;
	vec4 vertexColor;
	vec2 textureCoord;
}  outputVertex;

// ------------------------------------------------
// Functions
// ------------------------------------------------ 
 void main() {
	vec3 dir20 = vec3( inputVertices[2].position ) - vec3( inputVertices[0].position );
	vec3 dir10 = vec3( inputVertices[1].position ) - vec3( inputVertices[0].position );
	vec3 faceNormal = cross( dir10, dir20 );
	
	for ( int i = 0; i < 3; i++ ) {
		gl_Position = gl_in[i].gl_Position;
		outputVertex.normal = faceNormal;
		outputVertex.position = inputVertices[i].position;
		outputVertex.textureCoord = inputVertices[i].textureCoord;
		outputVertex.vertexColor = inputVertices[i].vertexColor;
		EmitVertex();
	}

	EndPrimitive();
}