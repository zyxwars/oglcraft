#include "Block.h"

enum BlockFace {
  BLOCK_FACE_FRONT,
  BLOCK_FACE_BACK,
  BLOCK_FACE_TOP,
  BLOCK_FACE_BOTTOM,
  BLOCK_FACE_LEFT,
  BLOCK_FACE_RIGHT,
};

int PosToIndex(int x, int y, int z) {
  return x + z * CHUNK_LENGTH + y * CHUNK_PLANE_AREA;
}

void AddFaceToBuffer(enum BlockFace blockFace, int x, int y, int z,
                     int* currentFaceIndex, struct Vertex* vertices,
                     unsigned int* triangles) {
  const static struct Vertex frontFace[] = {
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };
  const static struct Vertex backFace[] = {
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };
  const static struct Vertex topFace[] = {
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {GET_TEXEL_X(0), GET_TEXEL_Y(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {GET_TEXEL_X(0), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {GET_TEXEL_X(15), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {GET_TEXEL_X(15), GET_TEXEL_Y(15)}},

  };
  const static struct Vertex bottomFace[] = {
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };
  const static struct Vertex leftFace[] = {
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };
  const static struct Vertex rightFace[] = {
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(15)}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {GET_TEXEL_X(16), GET_TEXEL_Y(0)}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(0)}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {GET_TEXEL_X(31), GET_TEXEL_Y(15)}},
  };

  const struct Vertex* pFaceToCopy = NULL;

  int currentVertexIndex = (*currentFaceIndex) * 4;
  int currentTriangleIndex = (*currentFaceIndex) * 6;

  struct Vertex* currentVertexStart = vertices + currentVertexIndex;

  // TODO: create triangles more cleanly
  switch (blockFace) {
    case BLOCK_FACE_FRONT:
      pFaceToCopy = frontFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 3 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 2 + currentVertexIndex;
      break;
    case BLOCK_FACE_BACK:
      pFaceToCopy = backFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 3 + currentVertexIndex;
      break;
    case BLOCK_FACE_TOP:
      pFaceToCopy = topFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 3 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 2 + currentVertexIndex;
      break;
    case BLOCK_FACE_BOTTOM:
      pFaceToCopy = bottomFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 3 + currentVertexIndex;
      break;
    case BLOCK_FACE_LEFT:
      pFaceToCopy = leftFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 3 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 2 + currentVertexIndex;
      break;
    case BLOCK_FACE_RIGHT:
      pFaceToCopy = rightFace;
      // Define triangle indices for the face
      triangles[currentTriangleIndex] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 1] = 1 + currentVertexIndex;
      triangles[currentTriangleIndex + 2] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 3] = 0 + currentVertexIndex;
      triangles[currentTriangleIndex + 4] = 2 + currentVertexIndex;
      triangles[currentTriangleIndex + 5] = 3 + currentVertexIndex;
      break;
  }

  // TODO:
  if (pFaceToCopy == NULL) {
    __debugbreak();
  }
  // Copy the 4 face vertices into the vertex buffer
  memcpy(currentVertexStart, pFaceToCopy, 4 * sizeof(struct Vertex));

  // Set face position based on cube position
  for (int i = 0; i < 4; i++) {
    // TODO: multiply by chunk as well
    currentVertexStart[i].position[0] += x;
    currentVertexStart[i].position[1] += y;
    currentVertexStart[i].position[2] += z;
  }

  (*currentFaceIndex)++;
}

