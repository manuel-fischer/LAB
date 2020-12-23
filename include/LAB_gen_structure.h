#pragma once

#include "LAB_gen.h"
#include "LAB_random.h"

typedef struct LAB_StructureLayer
{
    int salt;
    int probability; // in range [0, 256],
    int min_count, max_count;
    int min_height, max_height; // both inclusive
    // return true if position was found,
    // read and write to x, y, z, to find the next position near
    // the given position
    // x, y, z might be changed anyway, if no position was found
    bool (*near_place_func)(void* gen_ctx, int* x, int* y, int* z);
    void (*structure_func)(LAB_Placer* p, LAB_Random* rnd);
} LAB_StructureLayer;
