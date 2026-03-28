#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 in_Vertex;

// Values that stay constant for the whole mesh.


uniform mat4 projectionMatrix;	// Projection Matrix
uniform mat4 viewMatrix; 	// View Matrix
uniform mat4 modelMatrix;	// Model Matrix

void main(){
	vec3 pos = in_Vertex;
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(pos,1);

}