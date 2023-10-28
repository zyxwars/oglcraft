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

// enum BlockId {
//   BLOCK_AIR,
//   BLOCK_GRASS,
//   BLOCK_WATER,
//   BLOCK_DIRT,
//   BLOCK_OAK_LOG,
//   BLOCK_LEAVES
// };

// struct Block {
//   // front back top bottom left right
//   ivec2 texCoord[6];
//   // Which buffer the object belongs to
//   int isOpaque;
//   int isTranslucent;
//   // Object is opaque but parts are fully transparent
//   int isCutout;
//   // Whether two same blocks next to each other will render their touching
//   faces
//   // i.e. leaves, chests will, glass won't
//   int isSelfCulled;
// };

// TODO: Read in json
// TODO: create on runtime
const struct Block BlockTypes[] = {
    // air
    // TODO: air defined as a cutout to make things easier
    {{0}, 0, 0, 1, 0},
    // grass
    {{{3, 0}, {3, 0}, {0, 0}, {3, 0}, {3, 0}, {3, 0}}, 1, 0, 0, 1},
    // water
    {{{13, 12}, {13, 12}, {13, 12}, {13, 12}, {13, 12}, {13, 12}}, 0, 1, 0, 1},
    // dirt
    {{{2, 0}, {2, 0}, {2, 0}, {2, 0}, {2, 0}, {2, 0}}, 1, 0, 0, 1},
    // oak log
    {{{4, 1}, {4, 1}, {5, 1}, {4, 1}, {4, 1}, {4, 1}}, 1, 0, 0, 1},
    // leaves
    {{{4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}, {4, 3}}, 1, 0, 1, 0},
};

float GetTexelX(int x) { return (x / AtlasW); };
float GetTexelY(int y) { return (y / AtlasH); };

