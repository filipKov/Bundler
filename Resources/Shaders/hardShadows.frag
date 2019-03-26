#version 400

const uint PASS_DEPTH_MAPS = 0;
const uint PASS_SHADOW_MAP = 1;

const uint MAX_LIGHTS = 8;

const float VISIBILITY_FULL = 1.0;
const float VISIBILITY_SHADOW = 0.0;

// ------------------------------------------------
// Uniforms
// ------------------------------------------------ 
uniform uint lightCount;

uniform sampler2D lightDepthMaps[ MAX_LIGHTS ];

uniform uint renderPass;

// ------------------------------------------------
// Inputs/Outputs
// ------------------------------------------------ 
in vec3 lightProjectedVertex[ MAX_LIGHTS ];

layout( location = 0 ) out vec4 fragmentColor;

// ------------------------------------------------
// Functions
// ------------------------------------------------ 
float LinearizeDepth( float depth ) {
	float near = 0.1f;
	float far = 100.0f;
	return ( 2 * near ) / ( far + near - depth * ( far - near ) );
}

float GetFragmentVisibility( sampler2D shadowMap, vec3 fragmentLightPos ) {
	float closestDepth = texture( shadowMap, fragmentLightPos.xy ).z;
	float currentDepth = fragmentLightPos.z;
	
	float bias = 0.005;
	if ( ( currentDepth - bias ) > closestDepth ) {
		return VISIBILITY_SHADOW;
	}
	return VISIBILITY_FULL;
}

float GetFragmentVisibility() {
	float visibility = VISIBILITY_FULL;
	for ( uint i = 0; i < lightCount; i++ ) {
		visibility *= GetFragmentVisibility( lightDepthMaps[i], lightProjectedVertex[i] );
	}
	return visibility;
}

void main() {
	if ( renderPass == PASS_DEPTH_MAPS ) {
		float z = gl_FragCoord.z; //LinearizeDepth( gl_FragCoord.z ); // 
		fragmentColor = vec4( z, z, z, 1.0 );

	} else if ( renderPass == PASS_SHADOW_MAP ) {
		float fragmentVisibility = GetFragmentVisibility();
		fragmentColor = vec4( fragmentVisibility, fragmentVisibility, fragmentVisibility, 1.0 ); // vec4( 1.0, 0.0, 0.0, 1.0 );
	}
}
