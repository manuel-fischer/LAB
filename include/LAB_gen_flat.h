#pragma once
#include "LAB_world.h"

typedef struct LAB_GenFlat
{
    LAB_Block* block;
} LAB_GenFlat;

LAB_Chunk* LAB_GenFlatProc(void* user, LAB_Chunk* chunk, int x, int y, int z);
