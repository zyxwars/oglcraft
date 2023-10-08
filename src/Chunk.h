#pragma once

#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/vec3.h>
#include <cglm/vec2.h>
#include <FastNoiseLite.h>

#include "GlWrapper.h"

// TODO: remove hardcoded width and height
// Get center of texel to avoid bleeding
#define GET_TEXEL_X(x) ((x + 0.5f) / 32.f)

#define GET_TEXEL_Y(y) ((y + 0.5f) / 16.f)

#define CHUNK_LENGTH 16
// how many blocks fit in 1 tall chunk
#define CHUNK_PLANE_AREA CHUNK_LENGTH* CHUNK_LENGTH

struct Vertex {
  vec3 position;
  vec3 color;
  vec3 normal;
  vec2 texCoords;
};

int PosToIndex(int x, int y, int z);

void AddFaceToBuffer(enum BlockFace blockFace, int x, int y, int z,
                     int* currentFaceIndex, struct Vertex* vertices,
                     unsigned int* triangles);

void AddBlockToBuffer(unsigned int* chunkBlocks, int x, int y, int z,
                      int* currentFaceIndex, struct Vertex* vertices,
                      unsigned int* triangles);

unsigned int* CreateChunk(fnl_state* noiseState, int x, int z);

int CreateChunkMesh(unsigned int* chunkData);
