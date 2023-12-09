#include "HeldItem.h"

struct Mesh* CreateHeldItem() {
  // TODO: free
  struct Mesh* mesh = calloc(1, sizeof(struct Mesh));

  CALL_GL(glGenVertexArrays(1, &(mesh->vao)));
  CALL_GL(glBindVertexArray(mesh->vao));

  CALL_GL(glGenBuffers(1, &(mesh->vbo)));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo));

  CALL_GL(glGenBuffers(1, &(mesh->ebo)));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo));

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

  unsigned int blockId = BLOCK_STONE;
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
                       vertices, GL_STATIC_DRAW));
  CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * 6,
                       triangles, GL_STATIC_DRAW));

  CALL_GL(glBindVertexArray(0));

  mesh->faceCount = 6;

  return mesh;
}

void DrawHeldItem(struct Mesh* mesh) {
  CALL_GL(glBindVertexArray(mesh->vao))

  CALL_GL(
      glDrawElements(GL_TRIANGLES, mesh->faceCount * 6, GL_UNSIGNED_INT, 0));

  CALL_GL(glBindVertexArray(0))
}