#version 330 core

out vec4 FragColor;

in vec4 out_color;
in vec3 out_normal;

float near = 0.1f; 
float far  = 5.f;
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main(){
    vec3 lightColor = vec3(1,1,0.8);

    vec3 fogColor = vec3(0.8,0.9,1);
    fogColor = fogColor * 1.f;
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    depth = pow(depth, 2);

    vec3 lightDir = -1 * normalize(vec3(1,-1,0.5));
    float ambientStrength = 0.2; 

    vec3 ambient = lightColor * ambientStrength;
    vec3 diffuse = lightColor * max(dot(out_normal, lightDir), 0); 

    vec3 color = out_color.xyz * (diffuse + ambient);
    FragColor = vec4(mix(color, fogColor, depth), 1);
}