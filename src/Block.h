#pragma once

#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/vec3.h>
#include <cglm/vec2.h>
#include <FastNoiseLite.h>

#include "GlWrapper.h"
#include "Chunk.h"

enum BlockId {
  BLOCK_AIR,
  BLOCK_GRASS,
  BLOCK_WATER,
  BLOCK_DIRT,
  BLOCK_OAK_LOG,
  BLOCK_LEAVES
};

struct Block {
  // front back top bottom left right
  ivec2 texCoord[6];
  int isOpaque;
  int isTranslucent;
};

struct Vertex {
  vec3 position;
  vec3 color;
  vec3 normal;
  vec2 texCoords;
};

int IsOpaque(unsigned int blockId);
int IsWater(unsigned int blockId);

void AddToBlockBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                      int worldZ, int* currentFaceIndex,
                      struct Vertex* vertices, unsigned int* triangles);

void AddToWaterBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                      int worldZ, int* currentFaceIndex,
                      struct Vertex* vertices, unsigned int* triangles);