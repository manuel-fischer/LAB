#pragma once

#include "LAB_game_surface_dimension.h"

LAB_Game_IStructurePlacement LAB_SurfaceDimension_PlaceOnSurface(LAB_SurfaceDimension* dim);
LAB_Game_IStructurePlacement LAB_SurfaceDimension_PlaceOnCaveWall(LAB_SurfaceDimension* dim, LAB_DirIndex into_wall);