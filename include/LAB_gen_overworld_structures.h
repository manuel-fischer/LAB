#pragma once

#include "LAB_gen.h"
#include "LAB_random.h"


typedef struct LAB_StructureLayer
{
    int salt;
    int probability; // in range [0, 256],
    int min_count, max_count;
    void (*structure_func)(LAB_Placer* p, LAB_Random* rnd);
} LAB_StructureLayer;

// Note: extern allows the array to be defined without a size
extern const LAB_StructureLayer overworld_layers[];
const size_t overworld_layers_count;

/*void LAB_Gen_Overworld_Plant(LAB_Placer* p, LAB_Random* rnd);
void LAB_Gen_Overworld_Bush(LAB_Placer* p, LAB_Random* rnd);
void LAB_Gen_Overworld_Tree(LAB_Placer* p, LAB_Random* rnd);
void LAB_Gen_Overworld_Tower(LAB_Placer* p, LAB_Random* rnd);*/
