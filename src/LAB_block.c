#include "LAB_block.h"
#include "IXO/IXO_classdef.h"


IXO_BITDEF(LAB_BlockFlags,
    (SOLID, LAB_BLOCK_SOLID),
        (MASSIVE, LAB_BLOCK_MASSIVE),
        (OPAQUE, LAB_BLOCK_OPAQUE),
        (VISUAL, LAB_BLOCK_VISUAL),
        (INTERACTABLE, LAB_BLOCK_INTERACTABLE),

    (TRANSPARENT, LAB_BLOCK_TRANSPARENT),

    (GLOWING, LAB_BLOCK_GLOWING),
        (FLAT_SHADE, LAB_BLOCK_FLAT_SHADE),
        (EMISSIVE, LAB_BLOCK_EMISSIVE)
);

#if 0
IXO_STRUCTDEF(LAB_Block,
    (flags, &LAB_BlockFlags_class)
    ...
);
#endif

LAB_Block LAB_BLOCK_OUTSIDE = {
    .flags = LAB_BLOCK_OPAQUE,
    .dia = LAB_RGB(255, 255, 255),
};


LAB_Block LAB_BLOCK_AIR = {
    .flags = 0,
    .dia = LAB_RGB(255, 255, 255),
};

LAB_DEF_STONE_BLOCKS(BASALT,    LAB_RGB(80, 70, 60));
LAB_DEF_STONE_BLOCKS(STONE,     LAB_RGB(180, 180, 170));
LAB_DEF_STONE_BLOCKS(MARBLE,    LAB_RGB(255, 255, 255));
LAB_DEF_STONE_BLOCKS(CLAY,      LAB_RGB(200, 80, 50));
LAB_DEF_STONE_BLOCKS(LAPIZ,     LAB_RGB(30, 80, 200));
LAB_DEF_STONE_BLOCKS(SANDSTONE, LAB_RGB(255, 230, 150)); //.tint = LAB_RGB(236, 223, 179),

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_SAND, 3, 1, LAB_RGB(255, 230, 130),);
LAB_Block LAB_BLOCK_SAND = {
    .flags = LAB_BLOCK_SOLID,
    .item_tint = LAB_RGB(255, 230, 130),
    .item_tx = 3, 1,
    .model = &LAB_MODEL_SAND,
};


LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_GLASS, 1, 3, LAB_RGBA(255, 255, 255, 128),
                       .render_pass = LAB_RENDER_PASS_ALPHA);
LAB_Block LAB_BLOCK_GLASS = {
    .flags = (LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_OPAQUE_SELF),
    //.tint = LAB_RGB(20, 75, 10),
    //.dia = LAB_RGB(200, 200, 230),
    .dia = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGBA(255, 255, 255, 128),
    .item_tx = 1, 3,
    .model = &LAB_MODEL_GLASS,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_LEAVES, 2, 2, LAB_RGB(20, 150, 40),
                       .render_pass = LAB_RENDER_PASS_MASKED);
LAB_Block LAB_BLOCK_LEAVES = {
    .flags = (LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL),
    //.tint = LAB_RGB(20, 75, 10),
    .dia = LAB_RGB(130, 200, 130),
    .item_tint = LAB_RGB(20, 150, 40),
    .item_tx = 2, 2,
    .model = &LAB_MODEL_LEAVES,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_WOOD, 2, 0, LAB_RGB(93, 75, 60),);
LAB_Block LAB_BLOCK_WOOD = {
    .flags = LAB_BLOCK_SOLID,
    .item_tint = LAB_RGB(93, 75, 60),
    .item_tx = 2, 0,
    .model = &LAB_MODEL_WOOD,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_WOOD_PLANKS, 3, 0, LAB_RGB(220, 197, 162),);
LAB_Block LAB_BLOCK_WOOD_PLANKS = {
    .flags = LAB_BLOCK_SOLID,
    .item_tint = LAB_RGB(220, 197, 162),
    .item_tx = 3, 0,
    .model = &LAB_MODEL_WOOD_PLANKS,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_WOOD_PLANKS_DARK, 3, 0, LAB_RGB(120, 97, 80),);
LAB_Block LAB_BLOCK_WOOD_PLANKS_DARK = {
    .flags = LAB_BLOCK_SOLID,
    .item_tint = LAB_RGB(120, 97, 80),
    .item_tx = 3, 0,
    .model = &LAB_MODEL_WOOD_PLANKS_DARK,
};

LAB_DEF_MODEL_CROSS(LAB_MODEL_TALLGRASS, 3, 2, LAB_RGB(30, 170, 50),
                    .render_pass = LAB_RENDER_PASS_MASKED);
LAB_Block LAB_BLOCK_TALLGRASS = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
    .dia = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGB(30, 170, 50),
    .item_tx = 3, 2,
    .model = &LAB_MODEL_TALLGRASS,
};

LAB_DEF_MODEL_CROSS(LAB_MODEL_TALLERGRASS, 3, 3, LAB_RGB(30, 170, 50),
                    .render_pass = LAB_RENDER_PASS_MASKED);
LAB_Block LAB_BLOCK_TALLERGRASS = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
    .dia = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGB(30, 170, 50),
    .item_tx = 3, 3,
    .model = &LAB_MODEL_TALLERGRASS,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_GRASS, 2, 1, LAB_RGB(30, 170, 50),);
