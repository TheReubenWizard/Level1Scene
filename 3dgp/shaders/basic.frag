#version 330

out vec4 outColor;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

// View Matrix
uniform mat4 matrixView;

// Point Light Data
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

// Per-pixel data from vertex shader
in vec4 position;
in vec3 normal;

// Ambient Light Data
struct AMBIENT
{	
	vec3 color;
};
uniform AMBIENT lightAmbient;

// Directional Light Data
struct DIRECTIONAL
{	
	vec3 direction;
	vec3 diffuse;
};
uniform DIRECTIONAL lightDir;

// TEXTURE START
in vec2 texCoord0; // Texture coordinates
uniform sampler2D texture0; // Sampler for the texture
// TEXTURE END

// Environment Mapping 
in vec3 texCoordCubeMap; // in variable
uniform samplerCube textureCubeMap;
uniform float reflectionPower;

// Calculates the ambient light of an object
vec4 AmbientLight(AMBIENT light)
{
    // Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}

// Calculates the directional light of an object
vec4 DirectionalLight(DIRECTIONAL light)
{
	// Calculate Directional Light
	vec3 L = normalize(mat3(matrixView) * light.direction);
	float NdotL = dot(normal, L);
	return vec4(materialDiffuse * light.diffuse, 1) * max(NdotL, 0);
}

// Calculates the point light of an object
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
	outColor += DirectionalLight(lightDir);
    outColor += PointLight(lightPoint1, lightIntensity1);
    outColor += PointLight(lightPoint2, lightIntensity2);
    
    // Apply texture to the output
	outColor *= texture(texture0, texCoord0);
	// mixing the textures
		outColor = mix(outColor, texture(textureCubeMap, texCoordCubeMap), reflectionPower);
}