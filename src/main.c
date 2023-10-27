#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#define FNL_IMPL
#include <FastNoiseLite.h>

#include "GlWrapper.h"
#include "Shader.h"
#include "Chunk.h"
#include "Camera.h"
// #include "Text.h"

// #define _CRTDBG_MAP_ALLOC
// #include<crtdbg.h>

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

  CALL_GL(glEnable(GL_BLEND));
  CALL_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  int textureWidth, textureHeight, nrChannels;
  unsigned char* textureData =
      stbi_load("C:/Users/Zyxwa/Documents/code/oglc/src/assets/terrain.png",
                &textureWidth, &textureHeight, &nrChannels, 0);
  if (textureData == NULL) {
    LOG("Texture", "Couldn't load texture from file");
  }

  // single vao for the whole app
  GLuint vao;
  CALL_GL(glGenVertexArrays(1, &vao));
  CALL_GL(glBindVertexArray(vao));

  // TODO: bind in loop
  GLuint texture;
  CALL_GL(glGenTextures(1, &texture));
  CALL_GL(glActiveTexture(GL_TEXTURE0));
  CALL_GL(glBindTexture(GL_TEXTURE_2D, texture));

  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                          GL_LINEAR_MIPMAP_NEAREST));
  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight,
                       0, GL_RGBA, GL_UNSIGNED_BYTE, textureData));
  CALL_GL(glGenerateMipmap(GL_TEXTURE_2D));
  stbi_image_free(textureData);

  GLuint chunkShaderProgram = CreateShaderProgram(
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/shader.vert",
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/shader.frag");

  struct Camera* camera = CreateCamera();

  float deltaTimeS = 0.f;
  float lastTimeS = 0.f;

  // Create and configure noise state
  fnl_state noise = fnlCreateState();
  noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

  int renderDistance = 8;
  int visibleChunkCount = (int)pow(renderDistance * 2 + 1, 2);
  int maxLoadedChunks = visibleChunkCount * 2;

  int chunkCount = 0;
  // TODO: free this and free chunks at program exit
  int loadedChunksSize = maxLoadedChunks;
  struct Chunk** loadedChunks = calloc(maxLoadedChunks, sizeof(struct Chunk*));

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

    MoveCamera((float)mouseX, (float)mouseY, verticalInput, horizontalInput,
               deltaTimeS, camera);

    // Fog color
    CALL_GL(glClearColor(0.8f, 0.9f, 1.f, 1.f));
    CALL_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // model matrix is not used since chunks are static
    // use the same mvp for all chunks
    mat4 mvp;
    glm_mat4_mulN((mat4*[]){&(camera->projectionMatrix), &(camera->viewMatrix)},
                  2, mvp);

    // Bind shader
    CALL_GL(glUseProgram(chunkShaderProgram));

    CALL_GL(GLint MVPUniformLocation =
                glGetUniformLocation(chunkShaderProgram, "u_MVP"));
    CALL_GL(glUniformMatrix4fv(MVPUniformLocation, 1, GL_FALSE, mvp[0]));

    int playerChunkX =
        (int)floor(camera->transform.translation[0] / CHUNK_LENGTH);
    int playerChunkZ =
        (int)floor(camera->transform.translation[2] / CHUNK_LENGTH);

    int chunksToRenderIndex = 0;
    struct Chunk** chunksToRender =
        calloc(visibleChunkCount, sizeof(struct Chunk*));
    for (int x = -renderDistance; x <= renderDistance; x++) {
      for (int z = -renderDistance; z <= renderDistance; z++) {
        struct Chunk* chunk = GetChunk(x + playerChunkX, z + playerChunkZ,
                                       loadedChunks, loadedChunksSize, &noise);
        // free space for new chunks
        // TODO: if loading too fast this leaves holes unloaded
        if (chunk == NULL) {
          UnloadChunks(
              playerChunkX - renderDistance, playerChunkZ - renderDistance,
              playerChunkX + renderDistance, playerChunkZ + renderDistance,
              loadedChunks, loadedChunksSize);
          continue;
        }

        chunksToRender[chunksToRenderIndex++] = chunk;
        // DrawBlocks(chunk);
      }
    }

    for (int i = 0; i < chunksToRenderIndex; i++) {
      DrawBlocks(chunksToRender[i]);
    }
    for (int i = 0; i < chunksToRenderIndex; i++) {
      DrawWater(chunksToRender[i]);
    }

    free(chunksToRender);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();

  //_CrtDumpMemoryLeaks();
  return 0;
}