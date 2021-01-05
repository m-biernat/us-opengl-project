#version 330
 
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
 
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out vec3 position;
out vec3 normal;
out vec2 texCoords;
 
void main()
{
	position = vec3(modelMatrix * vPosition);
	normal = mat3(transpose(inverse(modelMatrix))) * vNormal;
	texCoords = vTexCoords;
	
	gl_Position =  projectionMatrix * viewMatrix * vec4(position, 1.0);
}
