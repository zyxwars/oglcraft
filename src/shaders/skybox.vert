#version 330 core

layout (location = 0) in vec3 in_Pos;

out vec3 out_TexCoords;

uniform mat4 u_MVP;

void main()
{
    vec4 pos = vec4(in_Pos, 1.0);
    out_TexCoords = pos.xyz;
    gl_Position = u_MVP * pos;
}  