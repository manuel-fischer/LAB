#include "LAB_render_item.h"

#include "LAB_gl.h"
#include "LAB_direction.h"
#include "LAB_asset_manager.h"
#include "LAB_sdl.h"
#include "LAB_image.h"
#include "LAB_color_defs.h"

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

    LAB_ImageView vatlas = LAB_ImageView_Create(atlas->w, atlas->h, atlas->data);
    LAB_ImageView vsrc = LAB_ImageView_Clip(vatlas, LAB_Box2Z_Mul(atlas->cell_size, tex));
    LAB_ImageView vdst = LAB_ImageView_CreateSDL(surf);

    LAB_Color block_color = tint;
    LAB_Image_Copy(vdst, vsrc);
    LAB_Image_MulColor_Fast(vdst, block_color);

    // rounded corners
    LAB_Image_SetPixel(vdst,  0,  0, LAB_COLOR_BLANK);
    LAB_Image_SetPixel(vdst,  0, 31, LAB_COLOR_BLANK);
    LAB_Image_SetPixel(vdst, 31,  0, LAB_COLOR_BLANK);
    LAB_Image_SetPixel(vdst, 31, 31, LAB_COLOR_BLANK);

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