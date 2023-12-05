#version 330 core

layout(location = 0) in int in_BlockId;
layout(location = 1) in vec4 in_Pos;
layout(location = 2) in vec3 in_Normal;
layout(location = 3) in vec2 in_TexCoords;

uniform mat4 u_MVP;

out vec3 out_Normal;
out vec2 out_TexCoords;

void main(){
    vec4 pos = vec4(in_Pos, 1.0);

    gl_Position = u_MVP * pos;

    out_Normal = in_Normal;
    out_TexCoords = in_TexCoords;
}