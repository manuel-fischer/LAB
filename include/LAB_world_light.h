#pragma once

#include "LAB_world.h"

/**
 *  Return chunk pos of changed light blocks
 *  READ: pos, light_generated
 *  RW:   color array
 */
LAB_CCPS LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27],
                       int neighbors_changed, LAB_CCPS blocks_changed);
