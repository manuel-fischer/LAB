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

LAB_Block LAB_BLOCK_GRASS = {
    .flags = LAB_BLOCK_SOLID,
    .r = 50, 255, 100,
    .tx = 0, 0,
};

LAB_Block LAB_BLOCK_LIGHT = {
    .flags = LAB_BLOCK_SOLID,
    .r  = 255, 255, 255,
    .lr = 255, 255, 255,
    .tx = 0, 1,
};
