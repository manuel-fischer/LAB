#pragma once

/**
 * wrapper to use older code with the new dimension interface
 */

#include "LAB_stdinc.h"
#include "LAB_world.h"
#include "LAB_game_dimension.h"

typedef struct LAB_GenDimensionWrapper_Ctx
{
    LAB_IDimension dim;
    uint64_t world_seed;
} LAB_GenDimensionWrapper_Ctx;

bool LAB_GenDimensionWrapper_Gen_Func(void* user, LAB_Chunk* chunk, int x, int y, int z);