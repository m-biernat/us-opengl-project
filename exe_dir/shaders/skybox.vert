#version 330
 
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

layout (location = 0) in vec3 vPosition;

out vec3 texCoords;
 
void main()
{
	texCoords = vPosition;	

	vec4 position = projMatrix * viewMatrix * vec4(vPosition, 1.0);
	gl_Position = position.xyww; // Eliminuje problem z far clippingiem skyboxa
}
