#include "HeldItem.h"

struct HeldItemRenderer* CreateHeldItemRenderer(enum BlockId heldBlock) {
  struct HeldItemRenderer* rend = calloc(1, sizeof(struct HeldItemRenderer));

  // TODO:
  char DEBUG_PATH[512] = {0};
  char DEBUG_PATH2[512] = {0};
  _fullpath(DEBUG_PATH, "../../../src/shaders/heldItem.vert", 512);
  _fullpath(DEBUG_PATH2, "../../../src/shaders/heldItem.frag", 512);
  rend->shader = CreateShaderProgram(DEBUG_PATH, DEBUG_PATH2);

  // TODO: free
  CALL_GL(glGenVertexArrays(1, &(rend->mesh.vao)));
  CALL_GL(glBindVertexArray(rend->mesh.vao));

  CALL_GL(glGenBuffers(1, &(rend->mesh.vbo)));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, rend->mesh.vbo));

  CALL_GL(glGenBuffers(1, &(rend->mesh.ebo)));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rend->mesh.ebo));

  int stride = sizeof(struct BlockVertex);

  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glVertexAttribIPointer(0, 1, GL_INT, stride,
                                 (void*)offsetof(struct BlockVertex, blockId)));

  CALL_GL(glEnableVertexAttribArray(1));
  CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct BlockVertex, position)));

  CALL_GL(glEnableVertexAttribArray(2));
  CALL_GL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct BlockVertex, normal)));

  CALL_GL(glEnableVertexAttribArray(3));
  CALL_GL(
      glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride,
                            (void*)offsetof(struct BlockVertex, texCoords)));

  unsigned int blockId = heldBlock;
  int currentFaceIndex = 0;

  struct BlockVertex vertices[4 * 6] = {0};
  unsigned int triangles[6 * 6] = {0};
  AddFaceToBuffer(blockId, BLOCK_FACE_TOP, 0, 0, 0, &currentFaceIndex, vertices,
                  triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_BOTTOM, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_RIGHT, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_LEFT, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_FRONT, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_BACK, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);

  CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(struct BlockVertex) * 4 * 6,
                       vertices, GL_DYNAMIC_DRAW));
  CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * 6,
                       triangles, GL_STATIC_DRAW));

  CALL_GL(glBindVertexArray(0));

  rend->mesh.faceCount = 6;

  return rend;
}

// TODO: do more efficiently
void HeldItemRendererUpdate(struct HeldItemRenderer* const rend,
                            enum BlockId heldBlock) {
  CALL_GL(glBindVertexArray(rend->mesh.vao));
  // TODO: why does the vbo need binding if vao is bound
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, rend->mesh.vbo));

  unsigned int blockId = heldBlock;
  int currentFaceIndex = 0;

  struct BlockVertex vertices[4 * 6] = {0};
  unsigned int triangles[6 * 6] = {0};
  AddFaceToBuffer(blockId, BLOCK_FACE_TOP, 0, 0, 0, &currentFaceIndex, vertices,
                  triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_BOTTOM, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_RIGHT, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_LEFT, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_FRONT, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);
  AddFaceToBuffer(blockId, BLOCK_FACE_BACK, 0, 0, 0, &currentFaceIndex,
                  vertices, triangles);

  // TODO: use buffer subdata
  CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(struct BlockVertex) * 4 * 6,
                       vertices, GL_DYNAMIC_DRAW));

  CALL_GL(glBindVertexArray(0));
}

void HeldItemRendererDraw(struct HeldItemRenderer* const rend,
                          struct Camera* const camera) {
  mat4 heldItemMVP = GLM_MAT4_IDENTITY_INIT;
  vec3 heldItemTranslation = {2, -1, -3.5};
  glm_translate(heldItemMVP, heldItemTranslation);
  glm_mat4_mul(camera->projectionMatrix, heldItemMVP, heldItemMVP);

  CALL_GL(glBindVertexArray(rend->mesh.vao));
  CALL_GL(glUseProgram(rend->shader));
  CALL_GL(glClear(GL_DEPTH_BUFFER_BIT));

  CALL_GL(GLint heldItemMVPUniform =
              glGetUniformLocation(rend->shader, "u_MVP"));
  CALL_GL(glUniformMatrix4fv(heldItemMVPUniform, 1, GL_FALSE, heldItemMVP[0]));

  CALL_GL(glDrawElements(GL_TRIANGLES, rend->mesh.faceCount * 6,
                         GL_UNSIGNED_INT, 0));

  CALL_GL(glBindVertexArray(0))
  CALL_GL(glUseProgram(0));
}

// TODO:
void HeldItemRendererDestroy(struct HeldItemRenderer* rend) {}