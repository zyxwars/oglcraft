#pragma once

#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define LOG(label, msg) LOG_REMOTE(label, msg, __FILE__, __LINE__)

#define LOG_REMOTE(label, msg, file, line) \
  printf("[%s] %s in %s:%d\n", label, msg, file, line);

// TODO: dissable error log calls in release
#define CALL_GL(x) \
  x;               \
  LogGLError(__FILE__, __LINE__);

int LogGLError(const char* file, int line);

struct Mesh {
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  int faceCount;
};