#pragma once

#include "LAB_assets.h"
#include "LAB_block.h"
#include "LAB_opt.h"
#include "LAB_builtin_model.h"
#include "LAB_blocks.h"
#include "LAB_render_item.h"
#include "LAB_color_defs.h"

#include "LAB_aabb.h"

LAB_INLINE
bool LAB_BlockFull_Init(LAB_Assets* assets, LAB_BlockID* bid,
                        LAB_TexRect tex, LAB_Color tint, LAB_RenderPass render_pass)
{
    if(!LAB_RegisterBlocksGen(bid, 1)) return false;
    LAB_Block* b = LAB_BlockP(*bid);

    b->flags = LAB_BLOCK_SOLID;

    LAB_Box2F tex_f = LAB_Box2Z2F(tex);

    LAB_Model* m = LAB_Assets_NewModel(assets);
    if(!m) return false;
    m->render_pass = render_pass;
    LAB_Builtin_ModelAddCubeAll(m,
        LAB_AABB_FULL_CUBE,
        tex_f, LAB_BoxColors_Shaded(tint));

    b->model = m;
    b->bounds = LAB_AABB_FULL_CUBE;
    //b->item_texture = LAB_Assets_LoadTexture(assets, "stone");
    b->item_texture = LAB_Assets_RenderItem(assets, tex, tint);

    LAB_AddBlockItems(*bid, 1);

    return true;
}



typedef enum LAB_Stone_Variant
{
    LAB_STONE_VARIANT_RAW,
    LAB_STONE_VARIANT_LAYERED,
    LAB_STONE_VARIANT_COBBLE,
    LAB_STONE_VARIANT_BRICKS,
    LAB_STONE_VARIANT_SMOOTH,

    LAB_STONE_VARIANT_COUNT
} LAB_Stone_Variant;


typedef union LAB_BlockGroupStone
{
    struct
    {
        LAB_BlockID raw,
                    layered,
                    cobble,
                    bricks,
                    smooth;
    };
    LAB_BlockID blocks[LAB_STONE_VARIANT_COUNT];
} LAB_BlockGroupStone;

bool LAB_BlockGroupStone_Init(LAB_Assets* assets,
    LAB_BlockGroupStone* grp,
    LAB_Color black, LAB_Color white);



LAB_INLINE
bool LAB_BlockCross_Init(LAB_Assets* assets, LAB_BlockID* bid,
                          LAB_Box2Z tex, LAB_Color tint, LAB_RenderPass render_pass)
{
    if(!LAB_RegisterBlocksGen(bid, 1)) return false;
    LAB_Block* b = LAB_BlockP(*bid);

    b->flags = LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE;
    b->dia = LAB_COLOR_WHITE;

    LAB_Box2F tex_f = LAB_Box2Z2F(tex);

    LAB_Model* m = LAB_Assets_NewModel(assets);
    if(!m) return false;
    m->render_pass = render_pass;
    LAB_Builtin_ModelAddCross(m,
        LAB_AABB_CROSS,
        tex_f, tint);

    b->model = m;
    b->bounds = LAB_AABB_CROSS;
    //b->item_texture = LAB_Assets_LoadTexture(assets, "stone");
    b->item_texture = LAB_Assets_RenderItem(assets, tex, tint);

    LAB_AddBlockItems(*bid, 1);

    return true;
}




LAB_INLINE
bool LAB_BlockLight_Init(LAB_Assets* assets, LAB_BlockID* bid,
                         LAB_Box2Z tex, LAB_ColorHDR lum)
{
    if(!LAB_RegisterBlocksGen(bid, 1)) return false;
    LAB_Block* b = LAB_BlockP(*bid);

    LAB_Model* m = LAB_Assets_NewModel(assets);
    LAB_Box2F tex_f = LAB_Box2Z2F(tex);
    LAB_Builtin_ModelAddCubeAll(m, LAB_AABB_FULL_CUBE, tex_f, LAB_box_color_flat);
    *b = (LAB_Block) {
        .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
        //.lum = LAB_RGB(255, 64, 16),
        .lum = lum, //LAB_RGB(255, 20, 16),
        //.item_tint = LAB_RGB(255, 100, 50),
        .item_texture = LAB_Assets_RenderItem(assets, tex, LAB_COLOR_WHITE),
        .model = m,
        .bounds = LAB_AABB_FULL_CUBE,
    };

    LAB_AddBlockItems(*bid, 1);

    return true; // TODO
}