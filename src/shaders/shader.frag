#version 330 core

out vec4 FragColor;

in vec4 out_Color;
in vec3 out_Normal;
in vec2 out_TexCoords;

uniform sampler2D texture1;

float near = 0.1f;
float far = 5.f;

// TODO: what this does
float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0;  // back to NDC
  return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
  // Get texel pos
  vec4 tex = texture(texture1, out_TexCoords);

  // Cull transparent pixels
  // 0.1 due to linear filtering pixel with alpha 0 might not be actually 0
  // https://www.khronos.org/opengl/wiki/Transparency_Sorting#Depth_Sorting
  if (tex.a < 0.1) {
    discard;
  }

  // Get z depth
  float depth = LinearizeDepth(gl_FragCoord.z) / far;
  depth = pow(depth, 2);

  // Set colors
  vec3 lightColor = vec3(1, 1, 0.8);
  float ambientStrength = 0.2;

  vec3 fogColor = vec3(0.8, 0.9, 1);

  // Lighting
  vec3 lightDir = -1 * normalize(vec3(1, -1, 0.5));

  vec3 ambient = lightColor * ambientStrength;
  vec3 diffuse = lightColor * max(dot(out_Normal, lightDir), 0);

  vec3 color = tex.xyz * (diffuse + ambient);

  FragColor = vec4(mix(color, fogColor, depth), tex.a);
}