#include "Selection.h"

struct SelectionRenderer* CreateSelectionRenderer() {
  static float selectionVertices[] = {
      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
      1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f};

  struct SelectionRenderer* rend = calloc(1, sizeof(struct SelectionRenderer));

  rend->shader = CreateShaderProgram(
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/selection.vert",
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/selection.frag");

  CALL_GL(glGenVertexArrays(1, &(rend->mesh.vao)));
  CALL_GL(glBindVertexArray(rend->mesh.vao));

  CALL_GL(glGenBuffers(1, &(rend->mesh.vbo)));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, rend->mesh.vbo));
  CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(selectionVertices),
                       selectionVertices, GL_STATIC_DRAW));
  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));

  CALL_GL(glBindVertexArray(0));

  return rend;
}

void SelectionRendererDraw(struct SelectionRenderer* const rend,
                           ivec3 selectionPos, struct Camera* camera) {
  CALL_GL(glUseProgram(rend->shader));
  CALL_GL(glBindVertexArray(rend->mesh.vao));
  CALL_GL(glEnable(GL_BLEND));
  CALL_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  mat4 mvp;
  mat4 selectionModelMat = GLM_MAT4_IDENTITY_INIT;
  // avoid z fighting with actual block, also the slightly larger selection
  // effect is cool
  glm_mat4_scale(selectionModelMat, 0.505f);
  selectionModelMat[3][0] = (float)selectionPos[0];
  selectionModelMat[3][1] = (float)selectionPos[1];
  selectionModelMat[3][2] = (float)selectionPos[2];
  selectionModelMat[3][3] = 1;
  // TODO: this doesn't work in a function if passing mat4 as param, use mul
  // instead of mulN
  glm_mat4_mulN((mat4*[]){&(camera->projectionMatrix), &(camera->viewMatrix),
                          &selectionModelMat},
                3, mvp);

  CALL_GL(GLint mvpUniform = glGetUniformLocation(rend->shader, "u_MVP"));
  CALL_GL(glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, mvp[0]));

  CALL_GL(glDrawArrays(GL_TRIANGLES, 0, 36));

  CALL_GL(glDisable(GL_BLEND));
  CALL_GL(glBindVertexArray(0));
  CALL_GL(glUseProgram(0));
}

// TODO:
void SelectionRendererDestroy(struct SelectionRenderer* rend) {}