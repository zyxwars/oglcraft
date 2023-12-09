#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <stb_image.h>
#define FNL_IMPL
#include <FastNoiseLite.h>

#include "Camera.h"
#include "Renderer/GlWrapper.h"
#include "Renderer/Shader.h"
#include "Renderer/Skybox.h"
#include "Terrain/Chunk.h"
#include "Renderer/HeldItem.h"
#include "Game/Player.h"
// #include "Text.h"

// #define _CRTDBG_MAP_ALLOC
// #include<crtdbg.h>

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

int main(void) {
  struct Player player = PLAYER_INIT;

  // Init glfw
  GLFWwindow* window;

  if (!glfwInit()) return -1;

  // display scaling
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

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

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, 0);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                        GL_TRUE);

  CALL_GL(glEnable(GL_CULL_FACE));
  CALL_GL(glEnable(GL_DEPTH_TEST));

  int textureWidth, textureHeight, nrChannels;
  unsigned char* textureData =
      stbi_load("C:/Users/Zyxwa/Documents/code/oglc/src/assets/terrain.png",
                &textureWidth, &textureHeight, &nrChannels, 0);
  if (textureData == NULL) {
    LOG("Texture", "Couldn't load texture from file");
  }

  // TODO: bind in loop
  GLuint texture;
  CALL_GL(glGenTextures(1, &texture));
  CALL_GL(glActiveTexture(GL_TEXTURE0));
  CALL_GL(glBindTexture(GL_TEXTURE_2D, texture));

  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  CALL_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight,
                       0, GL_RGBA, GL_UNSIGNED_BYTE, textureData));
  // CALL_GL(glGenerateMipmap(GL_TEXTURE_2D));

  stbi_image_free(textureData);

  GLuint chunkShaderProgram = CreateShaderProgram(
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/shader.vert",
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/shader.frag");

  struct Camera* camera = CreateCamera();

  float deltaTimeS = 0.f;
  float lastTimeS = 0.f;

  // Create and configure noise state
  struct GenerationNoise generationNoise = {0};

  generationNoise.continentalness = fnlCreateState();
  generationNoise.continentalness.noiseType = FNL_NOISE_OPENSIMPLEX2;
  generationNoise.continentalness.seed = 1;

  generationNoise.temperature = fnlCreateState();
  generationNoise.temperature.noiseType = FNL_NOISE_OPENSIMPLEX2;
  generationNoise.temperature.seed = 2;

  generationNoise.humidity = fnlCreateState();
  generationNoise.humidity.noiseType = FNL_NOISE_OPENSIMPLEX2;
  generationNoise.humidity.seed = 3;

  srand(1);

  int renderDistance = 8;
  int visibleChunkCount = (int)pow(renderDistance * 2 + 1, 2);
  int maxLoadedChunks = visibleChunkCount * 2;

  int chunkCount = 0;
  // TODO: free this and free chunks at program exit
  int loadedChunksSize = maxLoadedChunks;
  struct Chunk** loadedChunks = calloc(maxLoadedChunks, sizeof(struct Chunk*));

  struct Skybox* skybox = CreateSkybox();

  struct HeldItemRenderer* heldItemRenderer =
      CreateHeldItemRenderer(player.heldItem);

  float selectionVertices[] = {
      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
      1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f};

  GLuint selectionShaderProgram = CreateShaderProgram(
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/selection.vert",
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/selection.frag");

  GLuint selectionVao;
  CALL_GL(glGenVertexArrays(1, &selectionVao));
  CALL_GL(glBindVertexArray(selectionVao));

  GLuint selectionVbo;
  CALL_GL(glGenBuffers(1, &selectionVbo));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, selectionVbo));
  CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(selectionVertices),
                       selectionVertices, GL_STATIC_DRAW));
  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));

  CALL_GL(glBindVertexArray(0));

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

    // TODO: just for testing
    enum BlockId lastHeldItem = player.heldItem;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
      player.heldItem = BLOCK_STONE;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
      player.heldItem = BLOCK_GRASS;
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
      player.heldItem = BLOCK_OAK_LOG;
    } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
      player.heldItem = BLOCK_DIRT;
    }
    if (player.heldItem != lastHeldItem) {
      HeldItemRendererUpdate(heldItemRenderer, player.heldItem);
    }

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    CameraMove((float)mouseX, (float)mouseY, verticalInput, horizontalInput,
               deltaTimeS, camera);

    // Fog color
    CALL_GL(glClearColor(1.f, 0.f, 1.f, 1.f));
    CALL_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    DrawSkybox(skybox, camera->viewMatrix, camera->projectionMatrix);

    CALL_GL(glUseProgram(chunkShaderProgram));

    // model matrix is not used since chunks are static
    // use the same mvp for all chunks
    mat4 mvp = {0};
    glm_mat4_mul(camera->projectionMatrix, camera->viewMatrix, mvp);

    CALL_GL(GLint MVPUniform =
                glGetUniformLocation(chunkShaderProgram, "u_MVP"));
    CALL_GL(glUniformMatrix4fv(MVPUniform, 1, GL_FALSE, mvp[0]));

    CALL_GL(GLint timeUniform =
                glGetUniformLocation(chunkShaderProgram, "u_TimeS"));
    CALL_GL(glUniform1f(timeUniform, currentTimeS));

    int playerChunkX =
        (int)floor(camera->transform.translation[0] / CHUNK_LENGTH);
    int playerChunkZ =
        (int)floor(camera->transform.translation[2] / CHUNK_LENGTH);

    int chunksToRenderIndex = 0;
    struct Chunk** chunksToRender =
        calloc(visibleChunkCount, sizeof(struct Chunk*));
    for (int x = -renderDistance; x <= renderDistance; x++) {
      for (int z = -renderDistance; z <= renderDistance; z++) {
        struct Chunk* chunk =
            GetChunk(x + playerChunkX, z + playerChunkZ, loadedChunks,
                     loadedChunksSize, &generationNoise);
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
      }
    }

    for (int i = 0; i < chunksToRenderIndex; i++) {
      DrawChunkMesh(&(chunksToRender[i]->opaqueMesh));
    }

    // // Only blend translucent buffer
    CALL_GL(glEnable(GL_BLEND));
    CALL_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    for (int i = 0; i < chunksToRenderIndex; i++) {
      DrawChunkMesh(&(chunksToRender[i]->translucentMesh));
    }

    free(chunksToRender);
    CALL_GL(glDisable(GL_BLEND));
    CALL_GL(glUseProgram(0));

    // TODO: refactor this mess
    // TODO: raycast
    float rayDistance = 10.f;
    float step = 0.1f;
    float currentStep = 0.f;
    int isHit = 0;
    ivec3 selectionPos = {0};
    ivec3 lastStepPos = {0};
    while (currentStep < rayDistance) {
      vec3 hitPos;
      glm_vec3_scale(camera->transform.forward, currentStep, hitPos);
      glm_vec3_add(hitPos, camera->transform.translation, hitPos);
      currentStep += step;

      selectionPos[0] = (int)round(hitPos[0]);
      selectionPos[1] = (int)round(hitPos[1]);
      selectionPos[2] = (int)round(hitPos[2]);

      // TODO:
      if (selectionPos[1] >= CHUNK_HEIGHT) continue;

      ivec2 selectionChunkPos = {
          (int)floor((float)selectionPos[0] / (CHUNK_LENGTH)),
          (int)floor((float)selectionPos[2] / (CHUNK_LENGTH))};

      struct Chunk* hitChunk =
          GetChunk(selectionChunkPos[0], selectionChunkPos[1], loadedChunks,
                   loadedChunksSize, &generationNoise);

      ivec3 selectionInChunkPos = {0};
      selectionInChunkPos[1] = selectionPos[1];

      if (selectionPos[0] < 0) {
        selectionInChunkPos[0] = abs(selectionPos[0] % CHUNK_LENGTH);
        if (selectionInChunkPos[0] != 0) {
          selectionInChunkPos[0] = CHUNK_LENGTH - selectionInChunkPos[0];
        }

      } else {
        selectionInChunkPos[0] = (selectionPos[0]) % CHUNK_LENGTH;
      }

      if (selectionPos[2] < 0) {
        selectionInChunkPos[2] = abs(selectionPos[2] % CHUNK_LENGTH);
        if (selectionInChunkPos[2] != 0) {
          selectionInChunkPos[2] = CHUNK_LENGTH - selectionInChunkPos[2];
        }
      } else {
        selectionInChunkPos[2] = (selectionPos[2]) % CHUNK_LENGTH;
      }

      unsigned int selectedBlock = hitChunk->blocks[PosInChunkToIndex(
          selectionInChunkPos[0], selectionInChunkPos[1],
          selectionInChunkPos[2])];

      if (selectedBlock == 0) {
        lastStepPos[0] = selectionPos[0];
        lastStepPos[1] = selectionPos[1];
        lastStepPos[2] = selectionPos[2];
        continue;
      }

      isHit = 1;

      printf("%d %d %d %d %d [%d %d %d]\n", selectionPos[0], selectionPos[1],
             selectionPos[2], selectionChunkPos[0], selectionChunkPos[1],
             selectionInChunkPos[0], selectionInChunkPos[1],
             selectionInChunkPos[2]);

      if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (currentTimeS - player.lastBreakTimeS < player.breakCooldownS) break;

        // TODO: don't do this raw
        hitChunk->blocks[PosInChunkToIndex(selectionInChunkPos[0],
                                           selectionInChunkPos[1],
                                           selectionInChunkPos[2])] = 0;

        player.lastBreakTimeS = currentTimeS;
        UpdateOpaqueMesh(hitChunk);

        break;
      }

      if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        if (currentTimeS - player.lastBreakTimeS < player.breakCooldownS) break;

        ivec2 testChunkPos = {
            (int)floor((float)lastStepPos[0] / (CHUNK_LENGTH)),
            (int)floor((float)lastStepPos[2] / (CHUNK_LENGTH))};

        struct Chunk* testChunk =
            GetChunk(testChunkPos[0], testChunkPos[1], loadedChunks,
                     loadedChunksSize, &generationNoise);

        ivec3 testInChunkPos = {0};
        testInChunkPos[1] = lastStepPos[1];

        if (lastStepPos[0] < 0) {
          testInChunkPos[0] = CHUNK_LENGTH + ((lastStepPos[0]) % CHUNK_LENGTH);
        } else {
          testInChunkPos[0] = (lastStepPos[0]) % CHUNK_LENGTH;
        }

        if (lastStepPos[2] < 0) {
          testInChunkPos[2] = CHUNK_LENGTH + ((lastStepPos[2]) % CHUNK_LENGTH);
        } else {
          testInChunkPos[2] = (lastStepPos[2]) % CHUNK_LENGTH;
        }

        unsigned int testBlock = testChunk->blocks[PosInChunkToIndex(
            testInChunkPos[0], testInChunkPos[1], testInChunkPos[2])];

        // TODO:
        if (testBlock != 0) break;

        // TODO: don't do this raw
        testChunk->blocks[PosInChunkToIndex(
            testInChunkPos[0], testInChunkPos[1], testInChunkPos[2])] =
            player.heldItem;

        player.lastBreakTimeS = currentTimeS;
        UpdateOpaqueMesh(testChunk);

        break;
      }

      break;
    }

    if (isHit) {
      CALL_GL(glUseProgram(selectionShaderProgram));
      CALL_GL(glBindVertexArray(selectionVao));

      mat4 selectionMvp;
      mat4 selectionModelMat = GLM_MAT4_IDENTITY_INIT;
      glm_mat4_scale(selectionModelMat, 0.51f);
      selectionModelMat[3][0] = (float)selectionPos[0];
      selectionModelMat[3][1] = (float)selectionPos[1];
      selectionModelMat[3][2] = (float)selectionPos[2];
      selectionModelMat[3][3] = 1;

      // TODO: this doesn't work in a function?, use mul instead of mulN
      glm_mat4_mulN((mat4*[]){&(camera->projectionMatrix),
                              &(camera->viewMatrix), &selectionModelMat},
                    3, selectionMvp);

      // mvp matrix
      CALL_GL(GLint selectionMvpUniform =
                  glGetUniformLocation(selectionShaderProgram, "u_MVP"));
      CALL_GL(glUniformMatrix4fv(selectionMvpUniform, 1, GL_FALSE,
                                 selectionMvp[0]));

      CALL_GL(glEnable(GL_BLEND));
      CALL_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      CALL_GL(glDrawArrays(GL_TRIANGLES, 0, 36));

      CALL_GL(glDisable(GL_BLEND));

      CALL_GL(glBindVertexArray(0));
      CALL_GL(glUseProgram(0));
    }

    HeldItemRendererDraw(heldItemRenderer, camera);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();

  //_CrtDumpMemoryLeaks();
  return 0;
}