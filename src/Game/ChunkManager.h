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
  int visibleChunkCount;
  int maxLoadedChunks;
  struct Chunk** loadedChunks;
  struct Chunk** chunksToRender;
  GLuint shader;
};

struct ChunkManager* CreateChunkManager();

void ChunkManagerUpdate(struct ChunkManager* const manager, ivec3 playerPos);

void ChunkManagerDraw(struct ChunkManager* const manager,
                      struct Camera* const camera, float currentTimeS);