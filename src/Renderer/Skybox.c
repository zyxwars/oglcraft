#include "Skybox.h"

struct Skybox* CreateSkybox() {
  float skyboxVertices[] = {
      // positions
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

  struct Skybox* skybox = calloc(1, sizeof(struct Skybox));

  skybox->shader = CreateShaderProgram(
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/skybox.vert",
      "C:/Users/Zyxwa/Documents/code/oglc/src/shaders/skybox.frag");

  CALL_GL(glGenVertexArrays(1, &(skybox->mesh.vao)));
  CALL_GL(glBindVertexArray(skybox->mesh.vao));

  CALL_GL(glGenBuffers(1, &(skybox->mesh.vbo)));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, skybox->mesh.vbo));
  CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices,
                       GL_STATIC_DRAW));
  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));

  CALL_GL(glBindVertexArray(0));

  return skybox;
}

void DrawSkybox(struct Skybox* skybox, mat4 viewMat, mat4 projectionMat) {
  // The skybox moves with the player
  mat4 viewMatWithoutTranslation = {0};
  viewMatWithoutTranslation[0][0] = viewMat[0][0];
  viewMatWithoutTranslation[0][1] = viewMat[0][1];
  viewMatWithoutTranslation[0][2] = viewMat[0][2];

  viewMatWithoutTranslation[1][0] = viewMat[1][0];
  viewMatWithoutTranslation[1][1] = viewMat[1][1];
  viewMatWithoutTranslation[1][2] = viewMat[1][2];

  viewMatWithoutTranslation[2][0] = viewMat[2][0];
  viewMatWithoutTranslation[2][1] = viewMat[2][1];
  viewMatWithoutTranslation[2][2] = viewMat[2][2];

  viewMatWithoutTranslation[3][3] = 1;

  mat4 mvp = {0};
  glm_mat4_mul(projectionMat, viewMatWithoutTranslation, mvp);

  CALL_GL(glUseProgram(skybox->shader));
  CALL_GL(glBindVertexArray(skybox->mesh.vao));
  CALL_GL(glDepthMask(GL_FALSE));

  // only rotation mvp matrix
  CALL_GL(GLint mvpUniform = glGetUniformLocation(skybox->shader, "u_MVP"));
  CALL_GL(glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, mvp[0]));

  CALL_GL(glDrawArrays(GL_TRIANGLES, 0, 36));

  CALL_GL(glDepthMask(GL_TRUE));
  CALL_GL(glUseProgram(0));
  CALL_GL(glBindVertexArray(0));
}
