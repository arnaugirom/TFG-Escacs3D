#version 330 core

struct Material
{	vec4 emission;		// Coeficient d'emissió (r,g,b,a) del material.
	vec4 ambient;		// Coeficient de reflectivitat ambient (r,g,b,a) del material.
        vec4 diffuse;		// Coeficient de reflectivitat difusa (r,g,b,a) del material.
	vec4 specular;		// Coeficient de reflectivitat especular (r,g,b,a) del material.
	float shininess;	// Exponent per al coeficient de reflectivitat especular del material (1,500).
};

layout (location = 0) in vec3 in_Vertex; 
layout (location = 1) in vec4 in_Color; 	
layout (location = 2) in vec3 in_Normal; 	
//layout (location = 3) in vec2 in_TexCoord; 	

out vec4 vertColor;
out vec3 normal;
out vec3 fragPos;
out vec4 FragPosLightSpace;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform mat4 lightSpaceMatrix;

void main() {
	fragPos = vec3(modelMatrix * vec4(in_Vertex, 1.0));

	vec4 transNormal = normalMatrix * vec4(in_Normal, 0.0);
	normal = normalize(transNormal.xyz);

	FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
	
	gl_Position = projectionMatrix * viewMatrix * vec4(fragPos, 1.0);
	vertColor = in_Color;
}