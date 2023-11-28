#pragma once

#include <string.h>

#include <cglm/vec3.h>
#include <cglm/vec2.h>

#include "Chunk.h"

enum BlockId {
  BLOCK_AIR,
  BLOCK_GRASS,
  BLOCK_WATER,
  BLOCK_DIRT,
  BLOCK_OAK_LOG,
  BLOCK_LEAVES,
  BLOCK_STONE,
  BLOCK_SAND,
  BLOCK_GRAVEL
};

struct Block {
  // front back top bottom left right
  ivec2 texCoord[6];
  // Which buffer the object belongs to
  int isOpaque;
  int isTranslucent;
  // Object is opaque but parts are fully transparent
  int isCutout;
  // Whether two same blocks next to each other will render their touching faces
  // i.e. leaves, chests will, glass won't
  // basic blocks like dirt, stone won't as well -> isSelfCulled = true
  int isSelfCulled;
};

struct Vertex {
  int blockId;
  vec3 position;
  vec3 normal;
  vec2 texCoords;
};

void AddToOpaqueBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                       int worldZ, int* currentFaceIndex,
                       struct Vertex* vertices, unsigned int* triangles);

void AddToTranslucentBuffer(unsigned int* chunkData, int x, int y, int z,
                            int worldX, int worldZ, int* currentFaceIndex,
                            struct Vertex* vertices, unsigned int* triangles);