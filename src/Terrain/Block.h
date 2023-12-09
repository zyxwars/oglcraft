#pragma once

#include <string.h>

#include <cglm/vec3.h>
#include <cglm/vec2.h>

#include "Chunk.h"

enum BlockFace {
  BLOCK_FACE_FRONT,
  BLOCK_FACE_BACK,
  BLOCK_FACE_TOP,
  BLOCK_FACE_BOTTOM,
  BLOCK_FACE_LEFT,
  BLOCK_FACE_RIGHT,
};

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

struct BlockVertex {
  int blockId;
  vec3 position;
  vec3 normal;
  vec2 texCoords;
};

void AddFaceToBuffer(unsigned int blockId, enum BlockFace blockFace, int worldX,
                     int worldY, int worldZ, int* currentFaceIndex,
                     struct BlockVertex* vertices, unsigned int* triangles);

void AddToOpaqueBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                       int worldZ, int* currentFaceIndex,
                       struct BlockVertex* vertices, unsigned int* triangles);

void AddToTranslucentBuffer(unsigned int* chunkData, int x, int y, int z,
                            int worldX, int worldZ, int* currentFaceIndex,
                            struct BlockVertex* vertices,
                            unsigned int* triangles);