#include "LAB_block.h"
//#include "IXO/IXO_classdef.h"
#include "LAB_color_defs.h"


/*IXO_BITDEF(LAB_BlockFlags,
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
#endif*/


#define LAB_COLOR_GRASS_PLAINS   LAB_RGB(60, 150, 20) // LAB_RGB(30, 170, 50)

#define LAB_COLOR_GRASS   LAB_RGBX(0f961e) // LAB_RGB(15, 150, 30) // LAB_RGB(30, 170, 50)
#define LAB_COLOR_LEAVES  LAB_RGB(10, 130, 20) // LAB_RGB(0, 100, 10) // LAB_RGB(20, 150, 40)
#define LAB_COLOR_WOOD    LAB_RGB(93, 75, 45) // LAB_RGB(93, 75, 60)

#define LAB_COLOR_BIRCH_LEAVES LAB_RGBX(ee8811) // LAB_RGB(0, 100, 10) // LAB_RGB(20, 150, 40)


LAB_Block LAB_BLOCK_OUTSIDE = {
    .flags = LAB_BLOCK_OPAQUE,
    .dia = LAB_RGB(255, 255, 255),
};


LAB_Block LAB_BLOCK_AIR = {
    .flags = 0,
    .dia = LAB_RGB(255, 255, 255),
};

LAB_DEF_BLOCK_GROUP_STONE(BASALT,    LAB_RGB(80, 70, 60));
LAB_DEF_BLOCK_GROUP_STONE(STONE,     LAB_RGB(180, 180, 170));
LAB_DEF_BLOCK_GROUP_STONE(MARBLE,    LAB_RGB(255, 255, 255));
LAB_DEF_BLOCK_GROUP_STONE(CLAY,      LAB_RGB(200, 80, 50));
LAB_DEF_BLOCK_GROUP_STONE(LAPIZ,     LAB_RGB(30, 80, 200));
LAB_DEF_BLOCK_GROUP_STONE(SANDSTONE, LAB_RGB(255, 230, 150)); //.tint = LAB_RGB(236, 223, 179),

LAB_DEF_BLOCK_CUBE(SAND,
    /*texture*/     3, 1, LAB_RGBX(ffffff), //LAB_RGB(255, 230, 130), 
    /*dia*/         0,
    /*render_pass*/ LAB_RENDER_PASS_SOLID,
    /*flags*/       LAB_BLOCK_SOLID
);

LAB_DEF_BLOCK_CUBE(GLASS,
    /*texture*/     1, 3, LAB_RGBA(255, 255, 255, 128), 
    /*dia*/         LAB_RGB(255, 255, 255), 
    /*render_pass*/ LAB_RENDER_PASS_ALPHA, 
    /*flags*/       LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_OPAQUE_SELF
);


LAB_DEF_BLOCK_CUBE(LEAVES, 
    /*texture*/     2, 2, LAB_COLOR_LEAVES, 
    /*dia*/         LAB_RGB(130, 200, 130),
    /*render_pass*/ LAB_RENDER_PASS_MASKED,
    /*flags*/       LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL
);

LAB_DEF_BLOCK_CUBE(WOOD,
    /*texture*/     2, 0, LAB_COLOR_WOOD,
    /*dia*/         0,
    /*render_pass*/ LAB_RENDER_PASS_SOLID,
    /*flags*/       LAB_BLOCK_SOLID
);

LAB_DEF_BLOCK_CUBE(WOOD_PLANKS,
    /*texture*/     3, 0, LAB_RGB(220, 197, 162),
    /*dia*/         0,
    /*render_pass*/ LAB_RENDER_PASS_SOLID,
    /*flags*/       LAB_BLOCK_SOLID
);

LAB_DEF_BLOCK_CUBE(WOOD_PLANKS_DARK,
    /*texture*/     3, 0, LAB_RGB(120, 97, 80),
    /*dia*/         0,
    /*render_pass*/ LAB_RENDER_PASS_SOLID,
    /*flags*/       LAB_BLOCK_SOLID
);

LAB_DEF_BLOCK_CUBE(BIRCH_LEAVES, 
    /*texture*/     2, 2, LAB_COLOR_BIRCH_LEAVES,
    /*dia*/         LAB_COLOR_BIRCH_LEAVES,
    /*render_pass*/ LAB_RENDER_PASS_MASKED,
    /*flags*/       LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL
);