LAB_Block LAB_BLOCK_GRASS = {
    .flags = LAB_BLOCK_SOLID,
    //.tint = LAB_RGB(30, 200, 80),
    .item_tint = LAB_RGB(30, 170, 50),
    .item_tx = 2, 1,
    .model = &LAB_MODEL_GRASS,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_DIRT, 3, 1, LAB_RGB(120, 80, 50),);
LAB_Block LAB_BLOCK_DIRT = {
    .flags = LAB_BLOCK_SOLID,
    .item_tint = LAB_RGB(120, 80, 50),
    .item_tx = 3, 1,
    .model = &LAB_MODEL_DIRT,
};


LAB_DEF_MODEL_CROSS(LAB_MODEL_TORCH, 0, 3, LAB_RGB(255, 255, 255),
                    .render_pass = LAB_RENDER_PASS_MASKED);
LAB_Block LAB_BLOCK_TORCH = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE|LAB_BLOCK_GLOWING),
    .dia = LAB_RGB(255, 255, 255),
    .lum = LAB_RGB(220, 210, 180),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 0, 3,
    .model = &LAB_MODEL_TORCH,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT, 0, 2, LAB_RGB(255, 255, 255),);
LAB_Block LAB_BLOCK_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING,
    .lum = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 0, 2,
    .model = &LAB_MODEL_LIGHT,
};
LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_WARM_LIGHT, 1, 2, LAB_RGB(255, 255, 255),);
LAB_Block LAB_BLOCK_WARM_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING,
    .lum = LAB_RGB(255, 255, 220),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 1, 2,
    .model = &LAB_MODEL_WARM_LIGHT,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT_BLUE, 0, 2, LAB_RGB(64, 150, 255),);
LAB_Block LAB_BLOCK_BLUE_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING,
    .lum = LAB_RGB(16, 64, 255),
    .item_tint = LAB_RGB(64, 150, 255),
    .item_tx = 0, 2,
    .model = &LAB_MODEL_LIGHT_BLUE,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT_YELLOW, 1, 2, LAB_RGB(255, 240, 128),);
LAB_Block LAB_BLOCK_YELLOW_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING,
    .lum = LAB_RGB(255, 240, 16),
    .item_tint = LAB_RGB(255, 240, 128),
    .item_tx = 1, 2,
    .model = &LAB_MODEL_LIGHT_YELLOW,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT_GREEN, 0, 2, LAB_RGB(150, 255, 128),);
LAB_Block LAB_BLOCK_GREEN_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING,
    .lum = LAB_RGB(64, 255, 16),
    .item_tint = LAB_RGB(150, 255, 128),
    .item_tx = 0, 2,
    .model = &LAB_MODEL_LIGHT_GREEN,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT_RED, 1, 2, LAB_RGB(255, 100, 75),);
LAB_Block LAB_BLOCK_RED_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING,
    //.lum = LAB_RGB(255, 64, 16),
    .lum = LAB_RGB(255, 20, 16),
    //.item_tint = LAB_RGB(255, 100, 50),
    .item_tint = LAB_RGB(255, 100, 75),
    .item_tx = 1, 2,
    .model = &LAB_MODEL_LIGHT_RED,
};

// TODO: different texture
LAB_Block LAB_BLOCK_INVISIBLE_LIGHT = {
    .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_EMISSIVE,
    .lum = LAB_RGB(255, 255, 255),
    .dia = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 0, 7,
    .model = &LAB_MODEL_LIGHT
};

LAB_Block LAB_BLOCK_BARRIER = {
    .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_MASSIVE,
    .dia = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 1, 7,
    .model = &LAB_MODEL_LIGHT_RED,
};


LAB_Block* LAB_blocks_arr[] =
{
    LAB_ENM_STONE_BLOCKS(&,BASALT),
    LAB_ENM_STONE_BLOCKS(&,STONE),
    LAB_ENM_STONE_BLOCKS(&,MARBLE),
    LAB_ENM_STONE_BLOCKS(&,CLAY),
    LAB_ENM_STONE_BLOCKS(&,LAPIZ),
    LAB_ENM_STONE_BLOCKS(&,SANDSTONE),
    &LAB_BLOCK_SAND,
    &LAB_BLOCK_GLASS,
    &LAB_BLOCK_LEAVES,
    &LAB_BLOCK_WOOD,
    &LAB_BLOCK_WOOD_PLANKS,
    &LAB_BLOCK_WOOD_PLANKS_DARK,
    &LAB_BLOCK_TALLGRASS,
    &LAB_BLOCK_TALLERGRASS,
    &LAB_BLOCK_GRASS,
    &LAB_BLOCK_DIRT,
    &LAB_BLOCK_TORCH,
    &LAB_BLOCK_LIGHT,
    &LAB_BLOCK_WARM_LIGHT,
    &LAB_BLOCK_BLUE_LIGHT,
    &LAB_BLOCK_YELLOW_LIGHT,
    &LAB_BLOCK_GREEN_LIGHT,
    &LAB_BLOCK_RED_LIGHT,
    &LAB_BLOCK_INVISIBLE_LIGHT,
    &LAB_BLOCK_BARRIER,
};
LAB_Block** LAB_blocks = LAB_blocks_arr;
size_t LAB_block_count = sizeof LAB_blocks_arr/sizeof *LAB_blocks_arr;
