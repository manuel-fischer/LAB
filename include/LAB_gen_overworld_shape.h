#pragma once

#include "LAB_gen_overworld.h"

void LAB_Gen_Surface_Shape(LAB_GenOverworld* gen, LAB_Chunk_Blocks* chunk_blocks, int cx, int cy, int cz);
int  LAB_Gen_Surface_Shape_Func(LAB_GenOverworld* gen, int x, int z);     // height

int  LAB_Gen_River_Func(LAB_GenOverworld* gen, int x, int z);     // depth of the river, 0 if no river

void LAB_Gen_Cave_Carve(LAB_GenOverworld* gen, LAB_Chunk_Blocks* chunk_blocks, int cx, int cy, int cz);
bool LAB_Gen_Cave_Carve_Func(LAB_GenOverworld* gen, int x, int y, int z); // block is cave
//int  LAB_Gen_Cave_Carve_Ceiling(LAB_GenOverworld* gen, int x, int y, int z, int y_dist);
//int  LAB_Gen_Cave_Carve_Floor(LAB_GenOverworld* gen, int x, int y, int z, int y_dist);

bool LAB_Gen_PlaceOnSurface(void* /*LAB_GenOverworld*/ gen, int* x, int* y, int* z);
bool LAB_Gen_PlaceOnRiverFloor(void* /*LAB_GenOverworld*/ gen, int* x, int* y, int* z);
bool LAB_Gen_PlaceOnCaveCeiling(void* /*LAB_GenOverworld*/ gen, int* x, int* y, int* z);
bool LAB_Gen_PlaceOnCaveFloor(void* /*LAB_GenOverworld*/ gen, int* x, int* y, int* z);
bool LAB_Gen_PlaceAnywhere(void* /*LAB_GenOverworld*/ gen, int* x, int* y, int* z);
