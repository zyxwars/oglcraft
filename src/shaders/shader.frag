#version 330 core

out vec4 FragColor;

in vec4 out_color;
in vec2 out_texCoords;

uniform sampler2D u_Texture;

void main(){
    FragColor = texture(u_Texture, out_texCoords); 
}