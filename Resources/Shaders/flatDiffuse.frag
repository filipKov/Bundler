#version 400

const uint MAX_LIGHTS = 4;

const uint PASS_OUTLINE = 0;
const uint PASS_COLOR = 1;

const float ambientLightStrength = 0.005;
 
struct DirectionalLight {
	vec3 direction;
	vec3 color;
};

struct PointLight {
	vec3 position;
	vec3 color;
	float attenuation;
};

struct Material {
	vec4 diffuseColor;
	bool isTextured;
	sampler2D diffuseTexture;

	vec4 specularColor;
	float specularIntensity;
};

// ------------------------------------------------
// Uniforms
// ------------------------------------------------
uniform uint renderPass;

// uniform float ambientLightStrength;
 
uniform uint directionalLightCount;
uniform DirectionalLight dirLights[MAX_LIGHTS];

uniform uint pointLightCount;
uniform PointLight pointLights[MAX_LIGHTS];

uniform mat4 projectionMatrix;

uniform bool ssaoEnabled;
uniform sampler2D ssaoMap;

uniform bool shadowsEnabled;
uniform sampler2D shadowMap;

uniform Material material;

// ------------------------------------------------
// Inputs/Outputs
// ------------------------------------------------ 
in VertexData {
    vec3 normal;
    vec4 position;
	vec2 textureCoord;
} fragmentData;

layout(location = 0) out vec4 fragmentColor;

// ------------------------------------------------
// Functions
// ------------------------------------------------ 
vec2 GetFragmentUV() {
	vec4 projected = projectionMatrix * fragmentData.position;
	projected = ( projected / projected.w ) * 0.5 + 0.5;
	return projected.xy;
}

vec4 GetSurfaceColor( Material objectMaterial, vec2 textureCoord ) {
	vec4 color = objectMaterial.diffuseColor;
	if ( objectMaterial.isTextured ) {
		color = texture( objectMaterial.diffuseTexture, textureCoord );
	}
	return color;
}

vec4 GetAmbientColor( vec4 surfaceColor, vec4 lightColor ) {
	return ambientLightStrength * surfaceColor * lightColor;
}

vec4 GetDiffuseColor( vec3 surfaceNormal, vec3 surfaceToLight, vec4 surfaceColor, vec4 lightColor ) {
	float diffuseStrength = max( 0.0, dot( surfaceNormal, surfaceToLight) );
	return diffuseStrength * surfaceColor * lightColor;
}

float GetVisibility( vec2 fragmentUV ) {
	return texture( shadowMap, fragmentUV ).r;
}

vec4 ApplyLight( PointLight light, vec4 surfacePosition, vec3 surfaceNormal, vec4 surfaceColor, vec2 fragmentUV ) {
	vec3 surfaceToLight = light.position - surfacePosition.xyz;

	float distanceToLight = length( surfaceToLight );
	float attenuation = 1.0 / ( 1.0 + light.attenuation * distanceToLight * distanceToLight );	

	surfaceToLight = normalize( surfaceToLight );
	vec4 lightColor = vec4( light.color, 1.0 );

	// TODO: specular term

	float visibility = 1.0;
	if ( shadowsEnabled ) {
		visibility = GetVisibility( fragmentUV );
	}

	return	GetAmbientColor( surfaceColor, lightColor )
			+ visibility * attenuation * GetDiffuseColor( surfaceNormal, surfaceToLight, surfaceColor, lightColor );
}

vec4 ApplyLight( DirectionalLight light, vec4 surfacePosition, vec3 surfaceNormal, vec4 surfaceColor, vec2 fragmentUV ) {
	vec3 surfaceToLight = normalize( -light.direction );
	vec4 lightColor = vec4( light.color, 1.0 );

	float visibility = 1.0;
	if ( shadowsEnabled ) {
		visibility = GetVisibility( fragmentUV );
	}

	return	GetAmbientColor( surfaceColor, lightColor ) +
			visibility * GetDiffuseColor( surfaceNormal, surfaceToLight, surfaceColor, lightColor );

}

float GetSSAO( vec2 fragmentUV ) {
	return texture( ssaoMap, fragmentUV ).r;
}

vec4 ApplyGammaCorrection( vec4 linearColor ) {
	const vec3 gammaCorrection = vec3( 1.0 / 2.2 );
	return vec4( pow( linearColor.xyz, gammaCorrection ), linearColor.w );
}


void main() {
	
	if ( renderPass == PASS_OUTLINE ) {
		fragmentColor = vec4( 0.0, 0.0, 0.0, 1.0 );
	} else if ( renderPass == PASS_COLOR ) {
		vec3 surfaceNormal = normalize( fragmentData.normal );
		vec4 surfaceColor = GetSurfaceColor( material, fragmentData.textureCoord );
		vec2 fragmentUV = GetFragmentUV();
		
		vec4 linearColor = vec4( 0.0 );
		for ( uint pointLightI = 0; pointLightI < pointLightCount; pointLightI++ ) {
			linearColor += ApplyLight( pointLights[pointLightI], fragmentData.position, surfaceNormal, surfaceColor, fragmentUV );
		}
		
		for ( uint dirLightI = 0; dirLightI < directionalLightCount; dirLightI++ ) {
			linearColor += ApplyLight( dirLights[dirLightI], fragmentData.position, surfaceNormal, surfaceColor, fragmentUV );
		}

		if ( ssaoEnabled ) {
			linearColor *= GetSSAO( fragmentUV );
		}

		fragmentColor = ApplyGammaCorrection( linearColor ); // vec4( fragmentData.projectedUVW.xy, 1.0, 1.0 );
	}

	// // Blinn-Phong specular term
	// vec4 specColor = vec4( 1,1,1,1 );
	// vec3 H = (L + fragmentData.position.xyz);
	// H = H / length( H );
	// float specStrength = clamp( pow( dot( H, N ), 10 ), 0.0, 1.0 );
}
