#pragma once

#include "../Terrain/Block.h"

#define PLAYER_INIT \
  {.heldItem = BLOCK_STONE, .breakCooldownS = 0.15f, .lastBreakTimeS = 0.f};

struct Player {
  enum BlockId heldItem;
  float breakCooldownS;
  float lastBreakTimeS;
};
