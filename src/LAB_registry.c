#include "LAB_registry.h"

#include "LAB_image.h"
#include "LAB_sdl.h"

bool LAB_Registry_Create(LAB_Registry* registry)
{
    registry->sprite_size = 32;
    return  LAB_TexAlloc_Create(&registry->tex_alloc)
        && (LAB_TexAtlas_Create(&registry->tex_atlas, registry->sprite_size)
              || (LAB_TexAlloc_Destroy(&registry->tex_alloc), false));
}

bool LAB_Registry_Destroy(LAB_Registry* registry)
{
    LAB_TexAtlas_Destroy(&registry->tex_atlas);
    LAB_TexAlloc_Destroy(&registry->tex_alloc);
}

bool LAB_Registry_LoadTexture(LAB_Registry* registry, const char* filename, LAB_OUT LAB_Texture* texture)
{
        SDL_Surface* surf = LAB_ImageLoad(filename);
        LAB_ASSERT(surf);
        LAB_ASSERT(surf->w == surf->h);
        LAB_ASSERT(LAB_IsPow2(surf->w));

        size_t pos[2];
        LAB_ASSERT(LAB_TexAlloc_Add(&registry->tex_alloc, surf->w/registry->sprite_size, pos));
        LAB_DBG_PRINTF("%s at %i, %i\n", filename, pos[0], pos[1]);
        //LAB_MemSetColor((LAB_Color*)surf->pixels, LAB_RGBX(0000ff), i);
        LAB_ASSERT(LAB_TexAtlas_DrawAlloc(&registry->tex_atlas, pos[0]*registry->sprite_size, pos[1]*registry->sprite_size, surf->w, (LAB_Color*)surf->pixels));

        LAB_SDL_FREE(SDL_FreeSurface, &surf);
}
