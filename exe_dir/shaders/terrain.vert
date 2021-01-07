#version 330
 
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform vec2 tiling;

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;

out vec3 position;
out vec3 normal;
out vec2 texCoords;
 
void main()
{
	position = vec3(modelMatrix * vPosition);
	normal = mat3(transpose(inverse(modelMatrix))) * vNormal;
	texCoords = vPosition.xy * tiling;
	
	gl_Position =  projectionMatrix * viewMatrix * vec4(position, 1.0);
}
