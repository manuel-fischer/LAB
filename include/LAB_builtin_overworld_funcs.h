#pragma once

#include "LAB_game_surface_dimension.h"
#include "LAB_random.h"

int LAB_BuiltinOverworld_SurfaceHeight_Func(uint64_t world_seed, int x, int z);
bool LAB_BuiltinOverworld_IsCave_Func(uint64_t world_seed, int x, int y, int z);
LAB_SurfaceBiomeID LAB_BuiltinOverworld_SurfaceBiome_Func(uint64_t world_seed, int x, int z, LAB_Random* random);
