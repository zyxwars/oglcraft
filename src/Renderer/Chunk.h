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

void ChunkRendererDraw(struct Chunk** chunks, int chunkCount, ivec2 chunkPos,
                       int renderDistance, GLuint shader, float currentTimeS,
                       struct Camera* camera);