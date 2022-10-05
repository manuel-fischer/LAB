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
#define LAB_COLOR_WATER_DIA LAB_RGBX(ccddff)


// TODO move
LAB_INLINE
bool LAB_InitBlock(LAB_BlockID* id, LAB_Block b)
{
    if(!LAB_RegisterBlocksGen(id, 1)) return false;
    *LAB_BlockP(*id) = b;
    return true;
}

LAB_INLINE
bool LAB_InitItemBlock(LAB_BlockID* id, LAB_Block b)
{
    if(!LAB_RegisterBlocksGen(id, 1)) return false;
    *LAB_BlockP(*id) = b;
    return LAB_AddBlockItems(*id, 1);
}


LAB_INLINE
bool LAB_LeavesBlock_Init(LAB_Assets* assets, LAB_BlockID* bid,
                          size_t tex[2][2], LAB_Color tint, LAB_RenderPass render_pass, LAB_Color dia)
{
    if(!LAB_BlockFull_Init(assets, bid, tex, tint, render_pass)) return false;
    LAB_BlockP(*bid)->dia = dia;
    LAB_BlockP(*bid)->flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL;
    return true;
}





bool LAB_BuiltinBlocks_Init(LAB_Assets* assets)
{
    // TODO error checking
    ///// FUNDAMENTAL /////
    {
        if(!LAB_InitBlock(&LAB_BLOCK_AIR, (LAB_Block) {
            .flags = 0,
            .tags = LAB_BLOCK_TAG_REPLACEABLE,
            .dia = LAB_RGB(255, 255, 255)
        })) return false;

        if(!LAB_InitBlock(&LAB_BLOCK_OUTSIDE, (LAB_Block) {
            .flags = LAB_BLOCK_OPAQUE,
            .dia = LAB_RGB(255, 255, 255)
        })) return false;

        LAB_ASSERT(LAB_BLOCK_AIR == LAB_BID_AIR);
        LAB_ASSERT(LAB_BLOCK_OUTSIDE == LAB_BID_OUTSIDE);
    }
    




    ///// ROCK TYPES /////
    {
        static const struct { LAB_BlockGroupStone* ids; LAB_Color tint[2]; bool lightup; }
        rocks[] =
        {
            { &LAB_BLOCK_BASALT,    { LAB_RGBX(080706), LAB_RGBX(50463c) }, 0 },
            { &LAB_BLOCK_STONE,     { LAB_RGBX(000000), LAB_RGBX(b4b4aa) }, 1 },
            { &LAB_BLOCK_MARBLE,    { LAB_RGBX(aa9988), LAB_RGBX(ffffff) }, 1 },
            { &LAB_BLOCK_CLAY,      { LAB_RGBX(402010), LAB_RGBX(c85032) }, 0 },
            { &LAB_BLOCK_LAPIZ,     { LAB_RGBX(100040), LAB_RGBX(1e50c8) }, 0 },
            { &LAB_BLOCK_SANDSTONE, { LAB_RGBX(896400), LAB_RGBX(fff0be) }, 1 },
            { &LAB_BLOCK_GRANITE,   { LAB_RGBX(402030), LAB_RGBX(b45a5a) }, 0 },
        };

        for(size_t i = 0; i < LAB_LEN(rocks); ++i)
        {
            LAB_Color tint0 = rocks[i].tint[0];
            LAB_Color tint1 = rocks[i].tint[1];
            if(rocks[i].lightup) tint0 = LAB_MixColor50(tint0, tint1);

            LAB_MaterialGroupStone mat;
            LAB_MaterialGroupStone_Init(assets, &mat, tint0, tint1);
            LAB_BlockGroupStone_Init(assets, &mat, rocks[i].ids, LAB_COLOR_WHITE);
        }

        goto jump;

        LAB_MaterialGroupStone stone_mat;
        LAB_MaterialGroupStone mat2;

        LAB_MaterialGroupStone_Init(assets, &stone_mat, LAB_COLOR_BLACK, LAB_COLOR_WHITE);
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_BASALT,     LAB_RGB(80, 70, 60));
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_STONE,      LAB_RGBX(b4b4aa));



        LAB_MaterialGroupStone_Init(assets, &mat2, LAB_RGBX(aa9988), LAB_COLOR_WHITE);
        LAB_BlockGroupStone_Init(assets, &mat2, &LAB_BLOCK_MARBLE,     LAB_RGB(255, 255, 255));

        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_CLAY,       LAB_RGB(200, 80, 50));
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_LAPIZ,      LAB_RGB(30, 80, 200));
        LAB_MaterialGroupStone_Init(assets, &mat2, LAB_RGBX(896400), LAB_RGB(255, 230+10, 130+60));
        LAB_BlockGroupStone_Init(assets, &mat2, &LAB_BLOCK_SANDSTONE,  LAB_COLOR_WHITE);// LAB_RGB(255, 230, 150));
        LAB_BlockGroupStone_Init(assets, &stone_mat, &LAB_BLOCK_GRANITE,  LAB_RGB(180, 90, 90));// LAB_RGB(255, 230, 150));
        jump:;
    }




    ///// ORES /////
    {
        static const struct { LAB_BlockID* id; LAB_Color ore_tint[2]; }
        ores[] =
        {
            { &LAB_BLOCK_IRON_ORE,    { LAB_RGBX(bb6655), LAB_RGBX(cc8866) } },
            { &LAB_BLOCK_COPPER_ORE,  { LAB_RGBX(cc6622), LAB_RGBX(ddaa55) } },
            { &LAB_BLOCK_GOLD_ORE,    { LAB_RGBX(aa8822), LAB_RGBX(eecc99) } },
            { &LAB_BLOCK_SILVER_ORE,  { LAB_RGBX(8899bb), LAB_RGBX(ffffff) } },
            { &LAB_BLOCK_URANIUM_ORE, { LAB_RGBX(55dd22), LAB_RGBX(ffff33) } },
        };

        for(size_t i = 0; i < LAB_LEN(ores); ++i)
        {
            size_t tex[2][2];
            LAB_TextureComposite c[] = {
                { "stone", LAB_RGBX(5a5a55),  LAB_RGBX(b4b4aa) },
                { "ore_overlay", ores[i].ore_tint[0], ores[i].ore_tint[1] },
                {0},
            };
            LAB_Assets_NewComposedTexture(assets, tex, c);
            LAB_BlockFull_Init(assets, ores[i].id, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
        }

        {
            size_t tex[2][2];
            LAB_TextureComposite c[] = {
                { "layered_stone", LAB_RGBX(5a5a55),  LAB_RGBX(b4b4aa) },
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
        LAB_BlockP(LAB_BLOCK_GLASS)->flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_OPAQUE_SELF;
        LAB_BlockP(LAB_BLOCK_GLASS)->dia   = LAB_COLOR_WHITE;
    }




    ///// TREE TYPES /////
    {
        size_t tex_leaves[2][2];
        size_t tex_needle_leaves[2][2];
        size_t tex_oak_log[2][2];
        size_t tex_birch_log[2][2];
        size_t tex_planks[2][2];
        LAB_Assets_NewTexture(assets, tex_leaves,        "leaves");
        LAB_Assets_NewTexture(assets, tex_needle_leaves, "needle_leaves");
        LAB_Assets_NewTexture(assets, tex_oak_log,       "oak_log");
        LAB_Assets_NewTexture(assets, tex_birch_log,     "birch_log");
        LAB_Assets_NewTexture(assets, tex_planks,        "planks");


        ///// OAK /////
        LAB_LeavesBlock_Init(assets, &LAB_BLOCK_LEAVES, tex_leaves, LAB_COLOR_LEAVES, LAB_RENDER_PASS_MASKED, LAB_RGB(130, 200, 130));
        LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD, tex_oak_log, LAB_COLOR_WOOD, LAB_RENDER_PASS_SOLID);
        LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD_PLANKS, tex_planks, LAB_RGB(220, 197, 162), LAB_RENDER_PASS_SOLID);
        LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD_PLANKS_DARK, tex_planks, LAB_RGB(120, 97, 80), LAB_RENDER_PASS_SOLID);


        ///// BIRCH /////
        LAB_LeavesBlock_Init(assets, &LAB_BLOCK_BIRCH_LEAVES, tex_leaves, LAB_COLOR_BIRCH_LEAVES, LAB_RENDER_PASS_MASKED, LAB_COLOR_BIRCH_LEAVES);
        LAB_BlockFull_Init(assets, &LAB_BLOCK_BIRCH_WOOD, tex_birch_log, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);

        
        ///// SPRUCE /////
        LAB_LeavesBlock_Init(assets, &LAB_BLOCK_SPRUCE_LEAVES, tex_needle_leaves, LAB_RGBX(006622), LAB_RENDER_PASS_MASKED, LAB_RGBX(006622));
        LAB_BlockFull_Init(assets, &LAB_BLOCK_SPRUCE_WOOD, tex_birch_log, LAB_RGBX(332211), LAB_RENDER_PASS_SOLID); // LAB_RGBX(664422)
    }





    ///// FOILAGE /////
    {
        size_t tex[2][2];

        LAB_Assets_NewTexture(assets, tex, "tall_grass");
        LAB_BlockCross_Init(assets, &LAB_BLOCK_TALLGRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_MASKED);
        LAB_BlockP(LAB_BLOCK_TALLGRASS)->dia = LAB_RGBX(dddddd);
        LAB_AABB3_Assign(LAB_BlockP(LAB_BLOCK_TALLGRASS)->bounds,   0.125, 0, 0.125,   0.875, 0.375, 0.875);

        LAB_Assets_NewTexture(assets, tex, "taller_grass");
        LAB_BlockCross_Init(assets, &LAB_BLOCK_TALLERGRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_MASKED);
        LAB_BlockP(LAB_BLOCK_TALLERGRASS)->dia = LAB_RGBX(dddddd);
        LAB_AABB3_Assign(LAB_BlockP(LAB_BLOCK_TALLERGRASS)->bounds,   0.125, 0, 0.125,   0.875, 0.75, 0.875);



        LAB_Assets_NewTexture(assets, tex, "grass");
        LAB_BlockFull_Init(assets, &LAB_BLOCK_GRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_SOLID);

        LAB_Assets_NewTexture(assets, tex, "dirt");
        LAB_BlockFull_Init(assets, &LAB_BLOCK_DIRT, tex, LAB_RGB(120, 80, 50), LAB_RENDER_PASS_SOLID);



        static const struct { LAB_BlockID* id; LAB_Color bloom_colors[2]; }
        tulips[] =
        {
            { &LAB_BLOCK_RED_TULIP,    { LAB_RGBX(400020), LAB_RGBX(ff2010) } },
            { &LAB_BLOCK_ORANGE_TULIP, { LAB_RGBX(603000), LAB_RGBX(ff8020) } },
            { &LAB_BLOCK_YELLOW_TULIP, { LAB_RGBX(805000), LAB_RGBX(ffff00) } },
            { &LAB_BLOCK_BLUE_TULIP,   { LAB_RGBX(002080), LAB_RGBX(4080ff) } },
            { &LAB_BLOCK_WHITE_TULIP,  { LAB_RGBX(402080), LAB_RGBX(eeddff) } },
        };

        for(size_t i = 0; i < LAB_LEN(tulips); ++i)
        {
            LAB_Assets_NewComposedTexture(assets, tex, (const LAB_TextureComposite[])
            {
                { "tulip_stem",  0, LAB_COLOR_GRASS  },
                { "tulip_bloom", tulips[i].bloom_colors[0], tulips[i].bloom_colors[1] },
                {0}
            });
            LAB_BlockCross_Init(assets, tulips[i].id, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_MASKED);
            LAB_BlockP(*tulips[i].id)->dia = LAB_RGBX(dddddd);
            LAB_AABB3_Assign(LAB_BlockP(*tulips[i].id)->bounds,   0.25, 0, 0.25,   0.75, 0.875, 0.75);
        }


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
            
            if(!LAB_InitItemBlock(&LAB_BLOCK_FALLEN_LEAVES, (LAB_Block) {
                .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
                .dia = LAB_RGBX(ffffff),
                .model = m,
                .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_BIRCH_LEAVES),
                .bounds = {{0, 0, 0}, {1, 0.0625, 1}},
            })) return false;
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
        LAB_Color tint = LAB_COLOR_WHITE;

        const float tex_f[2][2] = { { tex[0][0], tex[0][1] }, { tex[1][0], tex[1][1] } };

        LAB_Model* m = LAB_Assets_NewModel(assets);
        if(!m) return false;
        m->render_pass = LAB_RENDER_PASS_ALPHA;
        LAB_Builtin_ModelAddCubeAll(m,
            LAB_full_aabb,
            tex_f, LAB_cube_color_shade, tint);
        LAB_Builtin_ModelAddCubeInvertedAll(m,
            LAB_full_aabb,
            tex_f, LAB_cube_color_shade, tint);


        if(!LAB_InitItemBlock(&LAB_BLOCK_WATER, (LAB_Block) {
            .model = m,
            .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, tint),
            .flags = LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_OPAQUE_SELF,
            .tags  = LAB_BLOCK_TAG_REPLACEABLE,
            .dia   = LAB_COLOR_WATER_DIA
        })) return false;
    }


    ///// MISC /////
    {
        size_t tex[2][2];
        {
            LAB_Assets_NewComposedTexture(assets, tex, (const LAB_TextureComposite[])
            {
                { "torch_stick", 0, LAB_RGB(120, 97, 80) },
                { "torch_flame", 0, LAB_RGBX(ffffff) },
                {0}
            });
            
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

            if(!LAB_InitItemBlock(&LAB_BLOCK_TORCH, (LAB_Block) {
                .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE|LAB_BLOCK_GLOWING|LAB_BLOCK_NOSHADE),
                .dia = LAB_RGB(255, 255, 255),
                //.lum = LAB_RGB(220, 210, 180),
                .lum = LAB_RGB(110, 105, 90),
                .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_WHITE),
                .model = m,
                //.bounds = {{0.375, 0, 0.375}, {0.625, 0.75, 0.625}},
                .bounds = {{7/16., 0, 7/16.}, {9/16., 10/16., 9/16.}}, // aabb
            })) return false;
        }
    }

    ///// LIGHT BLOCKS /////
    {
        static const struct { LAB_BlockID* id; LAB_Color lum, tint[2]; }
        lights[] =
        {
            { &LAB_BLOCK_LIGHT,        LAB_RGBX(ffffff), { LAB_RGBX(bbddff), LAB_RGBX(ffffff) } }, // LAB_RGBX(86bedd)
            { &LAB_BLOCK_WARM_LIGHT,   LAB_RGBX(ffffdc), { LAB_RGBX(ffccaa), LAB_RGBX(ffffff) } }, // LAB_RGBX(ddbe86)
            { &LAB_BLOCK_COLD_LIGHT,   LAB_RGBX(c8e6ff), { LAB_RGBX(80ccff), LAB_RGBX(f0f8ff) } },
            { &LAB_BLOCK_BLUE_LIGHT,   LAB_RGBX(1040ff), { LAB_RGBX(1040ff), LAB_RGBX(a8d0ff) } },
            { &LAB_BLOCK_CYAN_LIGHT,   LAB_RGBX(1080ff), { LAB_RGBX(1080ff), LAB_RGBX(e0ffff) } },
            { &LAB_BLOCK_YELLOW_LIGHT, LAB_RGBX(fff010), { LAB_RGBX(fff010), LAB_RGBX(ffffc0) } },
            { &LAB_BLOCK_GREEN_LIGHT,  LAB_RGBX(40ff10), { LAB_RGBX(40ff10), LAB_RGBX(f0ffc0) } },
            { &LAB_BLOCK_RED_LIGHT,    LAB_RGBX(ff1410), { LAB_RGBX(ff0000), LAB_RGBX(ffc0a0) } },
            { &LAB_BLOCK_ORANGE_LIGHT, LAB_RGBX(ff8010), { LAB_RGBX(ff8010), LAB_RGBX(ffe0c0) } },
        };

        for(size_t i = 0; i < LAB_LEN(lights); ++i)
        {
            size_t tex[2][2];
            LAB_Assets_NewTintedTexture(assets, tex, "light_modern", lights[i].tint[0], lights[i].tint[1]);
            LAB_BlockLight_Init(assets, lights[i].id, tex, lights[i].lum);
        }


        static const struct { LAB_BlockID* id; LAB_Color lum, tint[2]; }
        crystals[] =
        {
            { &LAB_BLOCK_BLUE_CRYSTAL,   LAB_RGBX(082080), { LAB_RGBX(2020ff), LAB_RGBX(2040ff) } },
            { &LAB_BLOCK_YELLOW_CRYSTAL, LAB_RGBX(807808), { LAB_RGBX(fff010), LAB_RGBX(ffff40) } },
            { &LAB_BLOCK_GREEN_CRYSTAL,  LAB_RGBX(208008), { LAB_RGBX(28ff20), LAB_RGBX(80ff40) } },
            { &LAB_BLOCK_RED_CRYSTAL,    LAB_RGBX(ff0a08), { LAB_RGBX(ff190f), LAB_RGBX(ff4020) } },
        };
        const LAB_Color crystal_shade = LAB_RGBX(eeeeee);
        const LAB_Color crystal_light = LAB_RGBX(222222);

        for(size_t i = 0; i < LAB_LEN(crystals); ++i)
        {
            size_t tex[2][2];
            LAB_Color tint0 = LAB_MulColor(crystals[i].tint[0], crystal_shade);
            LAB_Color tint1 = LAB_MulColorInv(crystals[i].tint[1], crystal_light);
            LAB_Assets_NewTintedTexture(assets, tex, "crystal", tint0, tint1);
            LAB_BlockLight_Init(assets, crystals[i].id, tex, crystals[i].lum);
        }
    }

    ///// SPECIAL BLOCKS /////
    {
        size_t tex[2][2];

        LAB_Assets_NewTexture(assets, tex, "light_item");
        if(!LAB_InitItemBlock(&LAB_BLOCK_INVISIBLE_LIGHT, (LAB_Block) {
            .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_EMISSIVE,
            .lum = LAB_RGB(255, 255, 255),
            .dia = LAB_RGB(255, 255, 255),
            .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_WHITE),
            .bounds = LAB_AABB_FULL_CUBE,
        })) return false;

        LAB_Assets_NewTexture(assets, tex, "barrier_item");
        if(!LAB_InitItemBlock(&LAB_BLOCK_BARRIER, (LAB_Block) {
            .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_MASSIVE,
            .dia = LAB_RGB(255, 255, 255),
            .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_WHITE),
            .bounds = LAB_AABB_FULL_CUBE,
        })) return false;
    }

    return true;
}