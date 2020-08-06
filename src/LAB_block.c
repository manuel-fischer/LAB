#include "LAB_block.h"

LAB_Block LAB_BLOCK_OUTSIDE = {};


LAB_Block LAB_BLOCK_AIR = {
    .flags = 0,
};

LAB_Block LAB_BLOCK_STONE = {
    .flags = LAB_BLOCK_SOLID,
    //.r = 200, 200, 200
    //.r = 150, 150, 140
    .r = 180, 180, 170,
    .tx = 0, 0,
};

LAB_Block LAB_BLOCK_COBBLESTONE = {
    .flags = LAB_BLOCK_SOLID,
    .r = 180, 180, 170,
    .tx = 1, 0,
};


LAB_Block LAB_BLOCK_MARBLE = {
    .flags = LAB_BLOCK_SOLID,
    //.r = 200, 200, 200
    //.r = 150, 150, 140
    .r = 255, 255, 255,
    .tx = 0, 0,
};

LAB_Block LAB_BLOCK_COBBLEMARBLE = {
    .flags = LAB_BLOCK_SOLID,
    .r = 255, 255, 255,
    .tx = 1, 0,
};

LAB_Block LAB_BLOCK_WOOD = {
    .flags = LAB_BLOCK_SOLID,
    .r  = 93, 75, 60,
    .tx = 2, 0,
};

LAB_Block LAB_BLOCK_WOOD_PLANKS = {
    .flags = LAB_BLOCK_SOLID,
    .r  = 220, 197, 162,
    .tx = 3, 0,
};

LAB_Block LAB_BLOCK_WOOD_PLANKS_DARK = {
    .flags = LAB_BLOCK_SOLID,
    .r  = 120, 97, 80,
    .tx = 3, 0,
};


LAB_Block LAB_BLOCK_GRASS = {
    .flags = LAB_BLOCK_SOLID,
    .r = 50, 255, 100,
    .tx = 0, 0,
};

LAB_Block LAB_BLOCK_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    .r  = 255, 255, 255,
    .lr = 255, 255, 255,
    .tx = 0, 1,
};
