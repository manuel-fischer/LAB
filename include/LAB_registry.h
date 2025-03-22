#pragma once

#include "LAB_texture_atlas.h"


typedef struct LAB_Registry
{
    size_t sprite_size; // todo remove to settings
    LAB_TexAlloc tex_alloc;
    LAB_TexAtlas tex_atlas;
} LAB_Registry;

bool LAB_Registry_Create(LAB_Registry* registry);
void LAB_Registry_Destroy(LAB_Registry* registry);

