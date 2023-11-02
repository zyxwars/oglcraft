#version 330 core

layout(location = 0) in int in_BlockId;
layout(location = 1) in vec4 in_Pos;
layout(location = 2) in vec3 in_Normal;
layout(location = 3) in vec2 in_TexCoords;

uniform mat4 u_MVP;
uniform float u_TimeS;

out vec3 out_Normal;
out vec2 out_TexCoords;

void main() {
  vec4 pos = in_Pos;
  
  // Animate water
  if (in_BlockId == 2) {
    // Move with sine wave, change intensity based on time sine wave
    pos.y =
        pos.y + 0.1 * sin((pos.x * pos.z) * 10 + u_TimeS) * sin(u_TimeS * 0.1);
    // Move water level down
    pos.y -= 0.15;
  }
  // Animate leaves
  if (in_BlockId == 5) {
    pos.y =
        pos.y + 0.1 * sin((pos.x * pos.z) * 2 + u_TimeS) * sin(u_TimeS * 0.1);
  }

  gl_Position = u_MVP * pos;

  out_Normal = in_Normal;
  out_TexCoords = in_TexCoords;
}