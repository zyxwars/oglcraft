#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <stb_image.h>
#define FNL_IMPL
#include <FastNoiseLite.h>

#include "GlWrapper.h"
#include "Shader.h"
#include "Block.h"

struct Transform {
  vec3 translation;
  vec3 rotation;
  vec3 forward;
  vec3 right;
  vec3 up;
};

int main(void) {
  // Init glfw
  GLFWwindow* window;

  if (!glfwInit()) return -1;

  // display scaling
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwMakeContextCurrent(window);

  // Init glew
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  // Init gl
  printf("Status: Using GL %s\n", glGetString(GL_VERSION));

  CALL_GL(glEnable(GL_CULL_FACE));
  CALL_GL(glEnable(GL_DEPTH_TEST));

  int textureWidth, textureHeight, nrChannels;
  unsigned char* textureData = stbi_load(
      "C:/Users/Zyxwa/Documents/code/oglc/src/assets/texture_atlas.jpg",
      &textureWidth, &textureHeight, &nrChannels, 0);
  if (textureData == NULL) {
    LOG("Texture", "Couldn't load texture from file");
  }

  GLuint vao;
  CALL_GL(glGenVertexArrays(1, &vao));
  CALL_GL(glBindVertexArray(vao));

  GLuint texture;
  CALL_GL(glGenTextures(1, &texture));
  CALL_GL(glActiveTexture(GL_TEXTURE0));
  CALL_GL(glBindTexture(GL_TEXTURE_2D, texture));

  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                          GL_LINEAR_MIPMAP_NEAREST));
  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, textureData));
  CALL_GL(glGenerateMipmap(GL_TEXTURE_2D));
  stbi_image_free(textureData);

  GLuint shaderProgram = CreateShaderProgram(
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/shader.vert",
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/shader.frag");
  CALL_GL(glUseProgram(shaderProgram));

  CALL_GL(GLint MVPUniformLocation =
              glGetUniformLocation(shaderProgram, "u_MVP"));

  mat4 projection;
  glm_perspective(45.f, 640.f / 480.f, 1.f, 1000.f, projection);

  struct Transform cameraTransform;
  cameraTransform.translation[0] = 0.f;
  cameraTransform.translation[1] = 10.f;
  cameraTransform.translation[2] = 0.f;

  cameraTransform.rotation[0] = -90.f;
  cameraTransform.rotation[1] = 0.f;
  cameraTransform.rotation[2] = 0.f;

  cameraTransform.forward[0] = 0.f;
  cameraTransform.forward[1] = 0.f;
  cameraTransform.forward[2] = -1.f;

  vec3 up = {0.f, 1.f, 0.f};

  glm_vec3_cross(up, cameraTransform.forward, cameraTransform.right);
  glm_vec3_normalize(cameraTransform.right);
  glm_vec3_cross(cameraTransform.forward, cameraTransform.right,
                 cameraTransform.up);

  float movementSpeed = 7.5f;
  float speedMultiplier = 0.f;
  int lastVerticalInput = 0;
  int lastHorizontalInput = 0;

  int mouseHasMoved = 0;
  double lastMouseX, lastMouseY;
  float mouseSens = 0.01f;

  float deltaTimeS = 0.f;
  float lastTimeS = 0.f;

  // Create and configure noise state
  fnl_state noise = fnlCreateState();
  noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

  // Create chunk data
  unsigned int* chunkData = CreateChunk(&noise, 0, 0);

  // Create chunk mesh
  // This needs to be updated on every change
  int numOfFaces = CreateChunkMesh(chunkData);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    float currentTimeS = (float)glfwGetTime();
    deltaTimeS = currentTimeS - lastTimeS;
    lastTimeS = currentTimeS;

    int verticalInput = 0;
    int horizontalInput = 0;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      verticalInput += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      verticalInput -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      horizontalInput += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      horizontalInput -= 1;
    }

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    // camera rotation
    if (mouseHasMoved) {
      float relativeMouseX = mouseX - lastMouseX;
      float relativeMouseY = mouseY - lastMouseY;

      cameraTransform.rotation[0] += relativeMouseX * mouseSens;
      cameraTransform.rotation[1] += -relativeMouseY * mouseSens;

      // clamp y rotation
      if (cameraTransform.rotation[1] > 89.9f) {
        cameraTransform.rotation[1] = 89.9f;
      } else if (cameraTransform.rotation[1] < -89.9f) {
        cameraTransform.rotation[1] = -89.9f;
      }

      // TODO: visualize
      cameraTransform.forward[0] = cos(glm_rad(cameraTransform.rotation[0])) *
                                   cos(glm_rad(cameraTransform.rotation[1]));
      cameraTransform.forward[1] = sin(glm_rad(cameraTransform.rotation[1]));
      cameraTransform.forward[2] = sin(glm_rad(cameraTransform.rotation[0])) *
                                   cos(glm_rad(cameraTransform.rotation[1]));

      glm_vec3_normalize(cameraTransform.forward);

      vec3 up = {0.f, 1.f, 0.f};

      glm_vec3_cross(up, cameraTransform.forward, cameraTransform.right);
      glm_vec3_normalize(cameraTransform.right);
      glm_vec3_cross(cameraTransform.forward, cameraTransform.right,
                     cameraTransform.up);
    } else {
      mouseHasMoved = 1;
    }

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    vec3 zMovement;
    glm_vec3_scale(cameraTransform.forward,
                   movementSpeed * verticalInput * deltaTimeS * speedMultiplier,
                   zMovement);
    glm_vec3_add(cameraTransform.translation, zMovement,
                 cameraTransform.translation);

    vec3 xMovement;
    glm_vec3_scale(
        cameraTransform.right,
        movementSpeed * horizontalInput * deltaTimeS * speedMultiplier,
        xMovement);
    glm_vec3_add(cameraTransform.translation, xMovement,
                 cameraTransform.translation);

    if ((verticalInput == lastVerticalInput && verticalInput != 0) ||
        (horizontalInput == lastHorizontalInput && horizontalInput != 0)) {
      speedMultiplier += deltaTimeS;
    } else {
      speedMultiplier = 0;
    }

    lastVerticalInput = verticalInput;
    lastHorizontalInput = horizontalInput;

    mat4 view;
    vec3 cameraTarget;
    glm_vec3_add(cameraTransform.translation, cameraTransform.forward,
                 cameraTarget);
    glm_lookat(cameraTransform.translation, cameraTarget, cameraTransform.up,
               view);

    mat4 mvp;

    // Fog color
    CALL_GL(glClearColor(0.8f, 0.9f, 1.f, 1.f));
    CALL_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // position chunk
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, (vec3){0, 0, 0});
    glm_mat4_mulN((mat4*[]){&projection, &view, &model}, 3, mvp);

    // draw chunk
    CALL_GL(glUniformMatrix4fv(MVPUniformLocation, 1, GL_FALSE, mvp[0]));
    CALL_GL(glDrawElements(GL_TRIANGLES, numOfFaces * 6, GL_UNSIGNED_INT, 0));
    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}