#pragma once

#include "LAB_stdinc.h"
#include "LAB_color.h"

typedef unsigned LAB_BlockFlags;

#define LAB_BLOCK_MASSIVE      1u // Entities collide with this
#define LAB_BLOCK_OPAQUE       2u // Other blocks get visually covered by this block
#define LAB_BLOCK_VISUAL       4u // The block is rendered
#define LAB_BLOCK_INTERACTABLE 8u // The block can be interacted with
#define LAB_BLOCK_SOLID        (LAB_BLOCK_MASSIVE|LAB_BLOCK_OPAQUE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL)

#define LAB_BLOCK_EMISSIVE     16u



typedef struct LAB_Block
{
    LAB_BlockFlags flags;
    uint8_t r, g, b, a;
    //uint8_t lr, lg, lb, la; // Emitted Light
    LAB_Color lum;
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
                 LAB_BLOCK_DIRT,
                 LAB_BLOCK_LIGHT,
                 LAB_BLOCK_BLUE_LIGHT,
                 LAB_BLOCK_YELLOW_LIGHT,
                 LAB_BLOCK_GREEN_LIGHT,
                 LAB_BLOCK_RED_LIGHT;
