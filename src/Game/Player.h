#pragma once

#include "../Terrain/Block.h"

#define PLAYER_INIT \
  {.heldItem = BLOCK_GRASS, .breakCooldownS = 0.2f, .lastBreakTimeS = 0.f};

struct Player {
  enum BlockId heldItem;
  float breakCooldownS;
  float lastBreakTimeS;
};
