#pragma once

#include "LAB_stdinc.h"
#include "LAB_world.h"


#define LAB_SURFACE_FACTOR (4)
#define LAB_SURFACE_MIN_CY (0)
#define LAB_SURFACE_MAX_CY (12)
#define LAB_SURFACE_MIN_Y (16*LAB_SURFACE_MIN_CY)
#define LAB_SURFACE_MAX_Y (16*LAB_SURFACE_MAX_CY)
#define LAB_ROCKY_ALTITUDE (72)

#define LAB_CAVE_ALTITUDE_C (2)
#define LAB_CAVE_ALTITUDE (16*LAB_CAVE_ALTITUDE_C)

#define LAB_SEA_BOTTOM_C (-3)
#define LAB_SEA_BOTTOM (16*LAB_SEA_BOTTOM_C)

typedef struct LAB_GenOverworld
{
    uint64_t seed;
} LAB_GenOverworld;

bool LAB_GenOverworldProc(void* user, LAB_Chunk* chunk, int x, int y, int z);
