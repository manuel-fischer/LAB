#include "LAB_render_item.h"

#include "LAB_gl.h"
#include "LAB_direction.h"
#include "LAB_asset_manager.h"
#include "LAB_sdl.h"

#include <math.h>

// fails and returns NULL
SDL_Surface* LAB_RenderBlock2D(LAB_Block* block)
{
    SDL_Surface* surf;
    LAB_SDL_ALLOC(SDL_CreateRGBSurfaceWithFormat, &surf, 0, LAB_ITEM_SIZE, LAB_ITEM_SIZE, 32, SDL_PIXELFORMAT_RGBA32);
    if(!surf) return NULL;

    SDL_Rect src_rect;
    src_rect.x = LAB_TILE_SIZE*block->item_tx;
    src_rect.y = LAB_TILE_SIZE*block->item_ty;
    src_rect.w = LAB_TILE_SIZE;
    src_rect.h = LAB_TILE_SIZE;

    SDL_BlitScaled(LAB_block_terrain, &src_rect, surf, NULL);

    LAB_Color block_color = block->item_tint;
    for(int i = 0; i < LAB_ITEM_SIZE*LAB_ITEM_SIZE; ++i)
    {
        LAB_Color* c = &((LAB_Color*)surf->pixels)[i];
        *c = LAB_MulColor_Fast(*c, block_color);

    }
    ((LAB_Color*)surf->pixels)[0] =
    ((LAB_Color*)surf->pixels)[31] =
    ((LAB_Color*)surf->pixels)[32*31] =
    ((LAB_Color*)surf->pixels)[32*31+31] = 0;
    return surf;
}
