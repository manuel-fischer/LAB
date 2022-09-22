#pragma once

#include "LAB_assets.h"
#include "LAB_block.h"
#include "LAB_opt.h"
#include "LAB_builtin_model.h"
#include "LAB_blocks.h"
#include "LAB_render_item.h"
#include "LAB_color_defs.h"


#define LAB_BlockFull_Init(assets, bid, tex, tint, render_pass) \
        LAB_BlockFull_Init_(assets, bid, (const size_t(*)[2])tex, tint, render_pass)
LAB_INLINE 
bool LAB_BlockFull_Init_(LAB_Assets* assets, LAB_BlockID* bid,
                        const size_t tex[2][2], LAB_Color tint, LAB_RenderPass render_pass)
{
    if(!LAB_RegisterBlocksGen(bid, 1)) return false;
    LAB_Block* b = LAB_BlockP(*bid);

    b->flags = LAB_BLOCK_SOLID;

    const float tex_f[2][2] = { { tex[0][0], tex[0][1] }, { tex[1][0], tex[1][1] } };

    LAB_Model* m = LAB_Assets_NewModel(assets);
    if(!m) return false;
    m->render_pass = render_pass;
    LAB_Builtin_ModelAddCubeAll(m,
        LAB_full_aabb,
        tex_f, LAB_cube_color_shade, tint);
        
    b->model = m;
    LAB_ObjCopy(&b->bounds, &LAB_full_aabb);
    //b->item_texture = LAB_Assets_LoadTexture(assets, "stone");
    b->item_texture = LAB_Assets_RenderItem(assets, tex, tint);

    LAB_AddBlockItems(*bid, 1);

    return true;
}




#define LAB_BLOCK_GROUP_STONE_NUM 5
typedef union LAB_BlockGroupStone
{
    struct
    {
        LAB_BlockID raw,
                    layered,
                    cobble,
                    bricks,
                    smooth/*,
                    polished,
                    mossy*/;
    };
    LAB_BlockID blocks[LAB_BLOCK_GROUP_STONE_NUM];
} LAB_BlockGroupStone;

typedef struct LAB_MaterialGroupStone
{
    size_t textures[LAB_BLOCK_GROUP_STONE_NUM] [2][2];
} LAB_MaterialGroupStone;


LAB_INLINE
bool LAB_MaterialGroupStone_Init(LAB_Assets* assets, LAB_MaterialGroupStone* mat, LAB_Color black, LAB_Color white)
{
    LAB_Assets_NewTintedTexture(assets, mat->textures[0], "stone",          black, white);
    LAB_Assets_NewTintedTexture(assets, mat->textures[1], "layered_stone",  black, white);
    LAB_Assets_NewTintedTexture(assets, mat->textures[2], "cobble",         black, white);
    LAB_Assets_NewTintedTexture(assets, mat->textures[3], "bricks",         black, white);
    LAB_Assets_NewTintedTexture(assets, mat->textures[4], "smooth_stone",   black, white);
    /*LAB_Assets_NewTintedTexture(assets, mat->textures[4], "polished_stone", black, white);

    LAB_TextureComposite c[] = {
        { "cobble", black, white },
        { "cobble_moss_overlay", LAB_RGBX(107000), LAB_RGBX(20a010) },
        {0}
    };
    LAB_Assets_NewComposedTexture(assets, mat->textures[5], c);*/
    return true; // TODO
}


// pass assets = NULL to not initialize the resources
LAB_INLINE 
bool LAB_BlockGroupStone_Init(LAB_Assets* assets,
    LAB_MaterialGroupStone const* mat,
    LAB_BlockGroupStone* grp,
    LAB_Color tint)
{
    for(int i = 1; i < LAB_BLOCK_GROUP_STONE_NUM; ++i)
        LAB_ObjCopy(&grp->blocks[i], &grp->blocks[0]);

    for(int i = 0; i < LAB_BLOCK_GROUP_STONE_NUM; ++i)
    {
        if(!LAB_BlockFull_Init(assets, &grp->blocks[i], mat->textures[i], tint, LAB_RENDER_PASS_SOLID))
            return false;
    }
    return true;
}




#define LAB_BlockCross_Init(assets, b, tex, tint, render_pass) \
        LAB_BlockCross_Init_(assets, b, (const size_t(*)[2])tex, tint, render_pass)
LAB_INLINE 
bool LAB_BlockCross_Init_(LAB_Assets* assets, LAB_BlockID* bid,
                          const size_t tex[2][2], LAB_Color tint, LAB_RenderPass render_pass)
{
    if(!LAB_RegisterBlocksGen(bid, 1)) return false;
    LAB_Block* b = LAB_BlockP(*bid);

    b->flags = LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL|LAB_BLOCK_FLAT_SHADE;
    b->dia = LAB_COLOR_WHITE;

    const float tex_f[2][2] = { { tex[0][0], tex[0][1] }, { tex[1][0], tex[1][1] } };

    LAB_Model* m = LAB_Assets_NewModel(assets);
    if(!m) return false;
    m->render_pass = render_pass;
    LAB_Builtin_ModelAddCross(m,
        LAB_cross_aabb,
        tex_f, tint);
        
    b->model = m;
    LAB_ObjCopy(&b->bounds, &LAB_cross_aabb);
    //b->item_texture = LAB_Assets_LoadTexture(assets, "stone");
    b->item_texture = LAB_Assets_RenderItem(assets, tex, tint);

    LAB_AddBlockItems(*bid, 1);

    return true;
}




#define LAB_BlockLight_Init(assets, bid, tex, lum) \
        LAB_BlockLight_Init_(assets, bid, (const size_t(*)[2])tex, lum)
LAB_INLINE 
bool LAB_BlockLight_Init_(LAB_Assets* assets, LAB_BlockID* bid,
                          const size_t tex[2][2], LAB_Color lum)
{
    if(!LAB_RegisterBlocksGen(bid, 1)) return false;
    LAB_Block* b = LAB_BlockP(*bid);

    LAB_Model* m = LAB_Assets_NewModel(assets);
    const float tex_f[2][2] = { { tex[0][0], tex[0][1] }, { tex[1][0], tex[1][1] } };
    LAB_Builtin_ModelAddCubeAll(m, LAB_full_aabb, tex_f, LAB_cube_color_flat, LAB_COLOR_WHITE);
    *b = (LAB_Block) {
        .flags = LAB_BLOCK_SOLID | LAB_BLOCK_GLOWING | LAB_BLOCK_NOSHADE,
        //.lum = LAB_RGB(255, 64, 16),
        .lum = lum, //LAB_RGB(255, 20, 16),
        //.item_tint = LAB_RGB(255, 100, 50),
        .item_texture = LAB_Assets_RenderItem(assets, (const size_t(*)[2])tex, LAB_COLOR_WHITE),
        .model = m,
        .bounds = LAB_AABB_FULL_CUBE,
    };

    LAB_AddBlockItems(*bid, 1);

    return true; // TODO
}