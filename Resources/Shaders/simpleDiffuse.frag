#version 400
 
in vec3 normal;
in vec4 ecPos;
in vec2 texcoord;
 
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform bool texturePresent;
uniform sampler2D diffuseTexture;

layout(location = 0) out vec4 colorOut;

uniform vec4 specularColor;
uniform float specularStrength;

uniform vec4 diffuseColor;
 
void main()
{
  vec3 N = normalize(normal);
  vec3 L = normalize(lightPos - ecPos.xyz);
  float NdotL = clamp(dot(N, L), 0.0, 1.0);


  // Blinn-Phong specular term
  vec4 specColor = vec4( 1,1,1,1 );
  vec3 H = (L + ecPos.xyz);
  H = H / length( H );
  float specStrength = clamp( pow( dot( H, N ), 20 ), 0.0, 1.0 );

  vec4 color = vec4( 1, 1, 1, 1 );
  if ( texturePresent ) {
	color = texture( diffuseTexture, texcoord );
  }

  colorOut = ( color * NdotL ) + 0.7 * ( specStrength * specColor );
}
