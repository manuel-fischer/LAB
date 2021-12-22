#pragma once

#include "LAB_world.h"

/**
 *  Return face bitset of faces of the chunk cube that were touched
 */
void LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27]);
