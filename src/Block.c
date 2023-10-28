#include "Block.h"

enum BlockFace {
  BLOCK_FACE_FRONT,
  BLOCK_FACE_BACK,
  BLOCK_FACE_TOP,
  BLOCK_FACE_BOTTOM,
  BLOCK_FACE_LEFT,
  BLOCK_FACE_RIGHT,
};

const int blockSize = 16;

const float AtlasW = 256.f;
const float AtlasH = 256.f;

// TODO: Read in json
// TODO: create on runtime
const struct Block BlockTypes[] = {
    {0},
    {{{3, 0}, {3, 0}, {0, 0}, {3, 0}, {3, 0}, {3, 0}}, 1, 0},
    {{{13, 12}, {13, 12}, {13, 12}, {13, 12}, {13, 12}, {13, 12}}, 0, 1},
    {{{2, 0}, {2, 0}, {2, 0}, {2, 0}, {2, 0}, {2, 0}}, 1, 0},
    {{{4, 1}, {4, 1}, {5, 1}, {4, 1}, {4, 1}, {4, 1}}, 1, 0},
    {{{4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}}, 0, 1},
};

// TODO: remove hardcoded width and height
// Get center of texel to avoid bleeding
float GetTexelX(int x) { return ((x + 0.5f) / AtlasW); };
float GetTexelY(int y) { return ((y + 0.5f) / AtlasH); };

