#version 330 core

out vec4 FragColor;

in vec3 out_Normal;
in vec2 out_TexCoords;

uniform sampler2D texture1;

// TODO: adjust based on real near and far plane
float near = 0.1f;
float far = 200.f;

float LinearizeDepth(float depth) {
  // convert from [0,1] to [-1,1]
  float z = depth * 2.0 - 1.0;
  // reverse to linear
  return (2.0 * near * far) / (far + near - (z * (far - near)));
}

float ExpSquaredFog(float depth, float density){
  float factor = depth * density;
  return exp2(-factor * factor);
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

  // Get z depth and divide by far to get [0,1] range
  float linearDepthNormalized = LinearizeDepth(gl_FragCoord.z) / far;
  float fogFactor = ExpSquaredFog(linearDepthNormalized, 2.0f);

  // Set colors
  vec3 lightColor = vec3(1, 1, 0.8);
  float ambientStrength = 0.2;

  vec3 fogColor = vec3(0.8, 0.9, 1);

  // Lighting
  vec3 lightDir = -1 * normalize(vec3(1, -1, 0.5));

  vec3 ambient = lightColor * ambientStrength;
  vec3 diffuse = lightColor * max(dot(out_Normal, lightDir), 0);

  vec3 color = tex.xyz * (diffuse + ambient);

  FragColor = vec4(mix(fogColor, color, fogFactor), tex.a);
}