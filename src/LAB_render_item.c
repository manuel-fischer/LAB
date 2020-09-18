#include "LAB_render_item.h"

#include "LAB_gl.h"
#include "LAB_direction.h"
#include "LAB_asset_manager.h"

#include <math.h>

SDL_Surface* LAB_RenderBlock2D(LAB_Block* block)
{
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, LAB_ITEM_SIZE, LAB_ITEM_SIZE, 32, SDL_PIXELFORMAT_RGBA32);

    LAB_Triangle* tri = NULL;
    for(size_t i = 0; i < block->model->size; ++i)
    {
        if(block->model->data[i].v[0].flags==LAB_DIR_UP)
        {
            tri = &block->model->data[i];
            break;
        }
    }
    if(tri == NULL) return surf;

    SDL_Rect src_rect;
    src_rect.x = LAB_TILE_SIZE*(int)floor((tri->v[0].u+tri->v[1].u+tri->v[2].u)*(1/3.f));
    src_rect.y = LAB_TILE_SIZE*(int)floor((tri->v[0].v+tri->v[1].v+tri->v[2].v)*(1/3.f));
    src_rect.w = LAB_TILE_SIZE;
    src_rect.h = LAB_TILE_SIZE;

    SDL_BlitScaled(LAB_block_terrain, &src_rect, surf, NULL);

    LAB_Color block_color = tri->v[0].color;
    for(int i = 0; i < LAB_ITEM_SIZE*LAB_ITEM_SIZE; ++i)
    {
        LAB_Color* c = &((LAB_Color*)surf->pixels)[i];
        *c = LAB_MulColor(*c, block_color);

    }
    //SDL_BlitScaled()
    //memset(surf->pixels, rand()&0xff, 16*16*4);
    return surf;
}
