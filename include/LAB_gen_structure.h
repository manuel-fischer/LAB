#pragma once

#include "LAB_gen.h"
#include "LAB_random.h"

typedef struct LAB_StructureLayer
{
    int salt;
    int probability; // in range [0, 256],
    int min_count, max_count;
    int min_height, max_height; // both inclusive
    void (*structure_func)(LAB_Placer* p, LAB_Random* rnd);
} LAB_StructureLayer;
