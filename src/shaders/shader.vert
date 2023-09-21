#version 330 core

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;

uniform mat4 u_MVP;

out vec4 out_color;

void main() {
  gl_Position = u_MVP * in_pos;
  out_color = in_color;
}