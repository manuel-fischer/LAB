#pragma once

#include "LAB_gen.h"
#include "LAB_gen_structure.h"

// Note: extern allows the array to be defined without a size
extern const LAB_StructureLayer overworld_layers[];
extern const size_t overworld_layers_count;

extern const LAB_StructureLayer cave_layers[];
extern const size_t cave_layers_count;

/*void LAB_Gen_Overworld_Plant(LAB_Placer* p, LAB_Random* rnd);
void LAB_Gen_Overworld_Bush(LAB_Placer* p, LAB_Random* rnd);
void LAB_Gen_Overworld_Tree(LAB_Placer* p, LAB_Random* rnd);
void LAB_Gen_Overworld_Tower(LAB_Placer* p, LAB_Random* rnd);*/
