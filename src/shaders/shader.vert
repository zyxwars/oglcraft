#version 330 core

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_texCoords;

uniform mat4 u_MVP;

out vec4 out_color;
out vec2 out_texCoords;

void main() {
  gl_Position = u_MVP * in_pos;

  out_color = in_color;
  out_texCoords = in_texCoords;
}