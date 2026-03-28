#version 330 core
layout (location = 0) in vec3 in_Vertex;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(in_Vertex, 1.0);
}  