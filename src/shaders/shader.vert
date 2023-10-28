#version 330 core

layout(location = 0) in vec4 in_Pos;
layout(location = 1) in vec4 in_Color;
layout(location = 2) in vec3 in_Normal;
layout(location = 3) in vec2 in_TexCoords;

uniform mat4 u_MVP;
uniform int u_IsWater;
uniform float u_TimeS;

out vec4 out_Color;
out vec3 out_Normal;
out vec2 out_TexCoords;

void main() {
  vec4 pos = in_Pos;
  if (u_IsWater == 1) {
    pos.y =
        pos.y + 0.1 * sin((pos.x * pos.z) * 10 + u_TimeS) * sin(u_TimeS * 0.1);
    pos.y -= 0.15;
  }

  gl_Position = u_MVP * pos;

  out_Color = in_Color;
  out_Normal = in_Normal;
  out_TexCoords = in_TexCoords;
}