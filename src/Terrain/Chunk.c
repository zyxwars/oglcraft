#include "Chunk.h"

int PosInChunkToIndex(int x, int y, int z) {
  return x + z * CHUNK_LENGTH + y * CHUNK_PLANE_AREA;
}

void PosToChunkPos(float x, float z, ivec2* chunkPos) {
  // the decimal precision was lost, when dividing int coords so use float
  (*chunkPos)[0] = (int)floor(x / (CHUNK_LENGTH));
  (*chunkPos)[1] = (int)floor(z / (CHUNK_LENGTH));
}

void PosToPosInChunk(ivec3 pos, ivec3* posInChunk) {
  ivec2 chunkPos = {0};
  PosToChunkPos((float)pos[0], (float)pos[2], &chunkPos);

  // y isn't chunked
  (*posInChunk)[1] = pos[1];

  if (pos[0] < 0) {
    (*posInChunk)[0] = abs(pos[0] % CHUNK_LENGTH);
    if ((*posInChunk)[0] != 0) {
      (*posInChunk)[0] = CHUNK_LENGTH - (*posInChunk)[0];
    }

  } else {
    (*posInChunk)[0] = (pos[0]) % CHUNK_LENGTH;
  }

  if (pos[2] < 0) {
    (*posInChunk)[2] = abs(pos[2] % CHUNK_LENGTH);
    if ((*posInChunk)[2] != 0) {
      (*posInChunk)[2] = CHUNK_LENGTH - (*posInChunk)[2];
    }
  } else {
    (*posInChunk)[2] = (pos[2]) % CHUNK_LENGTH;
  }
}

void CreateOpaqueMesh(struct Chunk* chunk) {
  CALL_GL(glGenVertexArrays(1, &(chunk->opaqueMesh.vao)));
  CALL_GL(glBindVertexArray(chunk->opaqueMesh.vao));

  CALL_GL(glGenBuffers(1, &(chunk->opaqueMesh.vbo)));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, chunk->opaqueMesh.vbo));

  CALL_GL(glGenBuffers(1, &(chunk->opaqueMesh.ebo)));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->opaqueMesh.ebo));

  int stride = sizeof(struct BlockVertex);

  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glVertexAttribIPointer(0, 1, GL_INT, stride,
                                 (void*)offsetof(struct BlockVertex, blockId)));

  CALL_GL(glEnableVertexAttribArray(1));
  CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct BlockVertex, position)));

  CALL_GL(glEnableVertexAttribArray(2));
  CALL_GL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct BlockVertex, normal)));

  CALL_GL(glEnableVertexAttribArray(3));
  CALL_GL(
      glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride,
                            (void*)offsetof(struct BlockVertex, texCoords)));

  // TODO: allocate in a less wasteful way?
  const int maxFacesInChunk = ((int)CHUNK_VOLUME * 6);

  struct BlockVertex* vertices =
      calloc(maxFacesInChunk * 4, sizeof(struct BlockVertex));
  unsigned int* triangles = calloc(maxFacesInChunk * 6, sizeof(unsigned int));

  int currentFaceIndex = 0;
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        AddToOpaqueBuffer(chunk->blocks, x, y, z, x + chunk->x * CHUNK_LENGTH,
                          z + chunk->z * CHUNK_LENGTH, &currentFaceIndex,
                          vertices, triangles);
      }
    }
  }

  // We copy only the existing vertices based on face index
  // ignoring the rest of data that is garbage, allocated for the worst case
  // when culling faces
  CALL_GL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(struct BlockVertex) * 4 * currentFaceIndex,
                       vertices, GL_STATIC_DRAW));
  CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       sizeof(unsigned int) * 6 * currentFaceIndex, triangles,
                       GL_STATIC_DRAW));
  free(vertices);
  free(triangles);
  vertices = NULL;
  triangles = NULL;

  CALL_GL(glBindVertexArray(0));

  chunk->opaqueMesh.faceCount = currentFaceIndex;
}

