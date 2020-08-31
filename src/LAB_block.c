#include "LAB_block.h"
#include "LAB_blt_model.h"
#include "IXO/IXO_classdef.h"



#define LAB_BLOCK_MASSIVE      1u // Entities collide with this
#define LAB_BLOCK_OPAQUE       2u // Other blocks get visually covered by this block
#define LAB_BLOCK_VISUAL       4u // The block is rendered
#define LAB_BLOCK_INTERACTABLE 8u // The block can be interacted with
#define LAB_BLOCK_SOLID        (LAB_BLOCK_MASSIVE|LAB_BLOCK_OPAQUE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL)

#define LAB_BLOCK_TRANSPARENT  16u // The block has some transparent pixels
                                   // LAB_BLOCK_SOLID should not be set, otherwise it has an XRay-effect

#define LAB_BLOCK_EMISSIVE     128u
IXO_BITDEF(LAB_BlockFlags,
    (SOLID, LAB_BLOCK_SOLID),
        (MASSIVE, LAB_BLOCK_MASSIVE),
        (OPAQUE, LAB_BLOCK_OPAQUE),
        (VISUAL, LAB_BLOCK_VISUAL),
        (INTERACTABLE, LAB_BLOCK_INTERACTABLE),

    (TRANSPARENT, LAB_BLOCK_TRANSPARENT),

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
    .lum = LAB_RGB(255, 255, 255),
};


LAB_Block LAB_BLOCK_AIR = {
    .flags = 0,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_STONE, 0, 0, LAB_RGB(180, 180, 170));
LAB_Block LAB_BLOCK_STONE = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(180, 180, 170),
    .tx = 0, 0,
    .model = &LAB_MODEL_STONE,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_COBBLESTONE, 1, 0, LAB_RGB(180, 180, 170));
LAB_Block LAB_BLOCK_COBBLESTONE = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(180, 180, 170),
    .tx = 1, 0,
    .model = &LAB_MODEL_COBBLESTONE,
};


LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_MARBLE, 0, 0, LAB_RGB(255, 255, 255));
LAB_Block LAB_BLOCK_MARBLE = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(255, 255, 255),
    .tx = 0, 0,
    .model = &LAB_MODEL_MARBLE,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_MARBLECOBBLE, 1, 0, LAB_RGB(255, 255, 255));
LAB_Block LAB_BLOCK_MARBLECOBBLE = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(255, 255, 255),
    .tx = 1, 0,
    .model = &LAB_MODEL_MARBLECOBBLE,
};


LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_SANDSTONE, 0, 0, LAB_RGB(236, 223, 179));
LAB_Block LAB_BLOCK_SANDSTONE = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(236, 223, 179),
    .tx = 0, 0,
    .model = &LAB_MODEL_SANDSTONE,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_SANDCOBBLE, 1, 0, LAB_RGB(236, 223, 179));
LAB_Block LAB_BLOCK_SANDCOBBLE = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(236, 223, 179),
    .tx = 1, 0,
    .model = &LAB_MODEL_SANDCOBBLE,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_SAND, 3, 1, LAB_RGB(255, 230, 130));
LAB_Block LAB_BLOCK_SAND = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(255, 230, 130),
    .tx = 3, 1,
    .model = &LAB_MODEL_SAND,
};


LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_WOOD, 2, 0, LAB_RGB(93, 75, 60));
LAB_Block LAB_BLOCK_WOOD = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(93, 75, 60),
    .tx = 2, 0,
    .model = &LAB_MODEL_WOOD,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_WOOD_PLANKS, 3, 0, LAB_RGB(220, 197, 162));
LAB_Block LAB_BLOCK_WOOD_PLANKS = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(220, 197, 162),
    .tx = 3, 0,
    .model = &LAB_MODEL_WOOD_PLANKS,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_WOOD_PLANKS_DARK, 3, 0, LAB_RGB(120, 97, 80));
LAB_Block LAB_BLOCK_WOOD_PLANKS_DARK = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(120, 97, 80),
    .tx = 3, 0,
    .model = &LAB_MODEL_WOOD_PLANKS_DARK,
};


LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_GRASS, 2, 1, LAB_RGB(30, 200, 80));
LAB_Block LAB_BLOCK_GRASS = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(30, 200, 80),
    .tx = 2, 1,
    .model = &LAB_MODEL_GRASS,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_DIRT, 3, 1, LAB_RGB(120, 80, 50));
LAB_Block LAB_BLOCK_DIRT = {
    .flags = LAB_BLOCK_SOLID,
    .tint = LAB_RGB(120, 80, 50),
    .tx = 3, 1,
    .model = &LAB_MODEL_DIRT,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_LIGHT, 0, 1, LAB_RGB(255, 255, 255));
LAB_Block LAB_BLOCK_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    .tint = LAB_RGB(255, 255, 255),
    .lum = LAB_RGB(255, 255, 255),
    .tx = 0, 1,
    .model = &LAB_MODEL_LIGHT,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_LIGHT_BLUE, 0, 1, LAB_RGB(64, 150, 255));
LAB_Block LAB_BLOCK_BLUE_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    .tint = LAB_RGB(64, 150, 255),
    .lum = LAB_RGB(16, 64, 255),
    .tx = 0, 1,
    .model = &LAB_MODEL_LIGHT_BLUE,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_LIGHT_YELLOW, 0, 1, LAB_RGB(255, 240, 128));
LAB_Block LAB_BLOCK_YELLOW_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    .tint = LAB_RGB(255, 240, 128),
    .lum = LAB_RGB(255, 240, 16),
    .tx = 0, 1,
    .model = &LAB_MODEL_LIGHT_YELLOW,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_LIGHT_GREEN, 0, 1, LAB_RGB(150, 255, 128));
LAB_Block LAB_BLOCK_GREEN_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    .tint = LAB_RGB(150, 255, 128),
    .lum = LAB_RGB(64, 255, 16),
    .tx = 0, 1,
    .model = &LAB_MODEL_LIGHT_GREEN,
};

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_LIGHT_RED, 0, 1, LAB_RGB(255, 150, 50));
LAB_Block LAB_BLOCK_RED_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_EMISSIVE,
    .tint = LAB_RGB(255, 150, 50),
    .lum = LAB_RGB(255, 64, 16),
    .tx = 0, 1,
    .model = &LAB_MODEL_LIGHT_RED,
};
