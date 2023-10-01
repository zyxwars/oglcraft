#version 330 core

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 inTexCoords;

uniform mat4 u_MVP;

out vec4 out_color;
out vec3 out_normal;
out vec2 outTexCoords;

void main() {
  gl_Position = u_MVP * in_pos;

  out_color = in_color;
  out_normal = in_normal;
  outTexCoords = inTexCoords;
}