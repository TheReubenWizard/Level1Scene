#version 330

uniform mat4 matrixProjection;
uniform mat4 matrixModelView;
uniform mat4 matrixView;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord; // Texture Coordinate Input

out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0; // Texture Coordinate Output


// Light declarations
struct AMBIENT
{	
	vec3 color;
};
uniform AMBIENT lightAmbient;

vec4 AmbientLight(AMBIENT light)
{
    // Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}

struct DIRECTIONAL
{	
	vec3 direction;
	vec3 diffuse;
};
uniform DIRECTIONAL lightDir;

vec4 DirectionalLight(DIRECTIONAL light)
{
	// Calculate Directional Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize(mat3(matrixView) * light.direction);
	float NdotL = dot(normal, L);
	color += vec4(materialDiffuse * light.diffuse, 1) * max(NdotL, 0);
	return color;
}

struct POINT
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};
uniform POINT lightPoint1;
uniform POINT lightPoint2;

void main(void) 
{
    normal = normalize(mat3(matrixModelView) * aNormal);
	// calculate position
	position = matrixModelView * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;
    
    // calculate texture coordinate
	texCoord0 = aTexCoord; // Pass texture coord
    
	// calculate light
	color = vec4(0, 0, 0, 0);
	color += AmbientLight(lightAmbient);
	color += DirectionalLight(lightDir);
	
}