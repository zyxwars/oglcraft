#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GlWrapper.h"
#include "Terrain/Block.h"

struct Mesh* CreateHeldItem();

void DrawHeldItem(struct Mesh* mesh);