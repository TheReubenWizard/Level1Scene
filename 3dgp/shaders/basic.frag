#version 330

in vec4 color;
out vec4 outColor;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

// View Matrix
uniform mat4 matrixView;

struct POINT
{
		vec3 position;
		vec3 diffuse;
		vec3 specular;
};
uniform POINT lightPoint1;
uniform POINT lightPoint2;

uniform float lightIntensity1;
uniform float lightIntensity2;

in vec4 position;
in vec3 normal;

// Ambient Light
struct AMBIENT
{	
	vec3 color;
};
uniform AMBIENT lightAmbient;

// TEXTURE START
in vec2 texCoord0; // Texture coord input
uniform sampler2D texture0; // Sampler for the texture
// TEXTURE END

vec4 AmbientLight(AMBIENT light)
{
    // Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}

vec4 PointLight(POINT light, float intensity)
{
    // Calculate Point Light
    vec4 color = vec4(0, 0, 0, 0);

    // Calculate light vector L (normalized displacement vector)
    vec4 lightPositionViewSpace = matrixView * vec4(light.position, 1.0);
    vec3 L = normalize((lightPositionViewSpace.xyz - position.xyz));
    
    float NdotL = dot(normal, L);
    color += vec4(materialDiffuse * light.diffuse, 1) * max(NdotL, 0) * intensity;

    // Specular Calculation:
    vec3 V = normalize(-position.xyz);
    vec3 R = reflect(-L, normal);
    float RdotV = dot(R, V);
    color += vec4(materialSpecular * light.specular * pow(max(RdotV, 0), shininess), 1) * intensity;

    return color;
}

void main(void) 
{
    outColor = vec4(0,0,0,0);
    outColor += AmbientLight(lightAmbient);
    outColor += PointLight(lightPoint1, lightIntensity1);
    outColor += PointLight(lightPoint2, lightIntensity2);
    // apply texture
	outColor *= texture(texture0, texCoord0); // NEW: Apply texture
}