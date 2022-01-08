#include "LAB_builtin_blocks.h"
#include "LAB_blocks.h"

#include "LAB_color_defs.h"
#include "LAB_builtin_block_types.h"



#define LAB_BUILTIN_VIS
#include "LAB_builtin_blocks_list.h"
#undef LAB_BUILTIN_VIS



#define LAB_COLOR_GRASS_PLAINS   LAB_RGB(60, 150, 20) // LAB_RGB(30, 170, 50)

#define LAB_COLOR_GRASS   LAB_RGBX(0f961e) // LAB_RGB(15, 150, 30) // LAB_RGB(30, 170, 50)
#define LAB_COLOR_LEAVES  LAB_RGB(10, 130, 20) // LAB_RGB(0, 100, 10) // LAB_RGB(20, 150, 40)
#define LAB_COLOR_WOOD    LAB_RGB(93, 75, 45) // LAB_RGB(93, 75, 60)

#define LAB_COLOR_BIRCH_LEAVES LAB_RGBX(ee8811) // LAB_RGB(0, 100, 10) // LAB_RGB(20, 150, 40)

//#define LAB_COLOR_WATER     LAB_RGBAX(2244aa44)
#define LAB_COLOR_WATER     LAB_RGBAX(2277ff44)
#define LAB_COLOR_WATER_DIA LAB_RGBX(ccddee)



