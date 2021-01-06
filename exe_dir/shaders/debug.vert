#version 330 core

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(viewMatrix * modelMatrix)));
    vs_out.normal = vec3(vec4(normalMatrix * vNormal, 0.0));
    gl_Position = viewMatrix * modelMatrix * vPosition;
}
