#pragma once

#include "LAB_game_surface_dimension.h"

extern LAB_SurfaceDimension* LAB_builtin_overworld;

#define LAB_BUILTIN_VIS extern
#include "LAB_builtin_overworld_biomes_list.h"
#undef LAB_BUILTIN_VIS

#include "LAB_builtin_overworld_funcs.h"

bool LAB_BuiltinOverworld_Init(void);