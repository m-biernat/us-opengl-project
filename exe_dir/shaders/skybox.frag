#version 330

in vec3 texCoords;

out vec4 fColor;

uniform samplerCube skyboxTex;

void main()
{    
    fColor = texture(skyboxTex, texCoords);
}
