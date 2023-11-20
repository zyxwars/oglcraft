#pragma once

#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/vec3.h>
#include <cglm/vec2.h>
#include <FastNoiseLite.h>

#include "Renderer/GlWrapper.h"
#include "Block.h"

#define CHUNK_LENGTH 16
#define CHUNK_HEIGHT 32
// how many blocks fit in 1 tall chunk
#define CHUNK_PLANE_AREA (CHUNK_LENGTH * CHUNK_LENGTH)
#define CHUNK_VOLUME (CHUNK_PLANE_AREA * CHUNK_HEIGHT)

struct GenerationNoise {
  fnl_state continentalness;
  fnl_state temperature;
  fnl_state humidity;
};

struct Mesh {
  GLuint vbo;
  GLuint ebo;
  int faceCount;
};

struct Chunk {
  int x, z;
  unsigned int blocks[CHUNK_VOLUME];
  struct Mesh opaqueMesh;
  struct Mesh translucentMesh;
};

// TODO: move to utils or something more general
int PosToIndex(int x, int y, int z);

struct Chunk* CreateChunk(struct GenerationNoise* noiseState, int x, int z);

void DestroyChunk(struct Chunk** chunk);

void DrawOpaque(struct Chunk* chunk);

void DrawTranslucent(struct Chunk* chunk);

struct Chunk* GetChunk(int x, int z, struct Chunk** loadedChunks,
                       int loadedChunksSize, struct GenerationNoise* noise);

void UnloadChunks(int minChunkX, int minChunkZ, int maxChunkX, int maxChunkZ,
                  struct Chunk** loadedChunks, int loadedChunksSize);