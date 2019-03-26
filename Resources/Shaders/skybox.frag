#version 400

const uint MAX_LIGHTS = 4;
const uint MAX_SHADOW_MAPS = MAX_LIGHTS * 2;

const uint PASS_OUTLINE = 0;
const uint PASS_COLOR = 1;
 
struct PointLight {
	vec3 position;
	vec3 color;
	float attenuation;
};

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
uniform float ambientLightStrength;

uniform uint pointLightCount;
uniform PointLight pointLights[MAX_LIGHTS];

uniform Material material;

uniform uint renderPass;

// ------------------------------------------------
// Inputs/Outputs
// ------------------------------------------------ 
in VertexData {
    vec3 normal;
    vec4 position;
	vec4 vertexColor;
	vec2 textureCoord;
} fragmentData;

layout(location = 0) out vec4 fragmentColor;

// ------------------------------------------------
// Functions
// ------------------------------------------------ 
vec4 GetSurfaceColor() {
	vec4 color = material.diffuseColor;
	if ( material.isTextured ) {
		color = texture( material.diffuseTexture, fragmentData.textureCoord );
	} else if ( material.hasVertexColors ) {
		color = fragmentData.vertexColor;
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

vec4 ApplyLight( PointLight light, vec4 surfacePosition, vec3 surfaceNormal, vec4 surfaceColor ) {
	vec3 surfaceToLight = light.position - surfacePosition.xyz;

	float distanceToLight = length( surfaceToLight );
	float attenuation = 1.0 / ( 1.0 + light.attenuation * distanceToLight * distanceToLight );	

	surfaceToLight = normalize( surfaceToLight );
	vec4 lightColor = vec4( light.color, 1.0 );

	// TODO: specular term

	return	GetAmbientColor( surfaceColor, lightColor )
			+ attenuation * GetDiffuseColor( surfaceNormal, surfaceToLight, surfaceColor, lightColor );
}

vec4 ApplyGammaCorrection( vec4 linearColor ) {
	const vec3 gammaCorrection = vec3( 1.0 / 2.2 );
	return vec4( pow( linearColor.xyz, gammaCorrection ), linearColor.w );
}


void main() {
	vec3 surfaceNormal = normalize( fragmentData.normal );
	vec4 surfaceColor = GetSurfaceColor( );

	vec4 linearColor = vec4( 0.0 );
	for ( uint pointLightI = 0; pointLightI < pointLightCount; pointLightI++ ) {
		linearColor += ApplyLight( pointLights[pointLightI], fragmentData.position, surfaceNormal, surfaceColor );
	}
	
	fragmentColor = ApplyGammaCorrection( linearColor );
}