void AddBlockToBuffer(unsigned int* chunkData, int x, int y, int z,
                      int* currentFaceIndex, struct Vertex* vertices,
                      unsigned int* triangles) {
  // Check block type
  // Don't draw anything if block is air
  if (chunkData[PosToIndex(x, y, z)] == 0) {
    return;
  }
  // TODO: create visible faces
  // Left expression should be evaluated first and skip all others to avoid
  // index out of range
  if (y + 1 == CHUNK_LENGTH || chunkData[PosToIndex(x, y + 1, z)] == 0) {
    // Adds face to the buffer and incremenets faceIndex
    AddFaceToBuffer(BLOCK_FACE_TOP, x, y, z, currentFaceIndex, vertices,
                    triangles);
  }
  if (y - 1 == -1 || chunkData[PosToIndex(x, y - 1, z)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_BOTTOM, x, y, z, currentFaceIndex, vertices,
                    triangles);
  }
  if (x + 1 == CHUNK_LENGTH || chunkData[PosToIndex(x + 1, y, z)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_RIGHT, x, y, z, currentFaceIndex, vertices,
                    triangles);
  }
  if (x - 1 == -1 || chunkData[PosToIndex(x - 1, y, z)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_LEFT, x, y, z, currentFaceIndex, vertices,
                    triangles);
  }
  if (z + 1 == CHUNK_LENGTH || chunkData[PosToIndex(x, y, z + 1)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_FRONT, x, y, z, currentFaceIndex, vertices,
                    triangles);
  }
  if (z - 1 == -1 || chunkData[PosToIndex(x, y, z - 1)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_BACK, x, y, z, currentFaceIndex, vertices,
                    triangles);
  }
}

unsigned int* CreateChunk(fnl_state* noiseState, int x, int z) {
  // TODO: free on chunk destroy
  unsigned int* chunkData = calloc(pow(CHUNK_LENGTH, 3), sizeof(unsigned int));

  // Populate chunk
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int z = 0; z < CHUNK_LENGTH; z++) {
      // TODO: proper block types
      // TODO: multiply by chunk coords
      int height = fnlGetNoise2D(noiseState, x, z) * CHUNK_LENGTH;
      // Fill every block under height
      for (int y = 0; y < height; y++) {
        chunkData[PosToIndex(x, y, z)] = 1;
      }
    }
  }

  return chunkData;
};

// TODO: create update function
// TODO: create struct to store vbo, ebo to bind when rendering each chunk
int CreateChunkMesh(unsigned int* chunkData) {
  // TODO: Destroy buffer on chunk destroy
  GLuint vbo;
  CALL_GL(glGenBuffers(1, &vbo));
  CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));

  int stride = sizeof(struct Vertex);

  CALL_GL(glEnableVertexAttribArray(0));
  CALL_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct Vertex, position)));

  CALL_GL(glEnableVertexAttribArray(1));
  CALL_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct Vertex, color)));

  CALL_GL(glEnableVertexAttribArray(2));
  CALL_GL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct Vertex, normal)));

  CALL_GL(glEnableVertexAttribArray(3));
  CALL_GL(glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride,
                                (void*)offsetof(struct Vertex, texCoords)));

  GLuint ebo;
  CALL_GL(glGenBuffers(1, &ebo));
  CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

  // TODO: allocate in a less wasteful way?
  const int maxFacesInChunk = (pow(CHUNK_LENGTH, 3) * 6);

  struct Vertex* vertices = malloc(maxFacesInChunk * 4 * sizeof(struct Vertex));
  unsigned int* triangles = malloc(maxFacesInChunk * 6 * sizeof(unsigned int));

  int currentFaceIndex = 0;
  for (int x = 0; x < CHUNK_LENGTH; x++) {
    for (int y = 0; y < CHUNK_LENGTH; y++) {
      for (int z = 0; z < CHUNK_LENGTH; z++) {
        AddBlockToBuffer(chunkData, x, y, z, &currentFaceIndex, vertices,
                         triangles);
      }
    }
  }

  // We copy only the existing vertices based on face index
  // ignoring the rest of data that is garbage, allocated for the worst case
  // when culling faces
  CALL_GL(glBufferData(GL_ARRAY_BUFFER,
                       sizeof(struct Vertex) * 4 * currentFaceIndex, vertices,
                       GL_STATIC_DRAW));
  free(vertices);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(unsigned int) * 6 * currentFaceIndex, triangles,
               GL_STATIC_DRAW);
  free(triangles);

  return currentFaceIndex;
}