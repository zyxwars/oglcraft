#include "ChunkManager.h"

struct ChunkManager* CreateChunkManager() {
  struct ChunkManager* manager = calloc(1, sizeof(struct ChunkManager));

  manager->noise.continentalness = fnlCreateState();
  manager->noise.continentalness.noiseType = FNL_NOISE_OPENSIMPLEX2;
  manager->noise.continentalness.seed = 1;
  manager->noise.temperature = fnlCreateState();
  manager->noise.temperature.noiseType = FNL_NOISE_OPENSIMPLEX2;
  manager->noise.temperature.seed = 2;
  manager->noise.humidity = fnlCreateState();
  manager->noise.humidity.noiseType = FNL_NOISE_OPENSIMPLEX2;
  manager->noise.humidity.seed = 3;

  manager->renderDistance = 8;
  int visibleChunkCount = (int)pow(manager->renderDistance * 2 + 1, 2);
  manager->activeChunksCount = visibleChunkCount * 2;
  manager->activeChunks =
      calloc(manager->activeChunksCount, sizeof(struct Chunk*));

  return manager;
}

void ChunkManagerUpdate(struct ChunkManager* manager, ivec2 playerChunkPos) {
  for (int x = -manager->renderDistance; x <= manager->renderDistance; x++) {
    for (int z = -manager->renderDistance; z <= manager->renderDistance; z++) {
      ivec2 currChunkPos = {x + playerChunkPos[0], z + playerChunkPos[1]};

      int isLoaded = 0;
      int freeIndex = -1;
      for (int i = 0; i < manager->activeChunksCount; i++) {
        if (manager->activeChunks[i] == NULL) {
          freeIndex = i;
          continue;
        }

        if (manager->activeChunks[i]->x == currChunkPos[0] &&
            manager->activeChunks[i]->z == currChunkPos[1])
          isLoaded = 1;
      }

      if (isLoaded) continue;

      if (freeIndex == -1) {
        int minX = playerChunkPos[0] - manager->renderDistance;
        int minZ = playerChunkPos[1] - manager->renderDistance;
        int maxX = playerChunkPos[0] + manager->renderDistance;
        int maxZ = playerChunkPos[1] + manager->renderDistance;

        for (int i = 0; i < manager->activeChunksCount; i++) {
          struct Chunk* curr = manager->activeChunks[i];

          int isVisible = curr->x > minX && curr->z > minZ && curr->x < maxX &&
                          curr->z < maxZ;

          if (!isVisible) DestroyChunk(&(manager->activeChunks[i]));
          freeIndex = i;
        }
      }

      struct Chunk* newChunk =
          CreateChunk(&(manager->noise), currChunkPos[0], currChunkPos[1]);

      manager->activeChunks[freeIndex] = newChunk;
      printf("Chunk: (%d, %d) created\n", newChunk->x, newChunk->z);

      // TODO: test only create a single chunk per frame
      return;
    }
  }
}