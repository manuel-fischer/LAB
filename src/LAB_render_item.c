#include "LAB_render_item.h"

#include "LAB_gl.h"
#include "LAB_direction.h"
#include "LAB_asset_manager.h"
#include "LAB_sdl.h"
#include "LAB_image.h"

#include <math.h>

void LAB_ItemTexSet_Destroy(LAB_ItemTexSet* s)
{
    for(size_t i = 0; i < s->size; ++i)
        LAB_SDL_FREE(SDL_FreeSurface, &s->surfaces[i]);
    LAB_Free(s->surfaces);
}

// Return NULL on failure
static SDL_Surface* LAB_RenderBlockItem(LAB_TexAtlas* atlas, LAB_Box2Z tex, LAB_Color tint)
{
    LAB_ASSERT_FMT(LAB_Vec2Z_Equals(LAB_Box2Z_Size(tex), (LAB_Vec2Z) {1, 1}),
        "tex.w=%i, tex.h=%i", LAB_Box2Z_Size(tex).x, LAB_Box2Z_Size(tex).y
    );

    SDL_Surface* surf;
    LAB_SDL_ALLOC(SDL_CreateRGBSurfaceWithFormat, &surf, 0, LAB_ITEM_SIZE, LAB_ITEM_SIZE, 32, SDL_PIXELFORMAT_RGBA32);
    if(!surf) return NULL;

    /*SDL_Rect src_rect;
    src_rect.x = LAB_TILE_SIZE*tex[0][0];
    src_rect.y = LAB_TILE_SIZE*tex[0][0];
    src_rect.w = LAB_TILE_SIZE;
    src_rect.h = LAB_TILE_SIZE;*/

    //SDL_BlitScaled(atlas->data, &src_rect, surf, NULL);
    LAB_Color* dst = surf->pixels;
    size_t  stride = atlas->w;
    LAB_Color* src = atlas->data + (stride*tex.a.y + tex.a.x)*atlas->cell_size;
    for(size_t y = 0; y < atlas->cell_size; ++y, dst+=atlas->cell_size, src+=stride)
    {
        LAB_MemCpyColor(dst, src, atlas->cell_size);
    }

    LAB_Color block_color = tint;
    for(size_t i = 0; i < LAB_ITEM_SIZE*LAB_ITEM_SIZE; ++i)
    {
        LAB_Color* c = &((LAB_Color*)surf->pixels)[i];
        *c = LAB_MulColor_Fast(*c, block_color);

    }
    ((LAB_Color*)surf->pixels)[0] =
    ((LAB_Color*)surf->pixels)[31] =
    ((LAB_Color*)surf->pixels)[32*31] =
    ((LAB_Color*)surf->pixels)[32*31+31] = 0; // rounded corners
    return surf;
}


SDL_Surface* LAB_ItemTexSet_Render(LAB_ItemTexSet* s, LAB_TexAtlas* atlas, LAB_Box2Z tex, LAB_Color tint)
{
    if(s->size == s->capacity)
    {
        size_t new_cap = s->capacity * 2;
        if(new_cap == 0) new_cap = 8;
        SDL_Surface** new_arr = LAB_ReallocN(s->surfaces, new_cap, sizeof(*new_arr));
        if(new_arr == NULL) return NULL;
        s->capacity = new_cap;
        s->surfaces = new_arr;
    }

    SDL_Surface* item_tex = LAB_RenderBlockItem(atlas, tex, tint);
    if(item_tex == NULL) return NULL;

    return s->surfaces[s->size++] = item_tex;
}