LAB_DEF_BLOCK_CUBE(BIRCH_WOOD,
    /*texture*/     5, 0, LAB_RGBX(ffffff),
    /*dia*/         0,
    /*render_pass*/ LAB_RENDER_PASS_SOLID,
    /*flags*/       LAB_BLOCK_SOLID
);

LAB_DEF_BLOCK_CUBE(SPRUCE_LEAVES,
    /*texture*/     2, 2, LAB_RGBX(006622),
    /*dia*/         LAB_RGBX(006622),
    /*render_pass*/ LAB_RENDER_PASS_MASKED,
    /*flags*/       LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL
);

LAB_DEF_BLOCK_CUBE(SPRUCE_WOOD,
    /*texture*/     5, 0, LAB_RGBX(332211),
    /*dia*/         0,
    /*render_pass*/ LAB_RENDER_PASS_SOLID,
    /*flags*/       LAB_BLOCK_SOLID
);
//LAB_RGBX(664422)

LAB_DEF_MODEL_CROSS(LAB_MODEL_TALLGRASS, 3, 2, LAB_COLOR_GRASS,
                    .render_pass = LAB_RENDER_PASS_MASKED);
LAB_Block LAB_BLOCK_TALLGRASS = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
    .dia = LAB_RGBX(dddddd),
    .item_tint = LAB_COLOR_GRASS,
    .item_tx = 3, 2,
    .model = &LAB_MODEL_TALLGRASS,
    .bounds = {{0.125, 0, 0.125}, {0.875, 0.375, 0.875}},
};
//LAB_DEF_BLOCK_CUBE(TALLGRASS, 3, 2, LAB_SQR_COLOR(LAB_RGB(30, 170, 50)), LAB_RENDER_PASS_MASKED, LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE);

LAB_DEF_MODEL_CROSS(LAB_MODEL_TALLERGRASS, 3, 3, LAB_COLOR_GRASS,
                    .render_pass = LAB_RENDER_PASS_MASKED);
LAB_Block LAB_BLOCK_TALLERGRASS = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
    .dia = LAB_RGBX(dddddd),
    .item_tint = LAB_COLOR_GRASS,
    .item_tx = 3, 3,
    .model = &LAB_MODEL_TALLERGRASS,
    .bounds = {{0.125, 0, 0.125}, {0.875, 0.75, 0.875}},
};
//LAB_DEF_BLOCK_CUBE(TALLERGRASS, 3, 3, LAB_SQR_COLOR(LAB_RGB(30, 170, 50)), LAB_RENDER_PASS_MASKED, LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE);

/*LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_GRASS, 2, 1, LAB_RGB(30, 170, 50),);
LAB_Block LAB_BLOCK_GRASS = {
    .flags = LAB_BLOCK_SOLID,
    //.tint = LAB_RGB(30, 200, 80),
    .item_tint = LAB_RGB(30, 170, 50),
    .item_tx = 2, 1,
    .model = &LAB_MODEL_GRASS,
};*/
LAB_DEF_BLOCK_CUBE(GRASS,
    /*texture*/     2, 1, LAB_COLOR_GRASS,
    /*dia*/         LAB_COLOR_BLACK,
    /*render_pass*/ LAB_RENDER_PASS_SOLID,
    /*flags*/       LAB_BLOCK_SOLID
);
//LAB_DEF_BLOCK_CUBE(GRASS_PLAINS, 2, 1, LAB_COLOR_GRASS_PLAINS, LAB_COLOR_BLACK, LAB_RENDER_PASS_SOLID, LAB_BLOCK_SOLID);

LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_DIRT, 4, 1, LAB_RGB(120, 80, 50),);
LAB_Block LAB_BLOCK_DIRT = {
    .flags = LAB_BLOCK_SOLID,
    .item_tint = LAB_RGB(120, 80, 50),
    .item_tx = 4, 1,
    .model = &LAB_MODEL_DIRT,
    .bounds = LAB_AABB_FULL_CUBE,
};


LAB_DEF_MODEL_CROSS(LAB_MODEL_RED_TULIP, 5, 4, LAB_RGBX(ffffff),
                    .render_pass = LAB_RENDER_PASS_MASKED);
LAB_Block LAB_BLOCK_RED_TULIP = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
    .dia = LAB_RGBX(dddddd),
    .item_tint = LAB_RGBX(ffffff),
    .item_tx = 5, 4,
    .model = &LAB_MODEL_RED_TULIP,
    .bounds = {{0.25, 0, 0.25}, {0.75, 0.875, 0.75}},
};

