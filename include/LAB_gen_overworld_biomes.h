#pragma once

#include "LAB_block.h"
#include "LAB_gen_overworld.h"

typedef struct LAB_Gen_Biome
{
    uint64_t tags;
    const LAB_Block* surface_block;
    const LAB_Block* ground_block;

} LAB_Gen_Biome;

const LAB_Gen_Biome LAB_biome_forest;
const LAB_Gen_Biome LAB_biome_plains;
const LAB_Gen_Biome LAB_biome_desert;


float LAB_Gen_Biome_Temperature_Func(LAB_GenOverworld* gen, int x, int z);
float LAB_Gen_Biome_Humidity_Func(LAB_GenOverworld* gen, int x, int z);

const LAB_Gen_Biome* LAB_Gen_Biome_Func(LAB_GenOverworld* gen, int x, int z, uint64_t rand_val);
