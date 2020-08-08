#pragma once

#include "LAB_stdinc.h"

typedef unsigned LAB_BlockFlags;

#define LAB_BLOCK_SOLID    1u
#define LAB_BLOCK_EMISSIVE 2u


typedef struct LAB_Block
{
    LAB_BlockFlags flags;
    uint8_t r, g, b, a;
    uint8_t lr, lg, lb, la; // Emitted Light
    uint8_t tx, ty; // Offset of the texture tile
} LAB_Block;


extern LAB_Block LAB_BLOCK_OUTSIDE,
                 LAB_BLOCK_AIR,
                 LAB_BLOCK_STONE,
                 LAB_BLOCK_COBBLESTONE,
                 LAB_BLOCK_MARBLE,
                 LAB_BLOCK_MARBLECOBBLE,
                 LAB_BLOCK_SANDSTONE,
                 LAB_BLOCK_SANDCOBBLE,
                 LAB_BLOCK_SAND,
                 LAB_BLOCK_WOOD,
                 LAB_BLOCK_WOOD_PLANKS,
                 LAB_BLOCK_WOOD_PLANKS_DARK,
                 LAB_BLOCK_GRASS,
                 LAB_BLOCK_LIGHT;
