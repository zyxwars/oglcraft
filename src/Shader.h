#pragma once

#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GlWrapper.h"

void FileToString(const char* path, char** buffer);

int CompileShader(GLenum type, const char* shaderSource);

int CreateShaderProgram(const char* vertexSourcePath,
                        const char* fragmentSourcePath);