void CreateTranslucentMesh(struct Chunk* chunk) {
  CALL_GL(glGenVertexArrays(1, &(chunk->translucentMesh.vao)));
  CALL_GL(glBindVertexArray(chunk->translucentMesh.vao));

  CALL_GL(glGenBuffers(1, &(chunk->translucentMesh.vbo)));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, chunk->translucentMesh.vbo));

  CALL_GL(glGenBuffers(1, &(chunk->translucentMesh.ebo)));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->translucentMesh.ebo));

  int stride = sizeof(struct BlockVertex);

  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glVertexAttribIPointer(0, 1, GL_INT, stride,
                                 (void*)offsetof(struct BlockVertex, blockId)));

  CALL_GL(glEnableVertexAttribArray(1));
  CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct BlockVertex, position)));

  CALL_GL(glEnableVertexAttribArray(2));
  CALL_GL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct BlockVertex, normal)));

  CALL_GL(glEnableVertexAttribArray(3));
  CALL_GL(
      glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride,
                            (void*)offsetof(struct BlockVertex, texCoords)));

  // TODO: allocate in a less wasteful way?
  const int maxFacesInChunk = ((int)CHUNK_VOLUME * 6);

  struct BlockVertex* vertices =
      calloc(maxFacesInChunk * 4, sizeof(struct BlockVertex));
  unsigned int* triangles = calloc(maxFacesInChunk * 6, sizeof(unsigned int));

  int currentFaceIndex = 0;
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        AddToTranslucentBuffer(chunk->blocks, x, y, z,
                               x + chunk->x * CHUNK_LENGTH,
                               z + chunk->z * CHUNK_LENGTH, &currentFaceIndex,
                               vertices, triangles);
      }
    }
  }

  // We copy only the existing vertices based on face index
  // ignoring the rest of data that is garbage, allocated for the worst case
  // when culling faces
  CALL_GL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(struct BlockVertex) * 4 * currentFaceIndex,
                       vertices, GL_STATIC_DRAW));
  CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       sizeof(unsigned int) * 6 * currentFaceIndex, triangles,
                       GL_STATIC_DRAW));
  free(vertices);
  free(triangles);
  vertices = NULL;
  triangles = NULL;

  CALL_GL(glBindVertexArray(0));

  chunk->translucentMesh.faceCount = currentFaceIndex;
}

void UpdateChunkMeshes(struct Chunk* chunk) {
  CALL_GL(glBindVertexArray(chunk->opaqueMesh.vao));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, chunk->opaqueMesh.vbo));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->opaqueMesh.ebo));

  // TODO: allocate in a less wasteful way?
  const int maxFacesInChunk = ((int)CHUNK_VOLUME * 6);

  struct BlockVertex* vertices =
      calloc(maxFacesInChunk * 4, sizeof(struct BlockVertex));
  unsigned int* triangles = calloc(maxFacesInChunk * 6, sizeof(unsigned int));

  int currentFaceIndex = 0;
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        AddToOpaqueBuffer(chunk->blocks, x, y, z, x + chunk->x * CHUNK_LENGTH,
                          z + chunk->z * CHUNK_LENGTH, &currentFaceIndex,
                          vertices, triangles);
      }
    }
  }

  // We copy only the existing vertices based on face index
  // ignoring the rest of data that is garbage, allocated for the worst case
  // when culling faces
  CALL_GL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(struct BlockVertex) * 4 * currentFaceIndex,
                       vertices, GL_STATIC_DRAW));
  CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       sizeof(unsigned int) * 6 * currentFaceIndex, triangles,
                       GL_STATIC_DRAW));

  CALL_GL(glBindVertexArray(0));

  chunk->opaqueMesh.faceCount = currentFaceIndex;

  CALL_GL(glBindVertexArray(chunk->translucentMesh.vao));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, chunk->translucentMesh.vbo));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->translucentMesh.ebo));

  currentFaceIndex = 0;
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        AddToTranslucentBuffer(chunk->blocks, x, y, z,
                               x + chunk->x * CHUNK_LENGTH,
                               z + chunk->z * CHUNK_LENGTH, &currentFaceIndex,
                               vertices, triangles);
      }
    }
  }

  // We copy only the existing vertices based on face index
  // ignoring the rest of data that is garbage, allocated for the worst case
  // when culling faces
  CALL_GL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(struct BlockVertex) * 4 * currentFaceIndex,
                       vertices, GL_STATIC_DRAW));
  CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       sizeof(unsigned int) * 6 * currentFaceIndex, triangles,
                       GL_STATIC_DRAW));
  free(vertices);
  free(triangles);
  vertices = NULL;
  triangles = NULL;

  CALL_GL(glBindVertexArray(0));

  chunk->translucentMesh.faceCount = currentFaceIndex;
}

float SamplePerlinNoise(fnl_state* noiseState, float x, float z, int octaves,
                        float persistence, float scale) {
  float total = 0;
  float frequency = 1;
  float amplitude = 1;
  float maxValue = 0;

  for (int i = 0; i < octaves; i++) {
    total += fnlGetNoise2D(noiseState, x * frequency * scale,
                           z * frequency * scale) *
             amplitude;

    maxValue += amplitude;

    amplitude *= persistence;
    frequency *= 2;
  }

  return total / maxValue;
}

