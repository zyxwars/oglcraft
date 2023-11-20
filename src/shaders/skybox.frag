#version 330 core

out vec4 FragColor;

in vec3 out_TexCoords;

void main()
{    
    vec3 skyColor = vec3(128, 185, 255) / 255;
    vec3 horizonColor = vec3(0.8, 0.9, 1);
    
    // FragColor = vec4(mix(horizonColor, skyColor,(out_TexCoords.y + 1)/2), 1);

    // TODO: this works, but normalize() doesn't
    float a = out_TexCoords.y / sqrt(pow(out_TexCoords.x,2) + pow(out_TexCoords.y,2) + pow(out_TexCoords.z,2));

    // FragColor = vec4(mix(horizonColor, skyColor, a), 1);
    FragColor = vec4(1,1,0,1);
}