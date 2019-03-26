#version 400

const uint PASS_DEPTH_MAPS = 0;
const uint PASS_SHADOW_MAP = 1;

const uint MAX_LIGHTS = 8;

// ------------------------------------------------
// Uniforms
// ------------------------------------------------

// All passes
uniform uint renderPass;
uniform mat4 modelMatrix;

// Pass 0 ( depth maps )
uniform uint lightCount;
uniform uint lightIx;
uniform mat4 lightViewMats[MAX_LIGHTS];
uniform mat4 lightProjectionMats[MAX_LIGHTS];

// Pass 1 ( shadow map )
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

// ------------------------------------------------
// Inputs/Outputs
// ------------------------------------------------ 
layout(location = 0) in vec3 vertexPosition;

out vec3 lightProjectedVertex[ MAX_LIGHTS ];



// ------------------------------------------------
// Functions
// ------------------------------------------------ 

void main () {
	
	if ( renderPass == PASS_DEPTH_MAPS ) {
		mat4 proj = lightProjectionMats[lightIx];
		mat4 view = lightViewMats[lightIx];
		gl_Position = proj * view * modelMatrix * vec4( vertexPosition, 1.0 );

	} else if ( renderPass == PASS_SHADOW_MAP ) {
		vec4 worldVertex = modelMatrix * vec4( vertexPosition, 1.0 );
		for ( uint i = 0; i < lightCount; i++ ) {
			vec4 projected = lightProjectionMats[i] * lightViewMats[i] * worldVertex;

			// De-homogenize projected point
			lightProjectedVertex[i] = ( projected.xyz / projected.w ) * 0.5 + 0.5;

			// move XY to UV coordinates ( interval <0,1> )
			// lightProjectedVertex[i].xy = lightProjectedVertex[i].xy * 0.5 + 0.5;
		}
		gl_Position = projectionMatrix * viewMatrix * worldVertex;
	}

}