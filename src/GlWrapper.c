#include "GlWrapper.h"

int LogGLError(const char* file, int line) {
  GLenum e;
  while (e = glGetError()) {
    LOG_REMOTE("OpenGL Error", gluErrorString(e), file, line);
    return 1;
  }
  return 0;
}