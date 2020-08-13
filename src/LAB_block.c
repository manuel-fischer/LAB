#include "LAB_block.h"

LAB_Block LAB_BLOCK_OUTSIDE = {
    .lum = LAB_RGB(255, 255, 255),
};


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

LAB_Block LAB_BLOCK_MARBLECOBBLE = {
    .flags = LAB_BLOCK_SOLID,
    .r = 255, 255, 255,
    .tx = 1, 0,
};


LAB_Block LAB_BLOCK_SANDSTONE = {
    .flags = LAB_BLOCK_SOLID,
    .r = 236, 223, 179,
    .tx = 0, 0,
};

LAB_Block LAB_BLOCK_SANDCOBBLE = {
    .flags = LAB_BLOCK_SOLID,
    //.r = 255, 230, 150,
    .r = 236, 223, 179,
    .tx = 1, 0,
};

LAB_Block LAB_BLOCK_SAND = {
    .flags = LAB_BLOCK_SOLID,
    //.r = 255, 230, 150,
    .r = 255, 230, 130,
    .tx = 3, 1,
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
    .lum = LAB_RGB(255, 255, 255),
    .tx = 0, 1,
};

LAB_Block LAB_BLOCK_BLUE_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    //.r  = 255, 255, 255,
    .r  = 64, 150, 255,
    //.lr = 255, 255, 255,
    .lum = LAB_RGB(16, 64, 255),
    .tx = 0, 1,
};

LAB_Block LAB_BLOCK_YELLOW_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    //.r  = 255, 255, 255,
    .r  = 255, 240, 128,
    //.lr = 255, 255, 255,
    .lum = LAB_RGB(255, 240, 16),
    .tx = 0, 1,
};

LAB_Block LAB_BLOCK_GREEN_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    //.r  = 255, 255, 255,
    .r  = 150, 255, 128,
    //.lr = 255, 255, 255,
    .lum = LAB_RGB(64, 255, 16),
    .tx = 0, 1,
};

LAB_Block LAB_BLOCK_RED_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    //.r  = 255, 255, 255,
    .r  = 255, 150, 50,
    //.lr = 255, 255, 255,
    .lum = LAB_RGB(255, 64, 16),
    .tx = 0, 1,
};
