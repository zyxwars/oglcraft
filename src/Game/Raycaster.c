#include "Raycaster.h"

int CastRay(struct ChunkManager* chunkManager, vec3 startPos, vec3 dir,
            ivec3* outHitPos, ivec3* outBeforeHitPos) {
  float rayDistance = 10.f;
  float step = 0.1f;
  float currentStep = 0.f;

  while (currentStep < rayDistance) {
    vec3 hitPos;
    glm_vec3_scale(dir, currentStep, hitPos);
    glm_vec3_add(hitPos, startPos, hitPos);
    currentStep += step;

    ivec3 hitPosRounded = {
        (int)round(hitPos[0]),
        (int)round(hitPos[1]),
        (int)round(hitPos[2]),
    };

    if (hitPosRounded[1] >= CHUNK_HEIGHT) {
      glm_ivec3_zero(*outHitPos);
      glm_ivec3_zero(*outBeforeHitPos);
      return 0;
    }

    ivec2 chunkPos;
    PosToChunkPos(hitPosRounded[0], hitPosRounded[2], &chunkPos);

    struct Chunk* chunk = GetChunk(chunkManager, chunkPos);

    // stop raycast if the chunk isn't yet laoded
    if (chunk == NULL) return 0;

    ivec3 posInChunk;
    PosToPosInChunk(hitPosRounded, &posInChunk);
    enum BlockId block = chunk->blocks[PosInChunkToIndex(
        posInChunk[0], posInChunk[1], posInChunk[2])];

    if (block == BLOCK_AIR) {
      glm_ivec3_copy(hitPosRounded, *outBeforeHitPos);
      continue;
    }

    printf("%d %d %d %d %d [%d %d %d]\n", hitPosRounded[0], hitPosRounded[1],
           hitPosRounded[2], chunkPos[0], chunkPos[1], posInChunk[0],
           posInChunk[1], posInChunk[2]);

    glm_ivec3_copy(hitPosRounded, *outHitPos);
    return 1;
  }

  return 0;
}