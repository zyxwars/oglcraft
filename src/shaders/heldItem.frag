#version 330 core

in vec3 out_Normal;
in vec2 out_TexCoords;

uniform sampler2D texture1;

out vec4 FragColor;

void main(){
  vec4 tex = texture(texture1, out_TexCoords);
  FragColor = tex;
}