bool LAB_BuiltinBlocks_Init(LAB_Assets* assets)
{
    // TODO error checking
    ///// FUNDAMENTAL /////
    {
        LAB_BLOCK_OUTSIDE.flags = LAB_BLOCK_OPAQUE;
        LAB_BLOCK_OUTSIDE.dia = LAB_RGB(255, 255, 255);
        //LAB_RegisterBlock(&LAB_BLOCK_OUTSIDE);

        LAB_BLOCK_AIR.flags = 0;
        LAB_BLOCK_AIR.tags = LAB_BLOCK_TAG_REPLACEABLE;
        LAB_BLOCK_AIR.dia = LAB_RGB(255, 255, 255);
        //LAB_RegisterBlock(&LAB_BLOCK_AIR);
    }
    




    ///// ROCK TYPES /////
    {
        LAB_MaterialGroupStone stone_mat;
        LAB_MaterialGroupStone_Init(assets, &stone_mat, LAB_COLOR_BLACK, LAB_COLOR_WHITE);
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_BASALT,     LAB_RGB(80, 70, 60));
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_STONE,      LAB_RGB(180, 180, 170));
        LAB_MaterialGroupStone mat2;
        LAB_MaterialGroupStone_Init(assets, &mat2, LAB_RGBX(aa9988), LAB_COLOR_WHITE);
        LAB_BlockGroupStone_Init(assets, &mat2, &LAB_BLOCK_MARBLE,     LAB_RGB(255, 255, 255));
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_CLAY,       LAB_RGB(200, 80, 50));
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_LAPIZ,      LAB_RGB(30, 80, 200));
        LAB_MaterialGroupStone_Init(assets, &mat2, LAB_RGBX(896400), LAB_RGB(255, 230+10, 130+60));
        LAB_BlockGroupStone_Init(assets, &mat2, &LAB_BLOCK_SANDSTONE,  LAB_COLOR_WHITE);// LAB_RGB(255, 230, 150));
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_GRANITE,  LAB_RGB(180, 90, 90));// LAB_RGB(255, 230, 150));
    }




    ///// ORES /////
    {
        {
            size_t tex[2][2];
            LAB_TextureComposite c[] = {
                { "stone", 0,  LAB_RGB(180, 180, 170) },
                { "ore_overlay", LAB_RGBX(bb6655), LAB_RGBX(cc8866) },
                {0},
            };
            LAB_Assets_NewComposedTexture(assets, tex, c);
            LAB_BlockFull_Init(assets, &LAB_BLOCK_IRON_ORE, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
        }
        {
            size_t tex[2][2];
            LAB_TextureComposite c[] = {
                { "stone", 0,  LAB_RGB(180, 180, 170) },
                { "ore_overlay", LAB_RGBX(cc6622), LAB_RGBX(ddaa55) },
                {0},
            };
            LAB_Assets_NewComposedTexture(assets, tex, c);
            LAB_BlockFull_Init(assets, &LAB_BLOCK_COPPER_ORE, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
        }
        {
            size_t tex[2][2];
            LAB_TextureComposite c[] = {
                { "stone", 0,  LAB_RGB(180, 180, 170) },
                { "ore_overlay", LAB_RGBX(aa8822), LAB_RGBX(eecc99) }, // LAB_RGBX(bb9955), LAB_RGBX(ccbb66) },
                {0},
            };
            LAB_Assets_NewComposedTexture(assets, tex, c);
            LAB_BlockFull_Init(assets, &LAB_BLOCK_GOLD_ORE, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
        }
        {
            size_t tex[2][2];
            LAB_TextureComposite c[] = {
                { "stone", 0,  LAB_RGB(180, 180, 170) },
                { "ore_overlay", LAB_RGBX(8899bb), LAB_RGBX(ffffff) },
                {0},
            };
            LAB_Assets_NewComposedTexture(assets, tex, c);
            LAB_BlockFull_Init(assets, &LAB_BLOCK_SILVER_ORE, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
        }
        {
            size_t tex[2][2];
            LAB_TextureComposite c[] = {
                { "stone", 0,  LAB_RGB(180, 180, 170) },
                { "ore_overlay", LAB_RGBX(55dd22), LAB_RGBX(ffff33) },
                {0},
            };
            LAB_Assets_NewComposedTexture(assets, tex, c);
            LAB_BlockFull_Init(assets, &LAB_BLOCK_URANIUM_ORE, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
        }
        {
            size_t tex[2][2];
            LAB_TextureComposite c[] = {
                { "layered_stone", 0,  LAB_RGB(180, 180, 170) },
                { "coal_ore_overlay", LAB_RGBX(000000), LAB_RGBX(222222) },
                {0},
            };
            LAB_Assets_NewComposedTexture(assets, tex, c);
            LAB_BlockFull_Init(assets, &LAB_BLOCK_COAL_ORE, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
        }
    }




    ///// SOIL TYPES /////
    {
        size_t tex[2][2];
        LAB_TextureComposite c[] = { {"sand", LAB_RGBX(896400), LAB_RGB(255, 230+10, 130+60) }, {0} };
        LAB_Assets_NewComposedTexture(assets, tex, c);
        LAB_BlockFull_Init(assets, &LAB_BLOCK_SAND, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
    }




    ///// GLASS TYPES /////
    {
        size_t tex[2][2];
        LAB_Assets_NewTexture(assets, tex, "glass");
        LAB_BlockFull_Init(assets, &LAB_BLOCK_GLASS, tex, LAB_RGBA(255, 255, 255, 128), LAB_RENDER_PASS_ALPHA);
        LAB_BLOCK_GLASS.flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_OPAQUE_SELF;
        LAB_BLOCK_GLASS.dia   = LAB_COLOR_WHITE;
    }




    ///// TREE TYPES /////
    {
        size_t tex_leaves[2][2];
        size_t tex_oak_log[2][2];
        size_t tex_birch_log[2][2];
        size_t tex_planks[2][2];
        LAB_Assets_NewTexture(assets, tex_leaves,    "leaves");
        LAB_Assets_NewTexture(assets, tex_oak_log,   "oak_log");
        LAB_Assets_NewTexture(assets, tex_birch_log, "birch_log");
        LAB_Assets_NewTexture(assets, tex_planks,    "planks");


        ///// OAK /////
        LAB_BlockFull_Init(assets, &LAB_BLOCK_LEAVES, tex_leaves, LAB_COLOR_LEAVES, LAB_RENDER_PASS_MASKED);
        LAB_BLOCK_LEAVES.dia = LAB_RGB(130, 200, 130);
        LAB_BLOCK_LEAVES.flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL;

        LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD, tex_oak_log, LAB_COLOR_WOOD, LAB_RENDER_PASS_SOLID);
        LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD_PLANKS, tex_planks, LAB_RGB(220, 197, 162), LAB_RENDER_PASS_SOLID);
        LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD_PLANKS_DARK, tex_planks, LAB_RGB(120, 97, 80), LAB_RENDER_PASS_SOLID);


        ///// BIRCH /////
        LAB_BlockFull_Init(assets, &LAB_BLOCK_BIRCH_LEAVES, tex_leaves, LAB_COLOR_BIRCH_LEAVES, LAB_RENDER_PASS_MASKED);
        LAB_BLOCK_BIRCH_LEAVES.dia = LAB_COLOR_BIRCH_LEAVES;
        LAB_BLOCK_BIRCH_LEAVES.flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL;

        LAB_BlockFull_Init(assets, &LAB_BLOCK_BIRCH_WOOD, tex_birch_log, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);

        
        ///// SPRUCE /////
        LAB_BlockFull_Init(assets, &LAB_BLOCK_SPRUCE_LEAVES, tex_leaves, LAB_RGBX(006622), LAB_RENDER_PASS_MASKED);
        LAB_BLOCK_SPRUCE_LEAVES.dia = LAB_RGBX(006622);
        LAB_BLOCK_SPRUCE_LEAVES.flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL;

        LAB_BlockFull_Init(assets, &LAB_BLOCK_SPRUCE_WOOD, tex_birch_log, LAB_RGBX(332211), LAB_RENDER_PASS_SOLID); // LAB_RGBX(664422)
    }





    ///// FOILAGE /////
    {
        size_t tex[2][2];

        LAB_Assets_NewTexture(assets, tex, "tall_grass");
        LAB_BlockCross_Init(assets, &LAB_BLOCK_TALLGRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_MASKED);
        LAB_BLOCK_TALLGRASS.dia = LAB_RGBX(dddddd);
        LAB_AABB3_Assign(LAB_BLOCK_TALLGRASS.bounds,   0.125, 0, 0.125,   0.875, 0.375, 0.875);

        LAB_Assets_NewTexture(assets, tex, "taller_grass");
        LAB_BlockCross_Init(assets, &LAB_BLOCK_TALLERGRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_MASKED);
        LAB_BLOCK_TALLERGRASS.dia = LAB_RGBX(dddddd);
        LAB_AABB3_Assign(LAB_BLOCK_TALLERGRASS.bounds,   0.125, 0, 0.125,   0.875, 0.75, 0.875);



        LAB_Assets_NewTexture(assets, tex, "grass");
        LAB_BlockFull_Init(assets, &LAB_BLOCK_GRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_SOLID);

        LAB_Assets_NewTexture(assets, tex, "dirt");
        LAB_BlockFull_Init(assets, &LAB_BLOCK_DIRT, tex, LAB_RGB(120, 80, 50), LAB_RENDER_PASS_SOLID);



        static const LAB_TextureComposite red_tulip[] = {
            { "tulip_stem",                 0, LAB_COLOR_GRASS  },
            { "tulip_bloom", LAB_RGBX(400020), LAB_RGBX(ff2010) },
            {0}
        };
        LAB_Assets_NewComposedTexture(assets, tex, red_tulip);
        LAB_BlockCross_Init(assets, &LAB_BLOCK_RED_TULIP, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_MASKED);
        LAB_BLOCK_RED_TULIP.dia = LAB_RGBX(dddddd);
        LAB_AABB3_Assign(LAB_BLOCK_RED_TULIP.bounds,   0.25, 0, 0.25,   0.75, 0.875, 0.75);

        
        
        static const LAB_TextureComposite yellow_tulip[] = {
            { "tulip_stem",                 0, LAB_COLOR_GRASS  },
            { "tulip_bloom", LAB_RGBX(805000), LAB_RGBX(ffff00) },
            {0}
        };
        LAB_Assets_NewComposedTexture(assets, tex, yellow_tulip);
        LAB_BlockCross_Init(assets, &LAB_BLOCK_YELLOW_TULIP, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_MASKED);
        LAB_BLOCK_YELLOW_TULIP.dia = LAB_RGBX(dddddd);
        LAB_AABB3_Assign(LAB_BLOCK_YELLOW_TULIP.bounds,   0.25, 0, 0.25,   0.75, 0.875, 0.75);



        {
            LAB_Assets_NewTexture(assets, tex, "fallen_leaves");

            float vert[4][5] = {
                { 0, 0.0625, 0,   tex[0][0], tex[0][1] },
                { 1, 0.0625, 0,   tex[1][0], tex[0][1] },
                { 0, 0.0625, 1,   tex[0][0], tex[1][1] },
                { 1, 0.0625, 1,   tex[1][0], tex[1][1] },
            };
            LAB_Model* m = LAB_Assets_NewModel(assets);
            LAB_ASSERT_OR_ABORT(m);
            m->render_pass = LAB_RENDER_PASS_MASKED;
            LAB_Builtin_AddQuad(m, (const float(*)[5])vert, LAB_COLOR_BIRCH_LEAVES, LAB_DIR_ALL, 0, LAB_DIR_UP);
            
            LAB_BLOCK_FALLEN_LEAVES.flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE);
            LAB_BLOCK_FALLEN_LEAVES.dia = LAB_RGBX(ffffff);
            LAB_BLOCK_FALLEN_LEAVES.model = m;
            LAB_BLOCK_FALLEN_LEAVES.item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_BIRCH_LEAVES);
            LAB_AABB3_Assign(LAB_BLOCK_FALLEN_LEAVES.bounds,   0, 0, 0,   1, 0.0625, 1);

            LAB_RegisterBlock(&LAB_BLOCK_FALLEN_LEAVES);
        }
    }


    ///// METAL BLOCKS /////
    {
        size_t tex[2][2];
        
        LAB_Assets_NewTexture(assets, tex, "metal");
        LAB_BlockFull_Init(assets, &LAB_BLOCK_METAL, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
    }

    ///// FLUIDS /////
    {
        size_t tex[2][2];
        LAB_Assets_NewTintedTexture(assets, tex, "water", 0, LAB_COLOR_WATER);
        LAB_BlockFull_Init(assets, &LAB_BLOCK_WATER, tex, LAB_RGBA(255, 255, 255, 255), LAB_RENDER_PASS_ALPHA);
        LAB_BLOCK_WATER.flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_OPAQUE_SELF;
        LAB_BLOCK_WATER.tags  = LAB_BLOCK_TAG_REPLACEABLE;
        LAB_BLOCK_WATER.dia   = LAB_COLOR_WATER_DIA;

    }


    ///// MISC /////
    {
        size_t tex[2][2];
        {
            static const LAB_TextureComposite c[] = {
                { "torch_stick", 0, LAB_RGB(120, 97, 80) },
                { "torch_flame", 0, LAB_RGBX(ffffff) },
                {0}
            };
            LAB_Assets_NewComposedTexture(assets, tex, c);
            
            #define X0 (7/16.)
            #define X1 (9/16.)
            #define Y1 (10/16.)
            static const float aabb[2][3] = { {X0, 0, X0}, {X1, Y1, X1} };

            // TODO MOVE
            #define LAB_TEXCUT(dst, tex, u0, v0, u1, v1) do \
            { \
                (dst)[0][0] = (tex)[0][0] + ((tex)[1][0] - (tex)[0][0])*(u0); \
                (dst)[0][1] = (tex)[0][1] + ((tex)[1][1] - (tex)[0][1])*(v0); \
                (dst)[1][0] = (tex)[0][0] + ((tex)[1][0] - (tex)[0][0])*(u1); \
                (dst)[1][1] = (tex)[0][1] + ((tex)[1][1] - (tex)[0][1])*(v1); \
            } \
            while(0)

            float uv[6][2][2];
            LAB_TEXCUT(uv[0], tex, X0, 1-Y1, X1, 1);
            LAB_TEXCUT(uv[1], tex, X0, 1-Y1, X1, 1);
            LAB_TEXCUT(uv[2], tex, X0, (28/32.), X1, (32/32.));
            LAB_TEXCUT(uv[3], tex, X0, (11/32.), X1, (15/32.));
            LAB_TEXCUT(uv[4], tex, X0, 1-Y1, X1, 1);
            LAB_TEXCUT(uv[5], tex, X0, 1-Y1, X1, 1);

            LAB_Model* m = LAB_Assets_NewModel(assets);
            LAB_ASSERT_OR_ABORT(m);
            m->render_pass = LAB_RENDER_PASS_MASKED;
            LAB_Builtin_ModelAddCube(m, aabb, (const float (*)[2][2])uv, LAB_cube_color_flat, LAB_COLOR_WHITE);

            LAB_BLOCK_TORCH = (LAB_Block) {
                .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE|LAB_BLOCK_GLOWING|LAB_BLOCK_NOSHADE),
                .dia = LAB_RGB(255, 255, 255),
                //.lum = LAB_RGB(220, 210, 180),
                .lum = LAB_RGB(110, 105, 90),
                .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_WHITE),
                .model = m,
                //.bounds = {{0.375, 0, 0.375}, {0.625, 0.75, 0.625}},
                .bounds = {{7/16., 0, 7/16.}, {9/16., 10/16., 9/16.}}, // aabb
            };
            LAB_RegisterBlock(&LAB_BLOCK_TORCH);
        }
    }

    ///// LIGHT BLOCKS /////
    {
        size_t tex[2][2];


        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGBX(bbddff), LAB_RGBX(ffffff)); // LAB_RGBX(86bedd)
        LAB_BlockLight_Init(assets, &LAB_BLOCK_LIGHT, tex, LAB_RGBX(ffffff));

        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGBX(ffccaa), LAB_RGBX(ffffff)); // LAB_RGBX(ddbe86)
        LAB_BlockLight_Init(assets, &LAB_BLOCK_WARM_LIGHT, tex, LAB_RGB(255, 255, 220));

        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGBX(80ccff), LAB_RGBX(e0f0ff)); // LAB_RGBX(ddbe86)
        LAB_BlockLight_Init(assets, &LAB_BLOCK_COLD_LIGHT, tex, LAB_RGB(200, 230, 255));

        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGB(16, 64, 255), LAB_RGBX(a0c0ff));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_BLUE_LIGHT, tex, LAB_RGB(16, 64, 255));

        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGB(16, 128, 255), LAB_RGBX(e0ffff));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_CYAN_LIGHT, tex, LAB_RGB(16, 128, 255));

        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGB(255, 240, 16), LAB_RGBX(ffffc0));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_YELLOW_LIGHT, tex, LAB_RGB(255, 240, 16));

        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGB(64, 255, 16), LAB_RGBX(ffffc0));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_GREEN_LIGHT, tex, LAB_RGB(64, 255, 16));

        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGBX(ff0000), LAB_RGBX(ffc0a0));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_RED_LIGHT, tex, LAB_RGB(255, 20, 16));

        LAB_Assets_NewTintedTexture(assets, tex, "light", LAB_RGB(255, 128, 16), LAB_RGBX(ffe0c0));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_ORANGE_LIGHT, tex, LAB_RGB(255, 128, 16));




        LAB_Assets_NewTintedTexture(assets, tex, "crystal", LAB_RGB(32, 32, 255), LAB_RGBX(2040ff));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_BLUE_CRYSTAL, tex, LAB_RGB(16/2, 64/2, 255/2));

        LAB_Assets_NewTintedTexture(assets, tex, "crystal", LAB_RGB(255, 240, 32), LAB_RGBX(ffff40));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_YELLOW_CRYSTAL, tex, LAB_RGB(255/2, 240/2, 16/2));

        LAB_Assets_NewTintedTexture(assets, tex, "crystal", LAB_RGB(40, 255, 32), LAB_RGBX(80ff40));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_GREEN_CRYSTAL, tex, LAB_RGB(64/2, 255/2, 16/2));

        LAB_Assets_NewTintedTexture(assets, tex, "crystal", LAB_RGB(255, 25, 15), LAB_RGBX(ff4020));
        LAB_BlockLight_Init(assets, &LAB_BLOCK_RED_CRYSTAL, tex, LAB_RGB(255/2, 20/2, 16/2));
    }

    ///// SPECIAL BLOCKS /////
    {
        size_t tex[2][2];

        LAB_Assets_NewTexture(assets, tex, "light_item");
        LAB_BLOCK_INVISIBLE_LIGHT = (LAB_Block){
            .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_EMISSIVE,
            .lum = LAB_RGB(255, 255, 255),
            .dia = LAB_RGB(255, 255, 255),
            .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_WHITE),
            .bounds = LAB_AABB_FULL_CUBE,
        };
        LAB_RegisterBlock(&LAB_BLOCK_INVISIBLE_LIGHT);

        LAB_Assets_NewTexture(assets, tex, "barrier_item");
        LAB_BLOCK_BARRIER = (LAB_Block){
            .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_MASSIVE,
            .dia = LAB_RGB(255, 255, 255),
            .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_WHITE),
            .bounds = LAB_AABB_FULL_CUBE,
        };
        LAB_RegisterBlock(&LAB_BLOCK_BARRIER);
    }

    return true;
}