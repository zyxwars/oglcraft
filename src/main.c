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

// TODO: remove hardcoded width and height
// Get center of texel to avoid bleeding
#define GET_TEXEL_X(x) ((x + 0.5f) / 32.f)

#define GET_TEXEL_Y(y) ((y + 0.5f) / 16.f)

#define CHUNK_LENGTH 16
// how many blocks fit in 1 tall chunk
#define CHUNK_PLANE_AREA CHUNK_LENGTH* CHUNK_LENGTH

int PosToIndex(int x, int y, int z) {
  return x + z * CHUNK_LENGTH + y * CHUNK_PLANE_AREA;
}

enum BlockFace {
  BLOCK_FACE_FRONT,
  BLOCK_FACE_BACK,
  BLOCK_FACE_TOP,
  BLOCK_FACE_BOTTOM,
  BLOCK_FACE_LEFT,
  BLOCK_FACE_RIGHT,
};

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

void MakeFace(enum BlockFace blockFace, int x, int y, int z,
              int* currentFaceIndex, struct Vertex* vertices,
              unsigned int* triangles) {
  const static struct Vertex frontFace[] = {
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };
  const static struct Vertex backFace[] = {
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };
  const static struct Vertex topFace[] = {
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {GET_TEXEL_X(0), GET_TEXEL_Y(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {GET_TEXEL_X(0), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {GET_TEXEL_X(15), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {GET_TEXEL_X(15), GET_TEXEL_Y(15)}},

  };
  const static struct Vertex bottomFace[] = {
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };
  const static struct Vertex leftFace[] = {
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };
  const static struct Vertex rightFace[] = {
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };

  const struct Vertex* pFaceToCopy = NULL;

  int currentVertexIndex = (*currentFaceIndex) * 4;
  int currentTriangleIndex = (*currentFaceIndex) * 6;

  struct Vertex* currentVertexStart = vertices + currentVertexIndex;

  // TODO: create triangles more cleanly
  switch (blockFace) {
    case BLOCK_FACE_FRONT:
      pFaceToCopy = frontFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 3 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 2 + currentVertexIndex;
      break;
    case BLOCK_FACE_BACK:
      pFaceToCopy = backFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 3 + currentVertexIndex;
      break;
    case BLOCK_FACE_TOP:
      pFaceToCopy = topFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 3 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 2 + currentVertexIndex;
      break;
    case BLOCK_FACE_BOTTOM:
      pFaceToCopy = bottomFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 3 + currentVertexIndex;
      break;
    case BLOCK_FACE_LEFT:
      pFaceToCopy = leftFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 3 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 2 + currentVertexIndex;
      break;
    case BLOCK_FACE_RIGHT:
      pFaceToCopy = rightFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 3 + currentVertexIndex;
      break;
  }

  // TODO:
  if (pFaceToCopy == NULL) {
    __debugbreak();
  }
  // Copy the 4 face vertices into the vertex buffer
  memcpy(currentVertexStart, pFaceToCopy, 4 * sizeof(struct Vertex));

  // Set face position based on cube position
  for (int i = 0; i < 4; i++) {
    // TODO: multiply by chunk as well
    currentVertexStart[i].position[0] += x;
    currentVertexStart[i].position[1] += y;
    currentVertexStart[i].position[2] += z;
  }

  (*currentFaceIndex)++;
}

void MakeBlock(unsigned int* chunkBlocks, int x, int y, int z,
               int* currentFaceIndex, struct Vertex* vertices,
               unsigned int* triangles) {
  // TODO: create visible faces
  // Left expression should be evaluated first and skip all others to avoid
  // index out of range
  if (y + 1 == CHUNK_LENGTH || chunkBlocks[PosToIndex(x, y + 1, z)] == 0) {
    // Adds face to the buffer and incremenets faceIndex
    MakeFace(BLOCK_FACE_TOP, x, y, z, currentFaceIndex, vertices, triangles);
  }
  if (y - 1 == -1 || chunkBlocks[PosToIndex(x, y - 1, z)] == 0) {
    MakeFace(BLOCK_FACE_BOTTOM, x, y, z, currentFaceIndex, vertices, triangles);
  }
  if (x + 1 == CHUNK_LENGTH || chunkBlocks[PosToIndex(x + 1, y, z)] == 0) {
    MakeFace(BLOCK_FACE_RIGHT, x, y, z, currentFaceIndex, vertices, triangles);
  }
  if (x - 1 == -1 || chunkBlocks[PosToIndex(x - 1, y, z)] == 0) {
    MakeFace(BLOCK_FACE_LEFT, x, y, z, currentFaceIndex, vertices, triangles);
  }
  if (z + 1 == CHUNK_LENGTH || chunkBlocks[PosToIndex(x, y, z + 1)] == 0) {
    MakeFace(BLOCK_FACE_FRONT, x, y, z, currentFaceIndex, vertices, triangles);
  }
  if (z - 1 == -1 || chunkBlocks[PosToIndex(x, y, z - 1)] == 0) {
    MakeFace(BLOCK_FACE_BACK, x, y, z, currentFaceIndex, vertices, triangles);
  }
}

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

  GLuint vbo;
  CALL_GL(glGenBuffers(1, &vbo));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));

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
  // TODO: free
  unsigned int* chunkBlocks =
      malloc(sizeof(unsigned int) * pow(CHUNK_LENGTH, 3));

  // Populate chunk
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_LENGTH; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        // TODO: proper block types
        chunkBlocks[PosToIndex(x, y, z)] = 1;
      }
    }
  }

  // Create chunk mesh
  // This needs to be updated on every change

  // TODO: allocate in a less wasteful way?
  const int maxFacesInChunk = (pow(CHUNK_LENGTH, 3) * 6);

  struct Vertex* vertices = malloc(maxFacesInChunk * 4 * sizeof(struct Vertex));
  unsigned int* triangles = malloc(maxFacesInChunk * 6 * sizeof(unsigned int));

  int currentFaceIndex = 0;
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_LENGTH; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        MakeBlock(chunkBlocks, x, y, z, &currentFaceIndex, vertices, triangles);
      }
    }
  }

  // We copy only the existing vertices based on face index
  // ignoring the rest of data that is garbage, allocated for the worst case
  // when culling faces
  CALL_GL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(struct Vertex) * 4 * currentFaceIndex, vertices,
                       GL_STATIC_DRAW));
  free(vertices);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(unsigned int) * 6 * currentFaceIndex, triangles,
               GL_STATIC_DRAW);
  free(triangles);

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
    CALL_GL(
        glDrawElements(GL_TRIANGLES, currentFaceIndex * 6, GL_UNSIGNED_INT, 0));
    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}