LAB_DEF_MODEL_CROSS(LAB_MODEL_YELLOW_TULIP, 6, 4, LAB_RGBX(ffffff),
                    .render_pass = LAB_RENDER_PASS_MASKED);
LAB_Block LAB_BLOCK_YELLOW_TULIP = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
    .dia = LAB_RGBX(dddddd),
    .item_tint = LAB_RGBX(ffffff),
    .item_tx = 6, 4,
    .model = &LAB_MODEL_YELLOW_TULIP,
    .bounds = {{0.25, 0, 0.25}, {0.75, 0.875, 0.75}},
};


LAB_DEF_MODEL(LAB_MODEL_FALLEN_LEAVES, (.render_pass = LAB_RENDER_PASS_MASKED),
              LAB_MK_QUAD_1UV(0, 0.0625, 0,
                              1, 0.0625, 0,
                              0, 0.0625, 1,
                              1, 0.0625, 1,
                        /*uv*/2, 4,
                              LAB_COLOR_BIRCH_LEAVES,
                              LAB_DIR_ALL,
                              0,
                              LAB_DIR_UP)
);
LAB_Block LAB_BLOCK_FALLEN_LEAVES = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
    .dia = LAB_RGBX(ffffff),
    .item_tint = LAB_COLOR_BIRCH_LEAVES,
    .item_tx = 2, 4,
    .model = &LAB_MODEL_FALLEN_LEAVES,
    .bounds = {{0, 0, 0}, {1, 0.0625, 1}},
};




LAB_DEF_BLOCK_CUBE(METAL, 4, 0, LAB_RGBX(ffffff), LAB_COLOR_BLACK, LAB_RENDER_PASS_SOLID, LAB_BLOCK_SOLID);


/*LAB_DEF_MODEL_CROSS(LAB_MODEL_TORCH, 0, 3, LAB_RGB(255, 255, 255),
                    .render_pass = LAB_RENDER_PASS_MASKED);*/
#define X0 (7/16.)
#define X1 (9/16.)
#define Y1 (10/16.)
#define F0 0
#define F1 1
LAB_DEF_MODEL(LAB_MODEL_TORCH, (.render_pass = LAB_RENDER_PASS_MASKED),
              LAB_MK_QUAD_UV(X1, Y1, X0,
                             X0, Y1, X0,
                             X1, .0, X0,
                             X0, .0, X0,
                       /*uv*/0, 3, /**/ X0, 1-Y1, X1, 1,
                             LAB_RGB(255, 255, 255),
                             LAB_DIR_ALL,
                             0,
                             LAB_DIR_NORTH),
              LAB_MK_QUAD_UV(X0, Y1, X1, 
                             X1, Y1, X1,
                             X0, .0, X1,
                             X1, .0, X1,
                       /*uv*/0, 3, /**/ X0, 1-Y1, X1, 1,
                             LAB_RGB(255, 255, 255),
                             LAB_DIR_ALL,
                             0,
                             LAB_DIR_SOUTH),
              LAB_MK_QUAD_UV(X0, Y1, X0,
                             X0, Y1, X1,
                             X0, .0, X0,
                             X0, .0, X1,
                       /*uv*/0, 3, /**/ X0, 1-Y1, X1, 1,
                             LAB_RGB(255, 255, 255),
                             LAB_DIR_ALL,
                             0,
                             LAB_DIR_WEST),
              LAB_MK_QUAD_UV(X1, Y1, X1,
                             X1, Y1, X0,
                             X1, .0, X1,
                             X1, .0, X0,
                       /*uv*/0, 3, /**/ X0, 1-Y1, X1, 1,
                             LAB_RGB(255, 255, 255),
                             LAB_DIR_ALL,
                             0,
                             LAB_DIR_EAST),
              LAB_MK_QUAD_UV(X0, Y1, X0,
                             X1, Y1, X0,
                             X0, Y1, X1,
                             X1, Y1, X1,
                       /*uv*/0, 3, /**/ X0, (11/32.), X1, (15/32.),
                             LAB_RGB(255, 255, 255),
                             LAB_DIR_ALL,
                             0,
                             LAB_DIR_UP)
);
#undef X1
#undef X0
LAB_Block LAB_BLOCK_TORCH = {
    .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE|LAB_BLOCK_GLOWING|LAB_BLOCK_NOSHADE),
    .dia = LAB_RGB(255, 255, 255),
    //.lum = LAB_RGB(220, 210, 180),
    .lum = LAB_RGB(110, 105, 90),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 0, 3,
    .model = &LAB_MODEL_TORCH,
    //.bounds = {{0.375, 0, 0.375}, {0.625, 0.75, 0.625}},
    .bounds = {{7/16., 0, 7/16.}, {9/16., 10/16., 9/16.}},
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT, 0, 2, LAB_RGB(255, 255, 255),);
LAB_Block LAB_BLOCK_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 0, 2,
    .model = &LAB_MODEL_LIGHT,
    .bounds = LAB_AABB_FULL_CUBE,
};
LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_WARM_LIGHT, 1, 2, LAB_RGB(255, 255, 255),);
LAB_Block LAB_BLOCK_WARM_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(255, 255, 220),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 1, 2,
    .model = &LAB_MODEL_WARM_LIGHT,
    .bounds = LAB_AABB_FULL_CUBE,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT_BLUE, 0, 2, LAB_RGB(16, 64, 255),);