float Lerp(float x, float x1, float x2, float y1, float y2) {
  return y1 + (x - x1) * ((y2 - y1) / (x2 - x1));
}

int InterpolateHeight(float x) {
  // if (x < 0.1f) {
  //   return (int)Lerp(x, -1.f, 0.1f, 0, 30);
  // } else if (x < 0.4f) {
  //   return (int)Lerp(x, 0.1f, 0.4f, 30, 80);
  // }

  // return (int)Lerp(x, 0.4f, 1.f, 80, 85);
  return x;
}

enum Biome { BIOME_FOREST, BIOME_DESERT };

struct Chunk* CreateChunk(struct GenerationNoise* noise, int chunkX,
                          int chunkZ) {
  struct Chunk* chunk = calloc(1, sizeof(struct Chunk));
  chunk->x = chunkX;
  chunk->z = chunkZ;

  // Populate chunk
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int z = 0; z < CHUNK_LENGTH; z++) {
      float continentalness = SamplePerlinNoise(
          &(noise->continentalness), (x + (chunk->x * CHUNK_LENGTH)),
          (z + (chunk->z * CHUNK_LENGTH)), 4, 2, 0.1f);

      float temperature = SamplePerlinNoise(
          &(noise->temperature), (x + (chunk->x * CHUNK_LENGTH)),
          (z + (chunk->z * CHUNK_LENGTH)), 1, 2, 0.1f);
      temperature = (temperature + 1.f) / 2.f;

      float humidity =
          SamplePerlinNoise(&(noise->humidity), (x + (chunk->x * CHUNK_LENGTH)),
                            (z + (chunk->z * CHUNK_LENGTH)), 1, 2, 0.1f);
      humidity = (humidity + 1.f) / 2.f;

      continentalness = (continentalness + 1.f) / 2.f;
      int worldY = continentalness * (CHUNK_HEIGHT - 1);

      enum Biome biome = BIOME_FOREST;
      if (humidity < 0.2f && temperature > 0.5f) {
        biome = BIOME_DESERT;
      }

      enum BlockId surfaceBlock = BLOCK_STONE;

      switch (biome) {
        case BIOME_FOREST:
          surfaceBlock = BLOCK_GRASS;
          break;
        case BIOME_DESERT:
          surfaceBlock = BLOCK_SAND;
          break;
      }

      chunk->blocks[PosInChunkToIndex(x, worldY, z)] = surfaceBlock;

      // Fill every block under height to a solid
      for (int y = 0; y < worldY; y++) {
        chunk->blocks[PosInChunkToIndex(x, y, z)] = surfaceBlock;
      }
    }
  }

  // Fill in water
  int waterHeight = 10;
  for (int y = 0; y < waterHeight; y++) {
    for (int x = 0; x < CHUNK_LENGTH; x++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        // Fill air blocks with water
        if (chunk->blocks[PosInChunkToIndex(x, y, z)] == BLOCK_AIR) {
          chunk->blocks[PosInChunkToIndex(x, y, z)] = BLOCK_WATER;
        }
      }
    }
  }

  // Turn blocks under water to dirt
  for (int y = 0; y < waterHeight - 1; y++) {
    for (int x = 0; x < CHUNK_LENGTH; x++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        if (chunk->blocks[PosInChunkToIndex(x, y + 1, z)] == BLOCK_WATER &&
            chunk->blocks[PosInChunkToIndex(x, y, z)] != BLOCK_WATER) {
          chunk->blocks[PosInChunkToIndex(x, y, z)] = BLOCK_GRAVEL;
        }
      }
    }
  }

  // Don't spawn on chunk boundaries
  int treeX = rand() % (CHUNK_LENGTH - 4) + 2;
  int treeZ = rand() % (CHUNK_LENGTH - 4) + 2;

  for (int y = 0; y < CHUNK_HEIGHT - 8; y++) {
    // Block is air
    if (chunk->blocks[PosInChunkToIndex(treeX, y, treeZ)] != BLOCK_AIR)
      continue;

    // Spawn trunk only on grass
    if (chunk->blocks[PosInChunkToIndex(treeX, y - 1, treeZ)] != BLOCK_GRASS)
      continue;

    // Turn grass to dirt
    chunk->blocks[PosInChunkToIndex(treeX, y - 1, treeZ)] = BLOCK_DIRT;

    // Trunk
    chunk->blocks[PosInChunkToIndex(treeX, y, treeZ)] = BLOCK_OAK_LOG;
    chunk->blocks[PosInChunkToIndex(treeX, y + 1, treeZ)] = BLOCK_OAK_LOG;
    chunk->blocks[PosInChunkToIndex(treeX, y + 2, treeZ)] = BLOCK_OAK_LOG;
    chunk->blocks[PosInChunkToIndex(treeX, y + 3, treeZ)] = BLOCK_OAK_LOG;
    chunk->blocks[PosInChunkToIndex(treeX, y + 4, treeZ)] = BLOCK_OAK_LOG;
    chunk->blocks[PosInChunkToIndex(treeX, y + 5, treeZ)] = BLOCK_OAK_LOG;

    // Leaves
    // From top layer
    chunk->blocks[PosInChunkToIndex(treeX, y + 6, treeZ)] = BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 6, treeZ)] = BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX - 1, y + 6, treeZ)] = BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX, y + 6, treeZ - 1)] = BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX, y + 6, treeZ + 1)] = BLOCK_LEAVES;

    //
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 5, treeZ)] = BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX - 1, y + 5, treeZ)] = BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX, y + 5, treeZ - 1)] = BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX, y + 5, treeZ + 1)] = BLOCK_LEAVES;

    //
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 4, treeZ + 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 0, y + 4, treeZ + 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 4, treeZ + 2)] =
        BLOCK_LEAVES;

    chunk->blocks[PosInChunkToIndex(treeX + 2, y + 4, treeZ + 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 4, treeZ + 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 0, y + 4, treeZ + 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 4, treeZ + 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -2, y + 4, treeZ + 1)] =
        BLOCK_LEAVES;

    chunk->blocks[PosInChunkToIndex(treeX + 2, y + 4, treeZ + 0)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 4, treeZ + 0)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 4, treeZ + 0)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -2, y + 4, treeZ + 0)] =
        BLOCK_LEAVES;

    chunk->blocks[PosInChunkToIndex(treeX + 2, y + 4, treeZ - 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 4, treeZ - 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 0, y + 4, treeZ - 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 4, treeZ - 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -2, y + 4, treeZ - 1)] =
        BLOCK_LEAVES;

    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 4, treeZ - 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 0, y + 4, treeZ - 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 4, treeZ - 2)] =
        BLOCK_LEAVES;

    //
    chunk->blocks[PosInChunkToIndex(treeX + 2, y + 3, treeZ + 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 3, treeZ + 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 0, y + 3, treeZ + 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 3, treeZ + 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -2, y + 3, treeZ + 2)] =
        BLOCK_LEAVES;

    chunk->blocks[PosInChunkToIndex(treeX + 2, y + 3, treeZ + 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 3, treeZ + 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 0, y + 3, treeZ + 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 3, treeZ + 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -2, y + 3, treeZ + 1)] =
        BLOCK_LEAVES;

    chunk->blocks[PosInChunkToIndex(treeX + 2, y + 3, treeZ + 0)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 3, treeZ + 0)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 3, treeZ + 0)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -2, y + 3, treeZ + 0)] =
        BLOCK_LEAVES;

    chunk->blocks[PosInChunkToIndex(treeX + 2, y + 3, treeZ - 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 3, treeZ - 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 0, y + 3, treeZ - 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 3, treeZ - 1)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -2, y + 3, treeZ - 1)] =
        BLOCK_LEAVES;

    chunk->blocks[PosInChunkToIndex(treeX + 2, y + 3, treeZ - 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 1, y + 3, treeZ - 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + 0, y + 3, treeZ - 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -1, y + 3, treeZ - 2)] =
        BLOCK_LEAVES;
    chunk->blocks[PosInChunkToIndex(treeX + -2, y + 3, treeZ - 2)] =
        BLOCK_LEAVES;
  }

  CreateOpaqueMesh(chunk);
  CreateTranslucentMesh(chunk);

  return chunk;
};

void DestroyChunk(struct Chunk** chunk) {
  CALL_GL(glDeleteBuffers(1, &(*chunk)->opaqueMesh.vbo));
  CALL_GL(glDeleteBuffers(1, &(*chunk)->opaqueMesh.ebo));
  CALL_GL(glDeleteBuffers(1, &(*chunk)->translucentMesh.vbo));
  CALL_GL(glDeleteBuffers(1, &(*chunk)->translucentMesh.ebo));
  free(*chunk);
  *chunk = NULL;
}
