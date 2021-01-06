#version 330 core

uniform mat4 projMatrix;

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

const float MAGNITUDE = 0.3;

in VS_OUT {
    vec3 normal;
} gs_in[];

void GenerateLine(int index)
{
    gl_Position = projMatrix * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projMatrix * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}