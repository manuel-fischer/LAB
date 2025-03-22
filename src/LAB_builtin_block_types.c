#include "LAB_builtin_block_types.h"


static const char* const LAB_stone_textures[LAB_STONE_VARIANT_COUNT] =
{
    [LAB_STONE_VARIANT_RAW]     = "stone",
    [LAB_STONE_VARIANT_LAYERED] = "layered_stone",
    [LAB_STONE_VARIANT_COBBLE]  = "cobble",
    [LAB_STONE_VARIANT_BRICKS]  = "bricks",
    [LAB_STONE_VARIANT_SMOOTH]  = "smooth_stone",
};

LAB_Err LAB_BlockGroupStone_Init(LAB_Assets* assets,
    LAB_BlockGroupStone* grp,
    LAB_Color black, LAB_Color white)
{
    for(int i = 0; i < LAB_STONE_VARIANT_COUNT; ++i)
    {
        LAB_TexRect tex = LAB_Assets_NewTintedTexture(assets, LAB_stone_textures[i], black, white);
        LAB_TRY(assets->err);
        LAB_TRY(LAB_BlockFull_Init(assets, &grp->blocks[i], tex, LAB_COLOR_WHITE, LAB_RENDER_PASS_SOLID));
    }
    return LAB_OK;
}