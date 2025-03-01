#version 330

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

struct SPOT
{
    vec3 position;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
    float cutoff;
    float attenuation;
};
uniform SPOT lightSpot;

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

// Calculates the spot light of an object
vec4 SpotLight(SPOT light)
{
    // Calculate Spot Light
    vec4 color = vec4(0, 0, 0, 0);

    // Transform the light position and direction to view space
    vec4 lightPositionViewSpace = matrixView * vec4(light.position, 1.0);
    vec3 spotDir = normalize(mat3(matrixView) * light.direction);

    // Calculate the light vector (direction from fragment to light)
    vec3 lightVec = normalize(lightPositionViewSpace.xyz - position.xyz);

    // Calculate the cosine of the angle between the light vector and the spotlight direction
    float spotFactor = dot(-lightVec, spotDir);

    // Calculate the cutoff angle in cosine space
    float cutoffAngle = cos(radians(light.cutoff));

    // Check if the fragment is within the spotlight cone
    if (spotFactor > cutoffAngle) {
        // Calculate the NdotL factor
        float NdotL = dot(normal, -lightVec); // Use -lightVec
        color += vec4(materialDiffuse * light.diffuse, 1.0) * max(NdotL, 0.0);

        // Specular Calculation:
        vec3 V = normalize(-position.xyz);
        vec3 R = reflect(lightVec, normal);
        float RdotV = dot(R, V);
        color += vec4(materialSpecular * light.specular, 1.0) * pow(max(RdotV, 0.0), shininess);

        // Attenuation
        spotFactor = pow(spotFactor, light.attenuation);
        color *= spotFactor;
    } else {
        return vec4(0.0); // Outside the spotlight cone, return black
    }

    return color;
}

void main(void) 
{
    outColor = vec4(0,0,0,0);
    outColor += AmbientLight(lightAmbient);
	outColor += DirectionalLight(lightDir);
    outColor += PointLight(lightPoint1, lightIntensity1);
    outColor += PointLight(lightPoint2, lightIntensity2);
    outColor += SpotLight(lightSpot);
    
    // Apply texture to the output
	outColor *= texture(texture0, texCoord0);
	
	// Fresnel Calculation and Reflection:
	float F0 = 0.3; // Typical value for dielectrics
	vec3 V = normalize(-position.xyz);  // View direction
	float NdotV = max(dot(normal, V), 0.0); // Clamp to avoid negative values
	float fresnel = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);

	// Mix the base color with the reflection based on the Fresnel factor
	vec4 reflectionColor = texture(textureCubeMap, texCoordCubeMap);

	// Apply reflection ONLY if reflectionPower > 0
	outColor = mix(outColor, mix(outColor, reflectionColor, fresnel), reflectionPower); // Blend with Fresnel
}