void AddFaceToBuffer(unsigned int blockId, enum BlockFace blockFace, int worldX,
                     int worldY, int worldZ, int* currentFaceIndex,
                     struct Vertex* vertices, unsigned int* triangles) {
  const struct Block* currentBlockType = &BlockTypes[blockId];

  float uvs[24][2] = {0};

  for (int i = 0; i < 6; i++) {
    //
    uvs[i * 4][0] = GetTexelX(currentBlockType->texCoord[i][0] * 16);
    uvs[i * 4][1] = GetTexelY(currentBlockType->texCoord[i][1] * 16 + 15);

    //
    uvs[i * 4 + 1][0] = GetTexelX(currentBlockType->texCoord[i][0] * 16);
    uvs[i * 4 + 1][1] = GetTexelY(currentBlockType->texCoord[i][1] * 16);

    //
    uvs[i * 4 + 2][0] = GetTexelX(currentBlockType->texCoord[i][0] * 16 + 15);
    uvs[i * 4 + 2][1] = GetTexelY(currentBlockType->texCoord[i][1] * 16);

    //
    uvs[i * 4 + 3][0] = GetTexelX(currentBlockType->texCoord[i][0] * 16 + 15);
    uvs[i * 4 + 3][1] = GetTexelY(currentBlockType->texCoord[i][1] * 16 + 15);
  }

  const struct Vertex frontFace[] = {
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {uvs[0][0], uvs[0][1]}},
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {uvs[1][0], uvs[1][1]}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {uvs[2][0], uvs[2][1]}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, 1.f},
       {uvs[3][0], uvs[3][1]}},
  };
  const struct Vertex backFace[] = {
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {uvs[4][0], uvs[4][1]}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {uvs[5][0], uvs[5][1]}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {uvs[6][0], uvs[6][1]}},
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 0.f, -1.f},
       {uvs[7][0], uvs[7][1]}},
  };
  const struct Vertex topFace[] = {
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {uvs[8][0], uvs[8][1]}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {uvs[9][0], uvs[9][1]}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {uvs[10][0], uvs[10][1]}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, 1.f, 0.f},
       {uvs[11][0], uvs[11][1]}},

  };
  const struct Vertex bottomFace[] = {
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {uvs[12][0], uvs[12][1]}},
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {uvs[13][0], uvs[13][1]}},
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {uvs[14][0], uvs[14][1]}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {0.f, -1.f, 0.f},
       {uvs[15][0], uvs[15][1]}},
  };
  const struct Vertex leftFace[] = {
      {{-0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {uvs[16][0], uvs[16][1]}},
      {{-0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {uvs[17][0], uvs[17][1]}},
      {{-0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {uvs[18][0], uvs[18][1]}},
      {{-0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {1.f, 0.f, 0.f},
       {uvs[19][0], uvs[19][1]}},
  };
  const struct Vertex rightFace[] = {
      {{0.5f, -0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {uvs[20][0], uvs[20][1]}},
      {{0.5f, 0.5f, -0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {uvs[21][0], uvs[21][1]}},
      {{0.5f, 0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {uvs[22][0], uvs[22][1]}},
      {{0.5f, -0.5f, 0.5f},
       {0.f, 1.f, 0.f},
       {-1.f, 0.f, 0.f},
       {uvs[23][0], uvs[23][1]}},
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

void AddToBlockBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                      int worldZ, int* currentFaceIndex,
                      struct Vertex* vertices, unsigned int* triangles) {
  // Check block type
  if (!BlockTypes[chunkData[PosToIndex(x, y, z)]].isOpaque) {
    return;
  }

  // TODO: create visible faces
  // Left expression should be evaluated first and skip all others to avoid
  // index out of range
  // TODO: fix check != 1, since now only water, air and solid exist
  if (y + 1 == CHUNK_HEIGHT ||
      !BlockTypes[chunkData[PosToIndex(x, y + 1, z)]].isOpaque) {
    // Adds face to the buffer and incremenets faceIndex
    AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_TOP, worldX, y,
                    worldZ, currentFaceIndex, vertices, triangles);
  }
  if (y - 1 == -1 || !BlockTypes[chunkData[PosToIndex(x, y - 1, z)]].isOpaque) {
    AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_BOTTOM, worldX,
                    y, worldZ, currentFaceIndex, vertices, triangles);
  }
  if (x + 1 == CHUNK_LENGTH ||
      !BlockTypes[chunkData[PosToIndex(x + 1, y, z)]].isOpaque) {
    AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_RIGHT, worldX, y,
                    worldZ, currentFaceIndex, vertices, triangles);
  }
  if (x - 1 == -1 || !BlockTypes[chunkData[PosToIndex(x - 1, y, z)]].isOpaque) {
    AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_LEFT, worldX, y,
                    worldZ, currentFaceIndex, vertices, triangles);
  }
  if (z + 1 == CHUNK_LENGTH ||
      !BlockTypes[chunkData[PosToIndex(x, y, z + 1)]].isOpaque) {
    AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_FRONT, worldX, y,
                    worldZ, currentFaceIndex, vertices, triangles);
  }
  if (z - 1 == -1 || !BlockTypes[chunkData[PosToIndex(x, y, z - 1)]].isOpaque) {
    AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_BACK, worldX, y,
                    worldZ, currentFaceIndex, vertices, triangles);
  }
}

void AddToWaterBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                      int worldZ, int* currentFaceIndex,
                      struct Vertex* vertices, unsigned int* triangles) {
  // Check block type
  if (!BlockTypes[chunkData[PosToIndex(x, y, z)]].isTranslucent) {
    return;
  }
  // TODO: create visible faces
  // Left expression should be evaluated first and skip all others to avoid
  // index out of range
  // TODO: fix check != 1, since now only water, air and solid exist

  // if (y + 1 == CHUNK_HEIGHT ||
  //     !BlockTypes[chunkData[PosToIndex(x, y + 1, z)]].isTranslucent) {
  //   // Adds face to the buffer and incremenets faceIndex
  AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_TOP, worldX, y,
                  worldZ, currentFaceIndex, vertices, triangles);
  // }
  // if (y - 1 == -1 ||
  //     !BlockTypes[chunkData[PosToIndex(x, y - 1, z)]].isTranslucent) {
  AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_BOTTOM, worldX, y,
                  worldZ, currentFaceIndex, vertices, triangles);
  // }
  // if (x + 1 == CHUNK_LENGTH ||
  //     !BlockTypes[chunkData[PosToIndex(x + 1, y, z)]].isTranslucent) {
  AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_RIGHT, worldX, y,
                  worldZ, currentFaceIndex, vertices, triangles);
  // }
  // if (x - 1 == -1 ||
  //     !BlockTypes[chunkData[PosToIndex(x - 1, y, z)]].isTranslucent) {
  AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_LEFT, worldX, y,
                  worldZ, currentFaceIndex, vertices, triangles);
  // }
  // if (z + 1 == CHUNK_LENGTH ||
  //     !BlockTypes[chunkData[PosToIndex(x, y, z + 1)]].isTranslucent) {
  AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_FRONT, worldX, y,
                  worldZ, currentFaceIndex, vertices, triangles);
  // }
  // if (z - 1 == -1 ||
  //     !BlockTypes[chunkData[PosToIndex(x, y, z - 1)]].isTranslucent) {
  AddFaceToBuffer(chunkData[PosToIndex(x, y, z)], BLOCK_FACE_BACK, worldX, y,
                  worldZ, currentFaceIndex, vertices, triangles);
  // }
}