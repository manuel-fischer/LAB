#pragma once

#include "LAB_asset_manager.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_Assets
{
    LAB_AssetMgr textures;
} LAB_Assets;

bool LAB_Assets_Create(LAB_Assets* assets);
void LAB_Assets_Destroy(LAB_Assets* assets);

SDL_Surface* LAB_Assets_LoadTexture(LAB_Assets* assets, const char* resource_name);