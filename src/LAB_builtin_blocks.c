#include "LAB_builtin_blocks.h"
#include "LAB_blocks.h"

#include "LAB_color_defs.h"
#include "LAB_ptr.h"
#include "LAB_vec2.h"
#include "LAB_vec3.h"


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
LAB_Err LAB_InitBlock(LAB_BlockID* id, LAB_Block b)
{
    LAB_TRY(LAB_RegisterBlocksGen(id, 1));
    *LAB_BlockP(*id) = b;
    return LAB_OK;
}

LAB_INLINE
LAB_Err LAB_InitItemBlock(LAB_BlockID* id, LAB_Block b)
{
    LAB_TRY(LAB_RegisterBlocksGen(id, 1));
    *LAB_BlockP(*id) = b;
    LAB_TRY(LAB_AddBlockItems(*id, 1));
    return LAB_OK;
}


LAB_INLINE
LAB_Err LAB_LeavesBlock_Init(LAB_Assets* assets, LAB_BlockID* bid,
                             LAB_Box2Z tex, LAB_Color tint, LAB_RenderPass render_pass, LAB_Color dia)
{
    LAB_TRY(LAB_BlockFull_Init(assets, bid, tex, tint, render_pass));
    LAB_BlockP(*bid)->dia = dia;
    LAB_BlockP(*bid)->flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL;
    return LAB_OK;
}

LAB_INLINE
LAB_Err LAB_FallenLeavesBlock_Init(LAB_Assets* assets, LAB_BlockID* bid,
                                   LAB_Box2Z tex, LAB_Color tint)
{
    const float vert[4][5] = {
        { 0, 0.0625, 0,   tex.a.x, tex.a.y },
        { 1, 0.0625, 0,   tex.b.x, tex.a.y },
        { 0, 0.0625, 1,   tex.a.x, tex.b.y },
        { 1, 0.0625, 1,   tex.b.x, tex.b.y },
    };
    LAB_Model* m = LAB_Assets_NewModel(assets);
    LAB_TRY(assets->err);

    m->render_pass = LAB_RENDER_PASS_MASKED;
    LAB_Builtin_AddQuad(m, vert, tint, LAB_DIR_ALL, 0, LAB_DIR_UP);

    LAB_TRY(LAB_InitItemBlock(bid, (LAB_Block) {
        .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE),
        .dia = LAB_RGBX(ffffff),
        .model = m,
        .item_texture = LAB_Assets_RenderItem(assets, tex, tint),
        .bounds = {{0, 0, 0}, {1, 0.0625, 1}},
    }));

    return LAB_OK;
}



