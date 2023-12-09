#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/mat4.h>

#include "GlWrapper.h"
#include "Shader.h"
#include "Camera.h"

struct SelectionRenderer {
  struct Mesh mesh;
  GLuint shader;
};

struct SelectionRenderer* CreateSelectionRenderer();

void SelectionRendererDraw(struct SelectionRenderer* const rend,
                           ivec3 selectionPos, struct Camera* camera);

void SelectionRendererDestroy(struct SelectionRenderer* rend);
