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

struct Vertex {
  vec3 position;
  vec3 color;
  vec3 normal;
  vec2 texCoords;
};

void AddBlockToBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                      int worldZ, int* currentFaceIndex,
                      struct Vertex* vertices, unsigned int* triangles);