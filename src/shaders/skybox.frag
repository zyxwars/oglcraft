#version 330 core

out vec4 FragColor;

in vec3 out_TexCoords;

void main()
{    
    vec3 skyColor = vec3(1,1,1);
    vec3 horizonColor = vec3(1,0,0);
    FragColor = vec4(mix(horizonColor, skyColor,(out_TexCoords.y + 1)/2) * 0.7 + 0.3, 1);
}