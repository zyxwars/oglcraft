#pragma once

#include <cglm/vec3.h>
#include <cglm/vec2.h>
#include <cglm/ivec3.h>

#include "Game/ChunkManager.h"
#include "Terrain/Block.h"

int CastRay(struct ChunkManager* chunkManager, vec3 startPos, vec3 dir,
            ivec3* outHitPos, ivec3* outBeforeHitPos);
