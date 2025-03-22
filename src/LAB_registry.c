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
