#version 330 core

out vec4 FragColor;

in vec3 out_TexCoords;

void main()
{    
    FragColor = vec4(out_TexCoords, 1);
}