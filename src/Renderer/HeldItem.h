#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/mat4.h>

#include "GlWrapper.h"
#include "Shader.h"
#include "../Terrain/Block.h"
#include "../Camera.h"

struct HeldItemRenderer {
  struct Mesh mesh;
  GLuint shader;
};

void HeldItemRendererInit(struct HeldItemRenderer* const rend,
                          enumb BlockId heldBlock);

void HeldItemRendererUpdate(struct HeldItemRenderer* const rend,
                            enum BlockId heldBlock);

void HeldItemRendererDraw(struct HeldItemRenderer* const rend,
                          struct Camera* const camera);

void HeldItemRendererDestroy(struct HeldItemRenderer* const rend);