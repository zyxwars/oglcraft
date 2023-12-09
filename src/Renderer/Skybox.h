#pragma once

#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <stdio.h>
#include <stdlib.h>

#include "GlWrapper.h"
#include "Shader.h"

struct Skybox {
  struct Mesh mesh;
  GLuint shader;
};

struct Skybox* CreateSkyboxRenderer();

void SkyboxRendererDraw(struct Skybox* skybox, mat4 viewMat, mat4 projectionMat);
