#version 330

uniform mat4 matrixProjection;
uniform mat4 matrixModelView;
uniform mat4 matrixView;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

// Vertex Attributes
in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord;

// Output Variables (for fragment shader)
out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;
out vec3 texCoordCubeMap; // NEW - Cube Map TexCoord

void main(void) 
{
    normal = normalize(mat3(matrixModelView) * aNormal);
	// calculate position
	position = matrixModelView * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;
    
    // calculate texture coordinate
	texCoord0 = aTexCoord;
	
	// calculate reflection vector
	texCoordCubeMap = inverse(mat3(matrixView)) * reflect(position.xyz, normal);
}