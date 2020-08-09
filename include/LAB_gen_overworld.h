#pragma once

#include "LAB_stdinc.h"
#include "LAB_world.h"

typedef struct LAB_GenOverworld
{
    uint64_t seed;
} LAB_GenOverworld;

LAB_Chunk* LAB_GenOverworldProc(void* user, LAB_World* world, int x, int y, int z);
