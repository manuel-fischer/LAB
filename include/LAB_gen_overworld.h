#pragma once

#include "LAB_stdinc.h"
#include "LAB_world.h"


#define LAB_SURFACE_FACTOR (4)
#define LAB_SURFACE_MIN_CY (-2)
#define LAB_SURFACE_MAX_CY (10)
#define LAB_SURFACE_MIN_Y (16*LAB_SURFACE_MIN_CY)
#define LAB_SURFACE_MAX_Y (16*LAB_SURFACE_MAX_CY)


typedef struct LAB_GenOverworld
{
    uint64_t seed;
} LAB_GenOverworld;

LAB_Chunk* LAB_GenOverworldProc(void* user, LAB_Chunk* chunk, int x, int y, int z);
