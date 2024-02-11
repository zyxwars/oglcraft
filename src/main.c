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
#include "Renderer/Selection.h"
#include "Renderer/Chunk.h"
#include "Terrain/Chunk.h"
#include "Renderer/HeldItem.h"
#include "Game/Player.h"
#include "Game/ChunkManager.h"
#include "Game/Raycaster.h"
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
  unsigned char* textureData = stbi_load(
      "C:/Users/Zyxwa/Documents/_Zyxwars/code/oglc/src/assets/terrain.png",
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
      "C:/Users/Zyxwa/Documents/_Zyxwars/code/oglc/src/shaders/shader.vert",
      "C:/Users/Zyxwa/Documents/_Zyxwars/code/oglc/src/shaders/shader.frag");

  struct Camera* camera = CreateCamera();

  float deltaTimeS = 0.f;
  float lastTimeS = 0.f;

  struct ChunkManager* chunkManager = CreateChunkManager();
  srand(1);

  struct Skybox* skybox = CreateSkyboxRenderer();

  struct HeldItemRenderer* heldItemRenderer =
      CreateHeldItemRenderer(player.heldItem);

  struct SelectionRenderer* selectionRenderer = CreateSelectionRenderer();

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

    CALL_GL(glClearColor(1.f, 0.f, 1.f, 1.f));
    CALL_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    SkyboxRendererDraw(skybox, camera->viewMatrix, camera->projectionMatrix);

    ivec2 playerChunkPos;
    PosToChunkPos(camera->transform.translation[0],
                  camera->transform.translation[2], &playerChunkPos);

    ChunkManagerUpdate(chunkManager, playerChunkPos);

    // TODO:
    ChunkRendererDraw(chunkManager->activeChunks,
                      chunkManager->activeChunksCount, playerChunkPos,
                      chunkManager->renderDistance, chunkShaderProgram,
                      currentTimeS, camera);

    ivec3 hitPos;
    ivec3 beforeHitPos;
    int isHit = CastRay(chunkManager, camera->transform.translation,
                        camera->transform.forward, &hitPos, &beforeHitPos);

    if (isHit) {
      SelectionRendererDraw(selectionRenderer, hitPos, camera);

      if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (currentTimeS - player.lastBreakTimeS > player.breakCooldownS) {
          ivec2 chunkPos;
          PosToChunkPos(hitPos[0], hitPos[2], &chunkPos);
          struct Chunk* chunk = GetChunk(chunkManager, chunkPos);

          ivec3 posInChunk;
          PosToPosInChunk(hitPos, &posInChunk);
          // TODO: don't do this raw
          chunk->blocks[PosInChunkToIndex(posInChunk[0], posInChunk[1],
                                          posInChunk[2])] = 0;

          player.lastBreakTimeS = currentTimeS;
          UpdateChunkMeshes(chunk);
        }
      } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) ==
                 GLFW_PRESS) {
        if (currentTimeS - player.lastBreakTimeS > player.breakCooldownS) {
          ivec2 chunkPos;
          PosToChunkPos(beforeHitPos[0], beforeHitPos[2], &chunkPos);
          struct Chunk* chunk = GetChunk(chunkManager, chunkPos);

          ivec3 posInChunk;
          PosToPosInChunk(beforeHitPos, &posInChunk);
          // TODO: don't do this raw
          chunk->blocks[PosInChunkToIndex(posInChunk[0], posInChunk[1],
                                          posInChunk[2])] = player.heldItem;

          player.lastBreakTimeS = currentTimeS;
          UpdateChunkMeshes(chunk);
        }
      }
    }

    HeldItemRendererDraw(heldItemRenderer, camera);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();

  //_CrtDumpMemoryLeaks();
  return 0;
}