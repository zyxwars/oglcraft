#pragma once

#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>
#include <cglm/vec2.h>
#include <FastNoiseLite.h>

#include "Renderer/GlWrapper.h"
#include "Terrain/Block.h"
#include "Terrain/Chunk.h"
#include "Camera.h"
#include "Renderer/Shader.h"

struct ChunkManager {
  struct GenerationNoise noise;
  int renderDistance;
  int activeChunksCount;
  struct Chunk** activeChunks;
};

struct ChunkManager* CreateChunkManager();

void ChunkManagerUpdate(struct ChunkManager* manager, ivec2 playerChunkPos);

// Returns null if chunks isn't loaded
struct Chunk* GetChunk(struct ChunkManager* manager, ivec2 chunkPos);