LAB_Block LAB_BLOCK_BLUE_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(16, 64, 255),
    .item_tint = LAB_RGB(16, 64, 255),//LAB_RGB(64, 150, 255),
    .item_tx = 0, 2,
    .model = &LAB_MODEL_LIGHT_BLUE,
    .bounds = LAB_AABB_FULL_CUBE,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT_YELLOW, 1, 2, LAB_RGB(255, 240, 16),);
LAB_Block LAB_BLOCK_YELLOW_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(255, 240, 16),
    .item_tint = LAB_RGB(255, 240, 16),//LAB_RGB(255, 240, 128),
    .item_tx = 1, 2,
    .model = &LAB_MODEL_LIGHT_YELLOW,
    .bounds = LAB_AABB_FULL_CUBE,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT_GREEN, 0, 2,LAB_RGB(64, 255, 16),);
LAB_Block LAB_BLOCK_GREEN_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(64, 255, 16),
    .item_tint = LAB_RGB(64, 255, 16),//LAB_RGB(150, 255, 128),
    .item_tx = 0, 2,
    .model = &LAB_MODEL_LIGHT_GREEN,
    .bounds = LAB_AABB_FULL_CUBE,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_LIGHT_RED, 1, 2, LAB_RGB(255, 20, 16),);
LAB_Block LAB_BLOCK_RED_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    //.lum = LAB_RGB(255, 64, 16),
    .lum = LAB_RGB(255, 20, 16),
    //.item_tint = LAB_RGB(255, 100, 50),
    .item_tint = LAB_RGB(255, 20, 16),//LAB_RGB(255, 100, 75),
    .item_tx = 1, 2,
    .model = &LAB_MODEL_LIGHT_RED,
    .bounds = LAB_AABB_FULL_CUBE,
};


LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_ORANGE_LIGHT, 1, 2, LAB_RGB(255, 128, 16),);
LAB_Block LAB_BLOCK_ORANGE_LIGHT = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    //.lum = LAB_RGB(255, 64, 16),
    .lum = LAB_RGB(255, 128, 16),
    //.item_tint = LAB_RGB(255, 100, 50),
    .item_tint = LAB_RGB(255, 128, 16),//LAB_RGB(255, 100, 75),
    .item_tx = 1, 2,
    .model = &LAB_MODEL_ORANGE_LIGHT,
    .bounds = LAB_AABB_FULL_CUBE,
};




LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_CRYSTAL_BLUE, 2, 3, LAB_RGB(32, 32, 255),);
LAB_Block LAB_BLOCK_BLUE_CRYSTAL = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(16/2, 64/2, 255/2),
    .item_tint = LAB_RGB(32, 32, 255),
    .item_tx = 2, 3,
    .model = &LAB_MODEL_CRYSTAL_BLUE,
    .bounds = LAB_AABB_FULL_CUBE,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_CRYSTAL_YELLOW, 2, 3, LAB_RGB(255, 240, 32),);
LAB_Block LAB_BLOCK_YELLOW_CRYSTAL = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(255/2, 240/2, 16/2),
    .item_tint = LAB_RGB(255, 240, 32),
    .item_tx = 2, 3,
    .model = &LAB_MODEL_CRYSTAL_YELLOW,
    .bounds = LAB_AABB_FULL_CUBE,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_CRYSTAL_GREEN, 2, 3, LAB_RGB(40, 255, 32),);
