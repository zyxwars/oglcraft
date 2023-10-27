#include "Chunk.h"

int PosToIndex(int x, int y, int z) {
  return x + z * CHUNK_LENGTH + y * CHUNK_PLANE_AREA;
}

// TODO: create update function
void CreateBlockMesh(struct Chunk* chunk) {
  CALL_GL(glGenBuffers(1, &(chunk->blockMesh.vbo)));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, chunk->blockMesh.vbo));

  CALL_GL(glGenBuffers(1, &(chunk->blockMesh.ebo)));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->blockMesh.ebo));

  // TODO: allocate in a less wasteful way?
  const int maxFacesInChunk = ((int)CHUNK_VOLUME * 6);

  struct Vertex* vertices = malloc(maxFacesInChunk * 4 * sizeof(struct Vertex));
  unsigned int* triangles = malloc(maxFacesInChunk * 6 * sizeof(unsigned int));

  int currentFaceIndex = 0;
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        AddToBlockBuffer(chunk->blocks, x, y, z, x + chunk->x * CHUNK_LENGTH,
                         z + chunk->z * CHUNK_LENGTH, &currentFaceIndex,
                         vertices, triangles);
      }
    }
  }

  // We copy only the existing vertices based on face index
  // ignoring the rest of data that is garbage, allocated for the worst case
  // when culling faces
  CALL_GL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(struct Vertex) * 4 * currentFaceIndex, vertices,
                       GL_STATIC_DRAW));
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(unsigned int) * 6 * currentFaceIndex, triangles,
               GL_STATIC_DRAW);
  free(vertices);
  free(triangles);
  vertices = NULL;
  triangles = NULL;

  chunk->blockFaceCount = currentFaceIndex;
}

void CreateWaterMesh(struct Chunk* chunk) {
  CALL_GL(glGenBuffers(1, &(chunk->waterMesh.vbo)));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, chunk->waterMesh.vbo));

  CALL_GL(glGenBuffers(1, &(chunk->waterMesh.ebo)));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->waterMesh.ebo));

  // TODO: allocate in a less wasteful way?
  const int maxFacesInChunk = ((int)CHUNK_VOLUME * 6);

  struct Vertex* vertices = malloc(maxFacesInChunk * 4 * sizeof(struct Vertex));
  unsigned int* triangles = malloc(maxFacesInChunk * 6 * sizeof(unsigned int));

  int currentFaceIndex = 0;
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        AddToWaterBuffer(chunk->blocks, x, y, z, x + chunk->x * CHUNK_LENGTH,
                         z + chunk->z * CHUNK_LENGTH, &currentFaceIndex,
                         vertices, triangles);
      }
    }
  }

  // We copy only the existing vertices based on face index
  // ignoring the rest of data that is garbage, allocated for the worst case
  // when culling faces
  CALL_GL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(struct Vertex) * 4 * currentFaceIndex, vertices,
                       GL_STATIC_DRAW));
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(unsigned int) * 6 * currentFaceIndex, triangles,
               GL_STATIC_DRAW);
  free(vertices);
  free(triangles);
  vertices = NULL;
  triangles = NULL;

  chunk->waterBlockFaceCount = currentFaceIndex;
}

struct Chunk* CreateChunk(fnl_state* noiseState, int chunkX, int chunkZ) {
  struct Chunk* chunk = calloc(1, sizeof(struct Chunk));
  chunk->x = chunkX;
  chunk->z = chunkZ;

  // Populate chunk
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int z = 0; z < CHUNK_LENGTH; z++) {
      // TODO: proper block types
      float noise = fnlGetNoise2D(noiseState, x + chunk->x * CHUNK_LENGTH,
                                  z + chunk->z * CHUNK_LENGTH);
      noise = (noise + 1.f) / 2.f;
      // noise = (float)pow(noise, 3);
      int height = noise * CHUNK_HEIGHT;
      if (height == 0) height = 1;
      // Fill every block under height
      for (int y = 0; y < height; y++) {
        chunk->blocks[PosToIndex(x, y, z)] = 1;
      }
    }
  }

  // Fill in water
  int waterHeight = 10;
  for (int y = 0; y < waterHeight; y++) {
    for (int x = 0; x < CHUNK_LENGTH; x++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        // Fill air blocks with water
        if (chunk->blocks[PosToIndex(x, y, z)] == 0) {
          chunk->blocks[PosToIndex(x, y, z)] = 2;
        }
      }
    }
  }

  // Turn blocks under water to dirt
  for (int y = 0; y < waterHeight - 1; y++) {
    for (int x = 0; x < CHUNK_LENGTH; x++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        if (chunk->blocks[PosToIndex(x, y + 1, z)] == 2 &&
            chunk->blocks[PosToIndex(x, y, z)] != 2) {
          chunk->blocks[PosToIndex(x, y, z)] = 3;
        }
      }
    }
  }

  CreateBlockMesh(chunk);
  CreateWaterMesh(chunk);

  return chunk;
};

void DrawBlocks(struct Chunk* chunk) {
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, chunk->blockMesh.vbo));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->blockMesh.ebo));

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

  CALL_GL(glDrawElements(GL_TRIANGLES, chunk->blockFaceCount * 6,
                         GL_UNSIGNED_INT, 0));
}

void DrawWater(struct Chunk* chunk) {
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, chunk->waterMesh.vbo));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->waterMesh.ebo));

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

  CALL_GL(glDrawElements(GL_TRIANGLES, chunk->waterBlockFaceCount * 6,
                         GL_UNSIGNED_INT, 0));
};

void DestroyChunk(struct Chunk** chunk) {
  CALL_GL(glDeleteBuffers(1, &(*chunk)->blockMesh.vbo));
  CALL_GL(glDeleteBuffers(1, &(*chunk)->blockMesh.ebo));
  CALL_GL(glDeleteBuffers(1, &(*chunk)->waterMesh.vbo));
  CALL_GL(glDeleteBuffers(1, &(*chunk)->waterMesh.ebo));
  free(*chunk);
  *chunk = NULL;
}

struct Chunk* GetChunk(int x, int z, struct Chunk** loadedChunks,
                       int loadedChunksSize, fnl_state* noiseState) {
  // Use empty index to store new chunk
  int emptyIndex = -1;
  for (int i = 0; i < loadedChunksSize; i++) {
    // Skips empty chunks
    if (loadedChunks[i] == NULL) {
      if (emptyIndex == -1) emptyIndex = i;
      continue;
    }

    // Find chunk
    if (loadedChunks[i]->x == x && loadedChunks[i]->z == z) {
      return loadedChunks[i];
    }
  }

  // Chunks weren't unloaded in time
  if (emptyIndex == -1) {
    return NULL;
  }

  // Create chunk
  struct Chunk* chunk = CreateChunk(noiseState, x, z);
  printf("Chunk: (%d, %d) created\n", chunk->x, chunk->z);
  loadedChunks[emptyIndex] = chunk;

  return chunk;
};

void UnloadChunks(int minChunkX, int minChunkZ, int maxChunkX, int maxChunkZ,
                  struct Chunk** loadedChunks, int loadedChunksSize) {
  for (int i = 0; i < loadedChunksSize; i++) {
    if (loadedChunks[i] == NULL) {
      continue;
    }

    // Free chunk outside loaded area
    if (loadedChunks[i]->x < minChunkX || loadedChunks[i]->x > maxChunkX ||
        loadedChunks[i]->z < minChunkZ || loadedChunks[i]->z > maxChunkZ) {
      DestroyChunk(&loadedChunks[i]);
    }
  }
}