void AddFaceToBuffer(unsigned int blockId, enum BlockFace blockFace, int worldX,
                     int worldY, int worldZ, int* currentFaceIndex,
                     struct Vertex* vertices, unsigned int* triangles) {
  const struct Block* currentBlockType = &BlockTypes[blockId];

  float uvs[24][2] = {0};

  for (int i = 0; i < 6; i++) {
    //
    uvs[i * 4][0] = GetTexelX(currentBlockType->texCoord[i][0] * blockSize);
    uvs[i * 4][1] =
        GetTexelY(currentBlockType->texCoord[i][1] * blockSize + blockSize);

    //
    uvs[i * 4 + 1][0] = GetTexelX(currentBlockType->texCoord[i][0] * blockSize);
    uvs[i * 4 + 1][1] = GetTexelY(currentBlockType->texCoord[i][1] * blockSize);

    //
    uvs[i * 4 + 2][0] =
        GetTexelX(currentBlockType->texCoord[i][0] * blockSize + blockSize);
    uvs[i * 4 + 2][1] = GetTexelY(currentBlockType->texCoord[i][1] * blockSize);

    //
    uvs[i * 4 + 3][0] =
        GetTexelX(currentBlockType->texCoord[i][0] * blockSize + blockSize);
    uvs[i * 4 + 3][1] =
        GetTexelY(currentBlockType->texCoord[i][1] * blockSize + blockSize);
  }

  const struct Vertex frontFace[] = {
      {blockId, {-0.5f, -0.5f, 0.5f}, {0.f, 0.f, 1.f}, {uvs[0][0], uvs[0][1]}},
      {blockId, {-0.5f, 0.5f, 0.5f}, {0.f, 0.f, 1.f}, {uvs[1][0], uvs[1][1]}},
      {blockId, {0.5f, 0.5f, 0.5f}, {0.f, 0.f, 1.f}, {uvs[2][0], uvs[2][1]}},
      {blockId, {0.5f, -0.5f, 0.5f}, {0.f, 0.f, 1.f}, {uvs[3][0], uvs[3][1]}},
  };
  const struct Vertex backFace[] = {
      {blockId,
       {-0.5f, -0.5f, -0.5f},
       {0.f, 0.f, -1.f},
       {uvs[4][0], uvs[4][1]}},
      {blockId, {-0.5f, 0.5f, -0.5f}, {0.f, 0.f, -1.f}, {uvs[5][0], uvs[5][1]}},
      {blockId, {0.5f, 0.5f, -0.5f}, {0.f, 0.f, -1.f}, {uvs[6][0], uvs[6][1]}},
      {blockId, {0.5f, -0.5f, -0.5f}, {0.f, 0.f, -1.f}, {uvs[7][0], uvs[7][1]}},
  };
  const struct Vertex topFace[] = {
      {blockId, {-0.5f, 0.5f, 0.5f}, {0.f, 1.f, 0.f}, {uvs[8][0], uvs[8][1]}},
      {blockId, {-0.5f, 0.5f, -0.5f}, {0.f, 1.f, 0.f}, {uvs[9][0], uvs[9][1]}},
      {blockId, {0.5f, 0.5f, -0.5f}, {0.f, 1.f, 0.f}, {uvs[10][0], uvs[10][1]}},
      {blockId, {0.5f, 0.5f, 0.5f}, {0.f, 1.f, 0.f}, {uvs[11][0], uvs[11][1]}},
  };
  const struct Vertex bottomFace[] = {
      {blockId,
       {-0.5f, -0.5f, 0.5f},
       {0.f, -1.f, 0.f},
       {uvs[12][0], uvs[12][1]}},
      {blockId,
       {-0.5f, -0.5f, -0.5f},
       {0.f, -1.f, 0.f},
       {uvs[13][0], uvs[13][1]}},
      {blockId,
       {0.5f, -0.5f, -0.5f},
       {0.f, -1.f, 0.f},
       {uvs[14][0], uvs[14][1]}},
      {blockId,
       {0.5f, -0.5f, 0.5f},
       {0.f, -1.f, 0.f},
       {uvs[15][0], uvs[15][1]}},
  };
  const struct Vertex leftFace[] = {
      {blockId,
       {-0.5f, -0.5f, -0.5f},
       {1.f, 0.f, 0.f},
       {uvs[16][0], uvs[16][1]}},
      {blockId,
       {-0.5f, 0.5f, -0.5f},
       {1.f, 0.f, 0.f},
       {uvs[17][0], uvs[17][1]}},
      {blockId, {-0.5f, 0.5f, 0.5f}, {1.f, 0.f, 0.f}, {uvs[18][0], uvs[18][1]}},
      {blockId,
       {-0.5f, -0.5f, 0.5f},
       {1.f, 0.f, 0.f},
       {uvs[19][0], uvs[19][1]}},
  };
  const struct Vertex rightFace[] = {
      {blockId,
       {0.5f, -0.5f, -0.5f},
       {-1.f, 0.f, 0.f},
       {uvs[20][0], uvs[20][1]}},
      {blockId,
       {0.5f, 0.5f, -0.5f},
       {-1.f, 0.f, 0.f},
       {uvs[21][0], uvs[21][1]}},
      {blockId, {0.5f, 0.5f, 0.5f}, {-1.f, 0.f, 0.f}, {uvs[22][0], uvs[22][1]}},
      {blockId,
       {0.5f, -0.5f, 0.5f},
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

void AddToOpaqueBuffer(unsigned int* chunkData, int x, int y, int z, int worldX,
                       int worldZ, int* currentFaceIndex,
                       struct Vertex* vertices, unsigned int* triangles) {
  int blockId = chunkData[PosToIndex(x, y, z)];

  // Only process opaque blocks
  if (!BlockTypes[blockId].isOpaque) {
    return;
  }

  // Neighbors
  const struct Block* frontN =
      z + 1 == CHUNK_LENGTH ? NULL
                            : &BlockTypes[chunkData[PosToIndex(x, y, z + 1)]];
  const struct Block* backN =
      z == 0 ? NULL : &BlockTypes[chunkData[PosToIndex(x, y, z - 1)]];

  const struct Block* topN =
      y + 1 == CHUNK_HEIGHT ? NULL
                            : &BlockTypes[chunkData[PosToIndex(x, y + 1, z)]];
  const struct Block* bottomN =
      y == 0 ? NULL : &BlockTypes[chunkData[PosToIndex(x, y - 1, z)]];

  const struct Block* rightN =
      x + 1 == CHUNK_LENGTH ? NULL
                            : &BlockTypes[chunkData[PosToIndex(x + 1, y, z)]];
  const struct Block* leftN =
      x == 0 ? NULL : &BlockTypes[chunkData[PosToIndex(x - 1, y, z)]];

  // If neihbor is out of range render without checking other conditions
  // TODO: implement self culling for glass
  if (topN == NULL || topN->isCutout || topN->isTranslucent)
    AddFaceToBuffer(blockId, BLOCK_FACE_TOP, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (bottomN == NULL || bottomN->isCutout || bottomN->isTranslucent)
    AddFaceToBuffer(blockId, BLOCK_FACE_BOTTOM, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (rightN == NULL || rightN->isCutout || rightN->isTranslucent)
    AddFaceToBuffer(blockId, BLOCK_FACE_RIGHT, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (leftN == NULL || leftN->isCutout || leftN->isTranslucent)
    AddFaceToBuffer(blockId, BLOCK_FACE_LEFT, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (frontN == NULL || frontN->isCutout || frontN->isTranslucent)
    AddFaceToBuffer(blockId, BLOCK_FACE_FRONT, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (backN == NULL || backN->isCutout || backN->isTranslucent)
    AddFaceToBuffer(blockId, BLOCK_FACE_BACK, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);
}

void AddToTranslucentBuffer(unsigned int* chunkData, int x, int y, int z,
                            int worldX, int worldZ, int* currentFaceIndex,
                            struct Vertex* vertices, unsigned int* triangles) {
  int blockId = chunkData[PosToIndex(x, y, z)];

  // Only process translucent blocks
  if (!BlockTypes[blockId].isTranslucent) {
    return;
  }

  // Neighbors
  const struct Block* frontN =
      z + 1 == CHUNK_LENGTH ? NULL
                            : &BlockTypes[chunkData[PosToIndex(x, y, z + 1)]];
  const struct Block* backN =
      z == 0 ? NULL : &BlockTypes[chunkData[PosToIndex(x, y, z - 1)]];

  const struct Block* topN =
      y + 1 == CHUNK_HEIGHT ? NULL
                            : &BlockTypes[chunkData[PosToIndex(x, y + 1, z)]];
  const struct Block* bottomN =
      y == 0 ? NULL : &BlockTypes[chunkData[PosToIndex(x, y - 1, z)]];

  const struct Block* rightN =
      x + 1 == CHUNK_LENGTH ? NULL
                            : &BlockTypes[chunkData[PosToIndex(x + 1, y, z)]];
  const struct Block* leftN =
      x == 0 ? NULL : &BlockTypes[chunkData[PosToIndex(x - 1, y, z)]];

  // Add only faces touching cutouts
  // Air is a cutout for now as well
  if (topN == NULL || topN->isCutout)
    AddFaceToBuffer(blockId, BLOCK_FACE_TOP, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (bottomN == NULL || bottomN->isCutout)
    AddFaceToBuffer(blockId, BLOCK_FACE_BOTTOM, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (rightN == NULL || rightN->isCutout)
    AddFaceToBuffer(blockId, BLOCK_FACE_RIGHT, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (leftN == NULL || leftN->isCutout)
    AddFaceToBuffer(blockId, BLOCK_FACE_LEFT, worldX, y,

                    worldZ, currentFaceIndex, vertices, triangles);
  if (frontN == NULL || frontN->isCutout)
    AddFaceToBuffer(blockId, BLOCK_FACE_FRONT, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);

  if (backN == NULL || backN->isCutout)
    AddFaceToBuffer(blockId, BLOCK_FACE_BACK, worldX, y, worldZ,
                    currentFaceIndex, vertices, triangles);
}