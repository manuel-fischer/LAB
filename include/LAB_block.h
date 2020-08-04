#pragma once

#include "LAB_stdinc.h"

typedef unsigned LAB_BlockFlags;

#define LAB_BLOCK_SOLID 1u


typedef struct LAB_Block
{
    LAB_BlockFlags flags;
    uint8_t r, g, b, a;
    uint8_t tx, ty; // Offset of the texture tile
} LAB_Block;


extern LAB_Block LAB_BLOCK_OUTSIDE,
                 LAB_BLOCK_AIR,
                 LAB_BLOCK_STONE,
                 LAB_BLOCK_COBBLESTONE,
                 LAB_BLOCK_GRASS;
