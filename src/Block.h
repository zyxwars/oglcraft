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

// TODO: remove hardcoded width and height
// Get center of texel to avoid bleeding
#define GET_TEXEL_X(x) ((x + 0.5f) / 32.f)

#define GET_TEXEL_Y(y) ((y + 0.5f) / 16.f)

struct Vertex {
  vec3 position;
  vec3 color;
  vec3 normal;
  vec2 texCoords;
};

void AddBlockToBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                      int worldZ, int* currentFaceIndex,
                      struct Vertex* vertices, unsigned int* triangles);