LAB_Block LAB_BLOCK_GREEN_CRYSTAL = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(64/2, 255/2, 16/2),
    .item_tint = LAB_RGB(40, 255, 32),
    .item_tx = 2, 3,
    .model = &LAB_MODEL_CRYSTAL_GREEN,
    .bounds = LAB_AABB_FULL_CUBE,
};

LAB_DEF_MODEL_CUBE_SHADELESS_ALL(LAB_MODEL_CRYSTAL_RED, 2, 3, LAB_RGB(255, 25, 15),);
LAB_Block LAB_BLOCK_RED_CRYSTAL = {
    .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
    .lum = LAB_RGB(255/2, 20/2, 16/2),
    .item_tint = LAB_RGB(255, 25, 15),
    .item_tx = 2, 3,
    .model = &LAB_MODEL_CRYSTAL_RED,
    .bounds = LAB_AABB_FULL_CUBE,
};




LAB_Block LAB_BLOCK_INVISIBLE_LIGHT = {
    .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_EMISSIVE,
    .lum = LAB_RGB(255, 255, 255),
    .dia = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 0, 7,
    .model = &LAB_MODEL_LIGHT,
    .bounds = LAB_AABB_FULL_CUBE,
};

LAB_Block LAB_BLOCK_BARRIER = {
    .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_MASSIVE,
    .dia = LAB_RGB(255, 255, 255),
    .item_tint = LAB_RGB(255, 255, 255),
    .item_tx = 1, 7,
    .model = &LAB_MODEL_LIGHT_RED,
    .bounds = LAB_AABB_FULL_CUBE,
};


LAB_Block* LAB_blocks_arr[] =
{
    LAB_ENM_BLOCK_GROUP_STONE(&,BASALT),
    LAB_ENM_BLOCK_GROUP_STONE(&,STONE),
    LAB_ENM_BLOCK_GROUP_STONE(&,MARBLE),
    LAB_ENM_BLOCK_GROUP_STONE(&,CLAY),
    LAB_ENM_BLOCK_GROUP_STONE(&,LAPIZ),
    LAB_ENM_BLOCK_GROUP_STONE(&,SANDSTONE),
    &LAB_BLOCK_SAND,
    &LAB_BLOCK_GLASS,
    &LAB_BLOCK_LEAVES,
    &LAB_BLOCK_WOOD,
    &LAB_BLOCK_WOOD_PLANKS,
    &LAB_BLOCK_WOOD_PLANKS_DARK,
    &LAB_BLOCK_BIRCH_LEAVES,
    &LAB_BLOCK_BIRCH_WOOD,
    &LAB_BLOCK_SPRUCE_LEAVES,
    &LAB_BLOCK_SPRUCE_WOOD,
    &LAB_BLOCK_TALLGRASS,
    &LAB_BLOCK_TALLERGRASS,
    &LAB_BLOCK_GRASS,
    &LAB_BLOCK_RED_TULIP,
    &LAB_BLOCK_YELLOW_TULIP,
    &LAB_BLOCK_FALLEN_LEAVES,
    //&LAB_BLOCK_GRASS_PLAINS,
    &LAB_BLOCK_DIRT,
    &LAB_BLOCK_METAL,
    &LAB_BLOCK_TORCH,
    &LAB_BLOCK_LIGHT,
    &LAB_BLOCK_WARM_LIGHT,
    &LAB_BLOCK_BLUE_LIGHT,
    &LAB_BLOCK_YELLOW_LIGHT,
    &LAB_BLOCK_GREEN_LIGHT,
    &LAB_BLOCK_RED_LIGHT,
    &LAB_BLOCK_ORANGE_LIGHT,
    &LAB_BLOCK_BLUE_CRYSTAL,
    &LAB_BLOCK_YELLOW_CRYSTAL,
    &LAB_BLOCK_GREEN_CRYSTAL,
    &LAB_BLOCK_RED_CRYSTAL,
    &LAB_BLOCK_INVISIBLE_LIGHT,
    &LAB_BLOCK_BARRIER,
};
LAB_Block** LAB_blocks = LAB_blocks_arr;
size_t LAB_block_count = sizeof LAB_blocks_arr/sizeof *LAB_blocks_arr;
