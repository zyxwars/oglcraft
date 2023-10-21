#include "Block.h"

enum BlockFace {
  BLOCK_FACE_FRONT,
  BLOCK_FACE_BACK,
  BLOCK_FACE_TOP,
  BLOCK_FACE_BOTTOM,
  BLOCK_FACE_LEFT,
  BLOCK_FACE_RIGHT,
};

void AddFaceToBuffer(enum BlockFace blockFace, int worldX, int worldY,
                     int worldZ, int* currentFaceIndex, struct Vertex* vertices,
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
    currentVertexStart[i].position[0] += worldX;
    currentVertexStart[i].position[1] += worldY;
    currentVertexStart[i].position[2] += worldZ;
  }

  (*currentFaceIndex)++;
}

void AddBlockToBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                      int worldZ, int* currentFaceIndex,
                      struct Vertex* vertices, unsigned int* triangles) {
  // Check block type
  // Don't draw anything if block is air
  if (chunkData[PosToIndex(x, y, z)] == 0) {
    return;
  }
  // TODO: create visible faces
  // Left expression should be evaluated first and skip all others to avoid
  // index out of range
  if (y + 1 == CHUNK_HEIGHT || chunkData[PosToIndex(x, y + 1, z)] == 0) {
    // Adds face to the buffer and incremenets faceIndex
    AddFaceToBuffer(BLOCK_FACE_TOP, worldX, y, worldZ, currentFaceIndex,
                    vertices, triangles);
  }
  if (y - 1 == -1 || chunkData[PosToIndex(x, y - 1, z)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_BOTTOM, worldX, y, worldZ, currentFaceIndex,
                    vertices, triangles);
  }
  if (x + 1 == CHUNK_LENGTH || chunkData[PosToIndex(x + 1, y, z)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_RIGHT, worldX, y, worldZ, currentFaceIndex,
                    vertices, triangles);
  }
  if (x - 1 == -1 || chunkData[PosToIndex(x - 1, y, z)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_LEFT, worldX, y, worldZ, currentFaceIndex,
                    vertices, triangles);
  }
  if (z + 1 == CHUNK_LENGTH || chunkData[PosToIndex(x, y, z + 1)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_FRONT, worldX, y, worldZ, currentFaceIndex,
                    vertices, triangles);
  }
  if (z - 1 == -1 || chunkData[PosToIndex(x, y, z - 1)] == 0) {
    AddFaceToBuffer(BLOCK_FACE_BACK, worldX, y, worldZ, currentFaceIndex,
                    vertices, triangles);
  }
}