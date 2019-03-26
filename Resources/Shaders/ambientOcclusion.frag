#version 400


const uint PASS_DEPTH = 0;
const uint PASS_AO_MAP = 1;
const uint PASS_BLUR = 2;

const uint MAX_KERNEL_SIZE = 128;

const int gaussianSize = 9;

// ------------------------------------------------
// Uniforms
// ------------------------------------------------
uniform uint renderPass;

uniform mat4 projectionMatrix;

uniform uint kernelScale;
uniform vec3 kernelSamples[MAX_KERNEL_SIZE];
uniform sampler2D kernelNoise;
uniform vec2 kernelNoiseScale;
uniform float kernelRadius;
uniform float bias;

uniform sampler2D positionMap;
uniform sampler2D normalMap;

uniform sampler2D ssaoMap;
uniform float gaussian[gaussianSize * gaussianSize];


// ------------------------------------------------
// Inputs/Outputs
// ------------------------------------------------ 
in VertexData {
    vec3 normal;
    vec4 position;
} fragmentData;

layout( location = 0 ) out vec4 fragmentColor;
layout( location = 1 ) out vec4 fragmentNormal;

// ------------------------------------------------
// Functions
// ------------------------------------------------ 
vec2 GetUvCoords() {
	vec4 projected = projectionMatrix * fragmentData.position;
	projected = ( projected / projected.w ) * 0.5 + 0.5;
	return projected.xy;
}

float GetFragmentOcclusion() {
	vec2 uvCoords = GetUvCoords(); // fragmentData.textureCoords;
	vec3 position = texture( positionMap, uvCoords ).xyz;
	vec3 normal = texture( normalMap, uvCoords ).rgb;
	vec3 randomVector = texture( kernelNoise, uvCoords * kernelNoiseScale ).xyz;
		
	vec3 tangent = normalize( randomVector - normal * dot(randomVector, normal) );
	vec3 bitangent = cross( normal, tangent );
	mat3 TBN = mat3( tangent, bitangent, normal );

	float occlusion = 0.0;

	for ( uint i = 0; i < kernelScale; i++) {
		vec3 kernelSample = TBN * kernelSamples[i];
		kernelSample = position + kernelSample * kernelRadius;

		vec4 offset = projectionMatrix * vec4( kernelSample, 1.0 );
		offset /= offset.w;
		offset.xyz *= 0.5;
		offset.xyz += 0.5;

		float sampleDepth = texture( positionMap, offset.xy ).z;
		float rangeCheck = smoothstep( 0.0, 1.0, kernelRadius / abs( position.z - sampleDepth ) );
		occlusion += ( sampleDepth >= kernelSample.z + bias ? 1.0 : 0.0 ) * rangeCheck;
	}

	return occlusion / kernelScale;
}

float LinearizeDepth(float depth, float near, float far) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

float GaussianEl( int x, int y ) {
	return gaussian[gaussianSize * y + x];
}

float ApplyBlur() {
	vec2 fragmentUV = GetUvCoords();
	vec2 texelSize = 1.0 / vec2( textureSize( ssaoMap, 0 ) );
	int halfSize = int( floor( float(gaussianSize) / 2 ) );

	float result = 0.0;
	for ( int x = -halfSize; x <= halfSize; x++ ) {
		for ( int y = -halfSize; y <= halfSize; y++ ) {
			vec2 offset = vec2( x, y ) * texelSize;
			result += texture( ssaoMap, fragmentUV + offset ).r * GaussianEl( halfSize+x, halfSize+y );
		}
	}
	return result;
}

void main() {

	if ( renderPass == PASS_DEPTH ) {
		fragmentColor = vec4( fragmentData.position.xyz, 1.0 );
		fragmentNormal = vec4( normalize( fragmentData.normal ), 1.0 );
	
	} else if ( renderPass == PASS_AO_MAP ) {
		float visibility = 1.0 - GetFragmentOcclusion();
		visibility = pow( visibility, 3.0 );
		fragmentColor = vec4( visibility, visibility, visibility, 1.0 );
	} else if ( renderPass == PASS_BLUR ) {
		float blurred = ApplyBlur();
		fragmentColor = vec4( blurred, blurred, blurred, 1.0 );
	}

}
