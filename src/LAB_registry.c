#include "LAB_registry.h"

#include "LAB_image.h"
#include "LAB_sdl.h"
#include "LAB_bits.h"

bool LAB_Registry_Create(LAB_Registry* registry)
{
    registry->sprite_size = 32;
    return  LAB_TexAlloc_Create(&registry->tex_alloc)
        && (LAB_TexAtlas_Create(&registry->tex_atlas, registry->sprite_size)
              || (LAB_TexAlloc_Destroy(&registry->tex_alloc), false));
}

void LAB_Registry_Destroy(LAB_Registry* registry)
{
    LAB_TexAtlas_Destroy(&registry->tex_atlas);
    LAB_TexAlloc_Destroy(&registry->tex_alloc);
}

bool LAB_Registry_LoadTexture(LAB_Registry* registry, const char* filename, LAB_OUT LAB_Texture* texture)
{
    bool success = true;

    SDL_Surface* surf = LAB_ImageLoad(filename);

    if(!surf) return false;

    // TODO: rescale/pad image?
    LAB_ASSERT(surf->w == surf->h);
    LAB_ASSERT(LAB_IsPow2(surf->w));

    size_t pos[2];
    if(success) success = LAB_TexAlloc_Add(&registry->tex_alloc, surf->w/registry->sprite_size, pos);
    if(success) success = LAB_TexAtlas_DrawAlloc(&registry->tex_atlas, pos[0]*registry->sprite_size, pos[1]*registry->sprite_size, surf->w, (LAB_Color*)surf->pixels);

    LAB_SDL_FREE(SDL_FreeSurface, &surf);

    return success;
}
