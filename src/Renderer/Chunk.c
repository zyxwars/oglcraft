#include "Chunk.h"

void DrawChunkMesh(struct Mesh* mesh) {
  CALL_GL(glBindVertexArray(mesh->vao))

  CALL_GL(
      glDrawElements(GL_TRIANGLES, mesh->faceCount * 6, GL_UNSIGNED_INT, 0));

  CALL_GL(glBindVertexArray(0))
}

void ChunkRendererDraw(struct Chunk** chunks, int chunkCount, ivec2 chunkPos,
                       int renderDistance, GLuint shader, float currentTimeS,
                       struct Camera* camera) {
  int minX = chunkPos[0] - renderDistance;
  int minZ = chunkPos[1] - renderDistance;
  int maxX = chunkPos[0] + renderDistance;
  int maxZ = chunkPos[1] + renderDistance;

  // model matrix is not used since chunks are static
  // use the same mvp for all chunks
  mat4 mvp = {0};
  glm_mat4_mul(camera->projectionMatrix, camera->viewMatrix, mvp);

  CALL_GL(glUseProgram(shader));

  CALL_GL(GLint mvpUniform = glGetUniformLocation(shader, "u_MVP"));
  CALL_GL(glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, mvp[0]));
  CALL_GL(GLint timeUniform = glGetUniformLocation(shader, "u_TimeS"));
  CALL_GL(glUniform1f(timeUniform, currentTimeS));

  for (int i = 0; i < chunkCount; i++) {
    struct Chunk* curr = chunks[i];
    if (curr == NULL) continue;

    int shouldRender =
        curr->x > minX && curr->z > minZ && curr->x < maxX && curr->z < maxZ;
    if (!shouldRender) continue;

    DrawChunkMesh(&(curr->opaqueMesh));
  }

  CALL_GL(glEnable(GL_BLEND));
  CALL_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  for (int i = 0; i < chunkCount; i++) {
    struct Chunk* curr = chunks[i];
    if (curr == NULL) continue;

    int shouldRender =
        curr->x > minX && curr->z > minZ && curr->x < maxX && curr->z < maxZ;
    if (!shouldRender) continue;

    DrawChunkMesh(&(curr->translucentMesh));
  }

  CALL_GL(glDisable(GL_BLEND));
  CALL_GL(glUseProgram(0));
}