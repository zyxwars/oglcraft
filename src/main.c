#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <cglm/cam.h>
#include <cglm/affine.h>
#define FNL_IMPL
#include <FastNoiseLite.h>

#include "Shader.h"
#include "GlWrapper.h"

// TODO: remove hardcoded width and height
float getTexelX(int x) { return (x + 0.5f) / 32.f; }

float getTexelY(int y) { return (y + 0.5f) / 16.f; }

struct Transform {
  vec3 translation;
  vec3 rotation;
  vec3 forward;
  vec3 right;
  vec3 up;
};

struct Vertex {
  vec3 position;
  vec3 color;
  vec3 normal;
  vec2 texCoords;
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

  struct Vertex vertices[] = {
      // front
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {getTexelX(16), getTexelY(15)}},
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {getTexelX(16), getTexelY(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {getTexelX(31), getTexelY(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {getTexelX(31), getTexelY(15)}},
      // back
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {getTexelX(16), getTexelY(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {getTexelX(16), getTexelY(0)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {getTexelX(31), getTexelY(0)}},
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {getTexelX(31), getTexelY(15)}},
      // left
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {getTexelX(16), getTexelY(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {getTexelX(16), getTexelY(0)}},
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {getTexelX(31), getTexelY(0)}},
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {getTexelX(31), getTexelY(15)}},
      // right
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {getTexelX(16), getTexelY(15)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {getTexelX(16), getTexelY(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {getTexelX(31), getTexelY(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {getTexelX(31), getTexelY(15)}},
      // top
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {getTexelX(0), getTexelY(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {getTexelX(0), getTexelY(0)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {getTexelX(15), getTexelY(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {getTexelX(15), getTexelY(15)}},
      // bottom
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {getTexelX(16), getTexelY(15)}},
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {getTexelX(16), getTexelY(0)}},
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {getTexelX(31), getTexelY(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {getTexelX(31), getTexelY(15)}},
  };
  unsigned int triangles[] = {// front
                              0, 2, 1, 0, 3, 2,
                              // back
                              4, 5, 6, 4, 6, 7,
                              // left
                              8, 10, 9, 8, 11, 10,
                              // right
                              12, 13, 14, 12, 14, 15,
                              // top
                              16, 18, 17, 16, 19, 18,
                              // bottom
                              20, 21, 22, 20, 22, 23};

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

  GLuint vbo;
  CALL_GL(glGenBuffers(1, &vbo));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0]),
                       GL_STATIC_DRAW));

  int stride = sizeof(struct Vertex);

  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct Vertex, position)));

  CALL_GL(glEnableVertexAttribArray(1));
  CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct Vertex, color)));

  CALL_GL(glEnableVertexAttribArray(2));
  CALL_GL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct Vertex, normal)));

  CALL_GL(glEnableVertexAttribArray(3));
  CALL_GL(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct Vertex, texCoords)));

  GLuint ebo;
  CALL_GL(glGenBuffers(1, &ebo));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles), triangles,
               GL_STATIC_DRAW);

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

    CALL_GL(glClearColor(0.8f, 0.9f, 1.f, 1.f));
    CALL_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    int chunkSize = 16;
    int renderDistance = 8;
    int renderDistanceSquared = pow(renderDistance, 2);

    int chunkPlayerX = floor(cameraTransform.translation[0] / chunkSize);
    int chunkPlayerZ = floor(cameraTransform.translation[2] / chunkSize);

    // printf("x:%.2f z:%.2f cx:%d cz:%d\n", cameraTransform.translation[0],
    //        cameraTransform.translation[2], chunkPlayerX, chunkPlayerZ);

    // mat4 model = GLM_MAT4_IDENTITY_INIT;
    // glm_translate(model, (vec3){0, 0, 0});

    // // rendering
    // glm_mat4_mulN((mat4*[]){&projection, &view, &model}, 3, mvp);

    // CALL_GL(glUniformMatrix4fv(MVPUniformLocation, 1, GL_FALSE, mvp[0]));
    // CALL_GL(glDrawElements(GL_TRIANGLES, sizeof(triangles) / 3,
    // GL_UNSIGNED_INT,
    //                        0));

    for (int chunkX = chunkPlayerX - renderDistance;
         chunkX <= chunkPlayerX + renderDistance; chunkX++) {
      for (int chunkZ = chunkPlayerZ - renderDistance;
           chunkZ <= chunkPlayerZ + renderDistance; chunkZ++) {
        // circular render distance
        if (pow(chunkX - chunkPlayerX, 2) + pow(chunkZ - chunkPlayerZ, 2) >=
            renderDistanceSquared) {
          continue;
        }

        // render chunk
        for (int x = 0; x < chunkSize; x++) {
          for (int z = 0; z < chunkSize; z++) {
            for (int y = 0; y < 1; y++) {
              float worldX = chunkX * chunkSize + x;
              float worldZ = chunkZ * chunkSize + z;
              int height = floor(fnlGetNoise2D(&noise, worldX, worldZ) * 16);

              mat4 model = GLM_MAT4_IDENTITY_INIT;
              glm_translate(model, (vec3){worldX, height, worldZ});

              // rendering
              glm_mat4_mulN((mat4*[]){&projection, &view, &model}, 3, mvp);

              CALL_GL(
                  glUniformMatrix4fv(MVPUniformLocation, 1, GL_FALSE, mvp[0]));
              CALL_GL(glDrawElements(GL_TRIANGLES, sizeof(triangles) / 3,
                                     GL_UNSIGNED_INT, 0));
            }
          }
        }
      }
    }
    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}