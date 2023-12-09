
#include "ChunkManager.h"

struct ChunkManager* CreateChunkManager() {
  struct ChunkManager* manager = calloc(1, sizeof(struct ChunkManager));

  manager->renderDistance = 8;

  manager->noise.continentalness = fnlCreateState();
  manager->noise.continentalness.noiseType = FNL_NOISE_OPENSIMPLEX2;
  manager->noise.continentalness.seed = 1;
  manager->noise.temperature = fnlCreateState();
  manager->noise.temperature.noiseType = FNL_NOISE_OPENSIMPLEX2;
  manager->noise.temperature.seed = 2;
  manager->noise.humidity = fnlCreateState();
  manager->noise.humidity.noiseType = FNL_NOISE_OPENSIMPLEX2;
  manager->noise.humidity.seed = 3;

  // TODO: consistent random function

  manager->visibleChunkCount = (int)pow(manager->renderDistance * 2 + 1, 2);
  manager->maxLoadedChunks = manager->visibleChunkCount * 2;
  manager->loadedChunks =
      calloc(manager->maxLoadedChunks, sizeof(struct Chunk*));

  manager->chunksToRender =
      calloc(manager->visibleChunkCount, sizeof(struct Chunk*));

  manager->shader = CreateShaderProgram(
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/shader.vert",
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/shader.frag");

  return manager;
}

void ChunkManagerUpdate(struct ChunkManager* const manager, ivec3 playerPos) {
  int playerChunkX = (int)floor(playerPos[0] / CHUNK_LENGTH);
  int playerChunkZ = (int)floor(playerPos[2] / CHUNK_LENGTH);

  int chunksToRenderIndex = 0;
  struct Chunk** chunksToRender =
      calloc(manager->visibleChunkCount, sizeof(struct Chunk*));
  for (int x = -manager->renderDistance; x <= manager->renderDistance; x++) {
    for (int z = -manager->renderDistance; z <= manager->renderDistance; z++) {
      struct Chunk* chunk =
          GetChunk(x + playerChunkX, z + playerChunkZ, manager->loadedChunks,
                   manager->maxLoadedChunks, &(manager->noise));
      // free space for new chunks
      // TODO: if loading too fast this leaves holes unloaded
      if (chunk == NULL) {
        UnloadChunks(playerChunkX - manager->renderDistance,
                     playerChunkZ - manager->renderDistance,
                     playerChunkX + manager->renderDistance,
                     playerChunkZ + manager->renderDistance,
                     manager->loadedChunks, manager->maxLoadedChunks);
        continue;
      }

      chunksToRender[chunksToRenderIndex++] = chunk;
    }
  }
}

void ChunkManagerDraw(struct ChunkManager* const manager,
                      struct Camera* const camera, float currentTimeS) {
  CALL_GL(glUseProgram(manager->shader));

  // model matrix is not used since chunks are static
  // use the same mvp for all chunks
  mat4 mvp = {0};
  glm_mat4_mul(camera->projectionMatrix, camera->viewMatrix, mvp);

  CALL_GL(GLint mvpUniform = glGetUniformLocation(manager->shader, "u_MVP"));
  CALL_GL(glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, mvp[0]));

  CALL_GL(GLint timeUniform = glGetUniformLocation(manager->shader, "u_TimeS"));
  CALL_GL(glUniform1f(timeUniform, currentTimeS));

  for (int i = 0; i < manager->visibleChunkCount; i++) {
    DrawChunkMesh(&(manager->chunksToRender[i]->opaqueMesh));
  }

  // // Only blend translucent buffer
  CALL_GL(glEnable(GL_BLEND));
  CALL_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  for (int i = 0; i < manager->visibleChunkCount; i++) {
    DrawChunkMesh(&(manager->chunksToRender[i]->translucentMesh));
  }

  CALL_GL(glDisable(GL_BLEND));
  CALL_GL(glUseProgram(0));
}