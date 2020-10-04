#pragma once

#include "LAB_world.h"

/**
 *  Return face bitset of faces of the chunk cube that were touched
 */
int LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27], int cx, int cy, int cz);
