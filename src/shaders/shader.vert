#version 330 core

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 inTexCoords;

uniform mat4 u_MVP;
uniform int u_IsWater;
uniform float u_TimeS;

out vec4 out_color;
out vec3 out_normal;
out vec2 outTexCoords;

void main() {
  vec4 pos = in_pos;
  if(u_IsWater == 1){
  pos.y = pos.y + 0.1*sin((pos.x * pos.z)*10 + u_TimeS)*sin(u_TimeS*0.1);
  pos.y -= 0.15;

  }
  
  gl_Position = u_MVP * pos;

  out_color = in_color;
  out_normal = in_normal;
  outTexCoords = inTexCoords;
}