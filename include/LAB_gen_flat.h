#pragma once
#include "LAB_world.h"

typedef struct LAB_GenFlat
{
    LAB_BlockID block;
} LAB_GenFlat;

bool LAB_GenFlatProc(void* user, LAB_Chunk* chunk, int x, int y, int z);