LAB_Err LAB_BuiltinBlocks_Init(LAB_Assets* assets)
{
    // TODO error checking

    ///// FUNDAMENTAL /////
    {
        LAB_TRY(LAB_InitBlock(&LAB_BLOCK_AIR, (LAB_Block) {
            .flags = 0,
            .tags = LAB_BLOCK_TAG_REPLACEABLE,
            .dia = LAB_RGB(255, 255, 255)
        }));

        LAB_TRY(LAB_InitBlock(&LAB_BLOCK_OUTSIDE, (LAB_Block) {
            .flags = LAB_BLOCK_OPAQUE,
            .dia = LAB_RGB(255, 255, 255)
        }));

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
            LAB_TRY(LAB_BlockGroupStone_Init(assets, rocks[i].ids, tint0, tint1));
        }
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
            LAB_Box2Z tex;
            LAB_TextureComposite c[] = {
                { "stone", LAB_RGBX(5a5a55),  LAB_RGBX(b4b4aa) },
                { "ore_overlay", ores[i].ore_tint[0], ores[i].ore_tint[1] },
                {0},
            };
            tex = LAB_Assets_NewComposedTexture(assets, c);
            LAB_TRY(LAB_BlockFull_Init(assets, ores[i].id, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID));
        }

        {
            LAB_Box2Z tex;
            LAB_TextureComposite c[] = {
                { "layered_stone", LAB_RGBX(5a5a55),  LAB_RGBX(b4b4aa) },
                { "coal_ore_overlay", LAB_RGBX(000000), LAB_RGBX(222222) },
                {0},
            };
            tex = LAB_Assets_NewComposedTexture(assets, c);
            LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_COAL_ORE, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID));
        }
    }




    ///// SOIL TYPES /////
    {
        LAB_Box2Z tex;

        tex = LAB_Assets_NewTexture(assets, "grass");
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_GRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_SOLID));

        tex = LAB_Assets_NewTexture(assets, "dirt");
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_DIRT, tex, LAB_RGB(120, 80, 50), LAB_RENDER_PASS_SOLID));

        tex = LAB_Assets_NewComposedTexture(assets, (const LAB_TextureComposite[])
        {
            { "dirt",    0, LAB_RGBX(785032) }, // LAB_RGBX(785032)
            { "needle_leaves",  0, LAB_RGBX(704828) }, // LAB_RGBX(886440)
            { "leaves",  0, LAB_RGBAX(70482880) }, // LAB_RGBX(886440)
            {0}
        });
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_FOREST_SOIL, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID));

        tex = LAB_Assets_NewTintedTexture(assets, "sand", LAB_RGBI(0x896400), LAB_RGB(255, 230+10, 130+60));
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_SAND, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID));

        tex = LAB_Assets_NewTintedTexture(assets, "snow", LAB_RGBI(0xaaddff), LAB_RGBI(0xffffff));
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_SNOW, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID));
    }






    ///// TREE TYPES /////
    {
        LAB_Box2Z tex_leaves,
                  tex_needle_leaves,
                  tex_oak_log,
                  tex_birch_log,
                  tex_planks;
        tex_leaves        = LAB_Assets_NewTexture(assets, "leaves");
        tex_needle_leaves = LAB_Assets_NewTexture(assets, "needle_leaves");
        tex_oak_log       = LAB_Assets_NewTexture(assets, "oak_log");
        tex_birch_log     = LAB_Assets_NewTexture(assets, "birch_log");
        tex_planks        = LAB_Assets_NewTexture(assets, "planks");


        ///// OAK /////
        LAB_TRY(LAB_LeavesBlock_Init(assets, &LAB_BLOCK_LEAVES, tex_leaves, LAB_COLOR_LEAVES, LAB_RENDER_PASS_MASKED, LAB_RGB(130, 200, 130)));
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD, tex_oak_log, LAB_COLOR_WOOD, LAB_RENDER_PASS_SOLID));
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD_PLANKS, tex_planks, LAB_RGB(220, 197, 162), LAB_RENDER_PASS_SOLID));
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_WOOD_PLANKS_DARK, tex_planks, LAB_RGB(120, 97, 80), LAB_RENDER_PASS_SOLID));


        ///// BIRCH /////
        LAB_TRY(LAB_LeavesBlock_Init(assets, &LAB_BLOCK_BIRCH_LEAVES, tex_leaves, LAB_COLOR_BIRCH_LEAVES, LAB_RENDER_PASS_MASKED, LAB_COLOR_BIRCH_LEAVES));
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_BIRCH_WOOD, tex_birch_log, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID));


        ///// SPRUCE /////
        LAB_TRY(LAB_LeavesBlock_Init(assets, &LAB_BLOCK_SPRUCE_LEAVES, tex_needle_leaves, LAB_RGBX(006622), LAB_RENDER_PASS_MASKED, LAB_RGBX(006622)));
        LAB_TRY(LAB_BlockFull_Init(assets, &LAB_BLOCK_SPRUCE_WOOD, tex_birch_log, LAB_RGBX(332211), LAB_RENDER_PASS_SOLID)); // LAB_RGBX(664422)
    }





    ///// FOILAGE /////
    {
        LAB_Box2Z tex;

        tex = LAB_Assets_NewTexture(assets, "tall_grass");
        LAB_TRY(LAB_BlockCross_Init(assets, &LAB_BLOCK_TALLGRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_MASKED));
        LAB_BlockP(LAB_BLOCK_TALLGRASS)->dia = LAB_RGBX(dddddd);
        LAB_BlockP(LAB_BLOCK_TALLGRASS)->bounds = (LAB_Box3F) {{0.125, 0, 0.125}, {0.875, 0.375, 0.875}};

        tex = LAB_Assets_NewTexture(assets, "taller_grass");
        LAB_TRY(LAB_BlockCross_Init(assets, &LAB_BLOCK_TALLERGRASS, tex, LAB_COLOR_GRASS, LAB_RENDER_PASS_MASKED));
        LAB_BlockP(LAB_BLOCK_TALLERGRASS)->dia = LAB_RGBX(dddddd);
        LAB_BlockP(LAB_BLOCK_TALLERGRASS)->bounds = (LAB_Box3F) {{0.125, 0, 0.125}, {0.875, 0.75, 0.875}};


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
            tex = LAB_Assets_NewComposedTexture(assets, (const LAB_TextureComposite[])
            {
                { "tulip_stem",  0, LAB_COLOR_GRASS  },
                { "tulip_bloom", tulips[i].bloom_colors[0], tulips[i].bloom_colors[1] },
                {0}
            });
            LAB_TRY(LAB_BlockCross_Init(assets, tulips[i].id, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_MASKED));
            LAB_BlockP(*tulips[i].id)->dia = LAB_RGBX(dddddd);
            LAB_BlockP(*tulips[i].id)->bounds = (LAB_Box3F){{0.25, 0, 0.25}, {0.75, 0.875, 0.75}};
        }


        tex = LAB_Assets_NewTexture(assets, "fallen_leaves");
        LAB_TRY(LAB_FallenLeavesBlock_Init(assets, &LAB_BLOCK_FALLEN_LEAVES, tex, LAB_COLOR_BIRCH_LEAVES));
    }




    ///// FLUIDS /////
    {
        LAB_Box2Z tex;
        tex = LAB_Assets_NewTintedTexture(assets, "water", 0, LAB_COLOR_WATER);
        LAB_Color tint = LAB_COLOR_WHITE;

        LAB_Box2F tex_f = LAB_Box2Z2F(tex);

        LAB_Model* m = LAB_Assets_NewModel(assets);
        LAB_TRY(assets->err);

        m->render_pass = LAB_RENDER_PASS_ALPHA;
        LAB_Builtin_ModelAddCubeAll(m,
            LAB_AABB_FULL_CUBE,
            tex_f, LAB_BoxColors_Shaded(tint));
        LAB_Builtin_ModelAddCubeInvertedAll(m,
            LAB_AABB_FULL_CUBE,
            tex_f, LAB_BoxColors_Shaded(tint));


        LAB_TRY(LAB_InitItemBlock(&LAB_BLOCK_WATER, (LAB_Block) {
            .model = m,
            .item_texture = LAB_Assets_RenderItem(assets, tex, tint),
            .flags = LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_OPAQUE_SELF,
            .tags  = LAB_BLOCK_TAG_REPLACEABLE,
            .dia   = LAB_COLOR_WATER_DIA
        }));
    }

    ///// GLASS TYPES /////
    {
        LAB_Box2Z tex;
        tex = LAB_Assets_NewTexture(assets, "glass");
        LAB_BlockFull_Init(assets, &LAB_BLOCK_GLASS, tex, LAB_RGBA(255, 255, 255, 128), LAB_RENDER_PASS_ALPHA);
        LAB_BlockP(LAB_BLOCK_GLASS)->flags = LAB_BLOCK_MASSIVE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_OPAQUE_SELF;
        LAB_BlockP(LAB_BLOCK_GLASS)->dia   = LAB_COLOR_WHITE;
    }

    ///// METAL BLOCKS /////
    {
        LAB_Box2Z tex;

        tex = LAB_Assets_NewTexture(assets, "metal");
        LAB_BlockFull_Init(assets, &LAB_BLOCK_METAL, tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID);
    }

    ///// MISC /////
    {
        LAB_Box2Z tex;
        {
            tex = LAB_Assets_NewComposedTexture(assets, (const LAB_TextureComposite[])
            {
                { "torch_stick", 0, LAB_RGB(120, 97, 80) },
                { "torch_flame", 0, LAB_RGBX(ffffff) },
                {0}
            });

            #define X0 (7/16.)
            #define X1 (9/16.)
            #define Y1 (10/16.)
            static const LAB_Box3F aabb = { {X0, 0, X0}, {X1, Y1, X1} };

            static const LAB_BoxTextures torch_uv = {{
                {{X0,     1-Y1},        {X1, 1}},
                {{X0,     1-Y1},        {X1, 1}},
                {{X0, (28/32.)}, {X1, (32/32.)}},
                {{X0, (11/32.)}, {X1, (15/32.)}},
                {{X0,     1-Y1},        {X1, 1}},
                {{X0,     1-Y1},        {X1, 1}},
            }};

            LAB_Box2F tex_f = LAB_Box2Z2F(tex);

            LAB_BoxTextures uv = LAB_BoxTextures_Map(LAB_BoxTextures_All(tex_f), torch_uv);

            LAB_Model* m = LAB_Assets_NewModel(assets);
            LAB_TRY(assets->err);
            m->render_pass = LAB_RENDER_PASS_MASKED;
            LAB_Builtin_ModelAddCube(m, aabb, uv, LAB_box_color_flat);

            LAB_TRY(LAB_InitItemBlock(&LAB_BLOCK_TORCH, (LAB_Block) {
                .flags = (LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE|LAB_BLOCK_GLOWING|LAB_BLOCK_NOSHADE),
                .dia = LAB_RGB(255, 255, 255),
                //.lum = LAB_RGB(220, 210, 180),
                .lum = LAB_RGBI_HDR(0xdcd2b4, -1+1), //LAB_Color_To_ColorHDR(LAB_RGB(110, 105, 90)),
                .item_texture = LAB_Assets_RenderItem(assets, tex, LAB_COLOR_WHITE),
                .model = m,
                //.bounds = {{0.375, 0, 0.375}, {0.625, 0.75, 0.625}},
                .bounds = {{7/16., 0, 7/16.}, {9/16., 10/16., 9/16.}}, // aabb
            }));
        }
    }

    ///// LIGHT BLOCKS /////
    {
        static const struct { LAB_BlockID* id; LAB_ColorHDR lum, tint[2]; }
        lights[] =
        {
            { &LAB_BLOCK_LIGHT,        LAB_RGBI_HDR(0xffffff, 0), { LAB_RGBI(0xbbddff), LAB_RGBI(0xffffff) } }, // LAB_RGBX(86bedd)
            { &LAB_BLOCK_WARM_LIGHT,   LAB_RGBI_HDR(0xffffdc, 0), { LAB_RGBI(0xffccaa), LAB_RGBI(0xffffff) } }, // LAB_RGBX(ddbe86)
            { &LAB_BLOCK_COLD_LIGHT,   LAB_RGBI_HDR(0xc8e6ff, 0), { LAB_RGBI(0x80ccff), LAB_RGBI(0xf0f8ff) } },
            { &LAB_BLOCK_BLUE_LIGHT,   LAB_RGBI_HDR(0x1040ff, 0), { LAB_RGBI(0x1040ff), LAB_RGBI(0xa8d0ff) } },
            { &LAB_BLOCK_CYAN_LIGHT,   LAB_RGBI_HDR(0x1080ff, 0), { LAB_RGBI(0x1080ff), LAB_RGBI(0xe0ffff) } },
            { &LAB_BLOCK_GREEN_LIGHT,  LAB_RGBI_HDR(0x40ff10, 0), { LAB_RGBI(0x40ff10), LAB_RGBI(0xf0ffc0) } },
            { &LAB_BLOCK_URANIUM_LIGHT,LAB_RGBI_HDR(0xa0f010, 0), { LAB_RGBI(0xa0f010), LAB_RGBI(0xffffc0) } },
            { &LAB_BLOCK_YELLOW_LIGHT, LAB_RGBI_HDR(0xfff010, 0), { LAB_RGBI(0xfff010), LAB_RGBI(0xffffc0) } },
            { &LAB_BLOCK_ORANGE_LIGHT, LAB_RGBI_HDR(0xff8010, 0), { LAB_RGBI(0xff8010), LAB_RGBI(0xffe0c0) } },
            { &LAB_BLOCK_RED_LIGHT,    LAB_RGBI_HDR(0xff1410, 0), { LAB_RGBI(0xff1410), LAB_RGBI(0xffc0a0) } },
            { &LAB_BLOCK_PURPLE_LIGHT, LAB_RGBI_HDR(0x8010ff, 0), { LAB_RGBI(0x8010ff), LAB_RGBI(0xffe0ff) } },
        };

        for(size_t i = 0; i < LAB_LEN(lights); ++i)
        {
            LAB_Box2Z tex;
            tex = LAB_Assets_NewTintedTexture(assets, "light_modern", lights[i].tint[0], lights[i].tint[1]);
            LAB_BlockLight_Init(assets, lights[i].id, tex, lights[i].lum);
        }


        static const struct { LAB_BlockID* id; LAB_ColorHDR lum, tint[2]; }
        crystals[] =
        {
            { &LAB_BLOCK_BLUE_CRYSTAL,   LAB_RGBI_HDR(0x082080, 0), { LAB_RGBI(0x2020ff), LAB_RGBI(0x2040ff) } },
            { &LAB_BLOCK_GREEN_CRYSTAL,  LAB_RGBI_HDR(0x208008, 0), { LAB_RGBI(0x28ff20), LAB_RGBI(0x80ff40) } },
            { &LAB_BLOCK_YELLOW_CRYSTAL, LAB_RGBI_HDR(0x807808, 0), { LAB_RGBI(0xfff010), LAB_RGBI(0xffff40) } },
            { &LAB_BLOCK_RED_CRYSTAL,    LAB_RGBI_HDR(0xff0a08, 0), { LAB_RGBI(0xff190f), LAB_RGBI(0xff4020) } },
        };
        const LAB_Color crystal_shade = LAB_RGBX(eeeeee);
        const LAB_Color crystal_light = LAB_RGBX(222222);

        for(size_t i = 0; i < LAB_LEN(crystals); ++i)
        {
            LAB_Box2Z tex;
            LAB_Color tint0 = LAB_MulColor(crystals[i].tint[0], crystal_shade);
            LAB_Color tint1 = LAB_MulColorInv(crystals[i].tint[1], crystal_light);
            tex = LAB_Assets_NewTintedTexture(assets, "crystal", tint0, tint1);
            LAB_BlockLight_Init(assets, crystals[i].id, tex, crystals[i].lum);
        }
    }

    ///// SPECIAL BLOCKS /////
    {
        LAB_Box2Z tex;

        tex = LAB_Assets_NewTexture(assets, "light_item");
        LAB_TRY(LAB_InitItemBlock(&LAB_BLOCK_INVISIBLE_LIGHT, (LAB_Block) {
            .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_EMISSIVE,
            .lum = LAB_RGBI_HDR(0xffffff, 0),
            .dia = LAB_RGB(255, 255, 255),
            .item_texture = LAB_Assets_RenderItem(assets, tex, LAB_COLOR_WHITE),
            .bounds = LAB_AABB_FULL_CUBE,
        }));

        tex = LAB_Assets_NewTexture(assets, "barrier_item");
        LAB_TRY(LAB_InitItemBlock(&LAB_BLOCK_BARRIER, (LAB_Block) {
            .flags = LAB_BLOCK_INTERACTABLE | LAB_BLOCK_MASSIVE,
            .dia = LAB_RGB(255, 255, 255),
            .item_texture = LAB_Assets_RenderItem(assets, tex, LAB_COLOR_WHITE),
            .bounds = LAB_AABB_FULL_CUBE,
        }));
    }

    return assets->err;
}