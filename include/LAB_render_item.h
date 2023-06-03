#pragma once

#include <SDL2/SDL_video.h>
#include "LAB_block.h"
#include "LAB_texture_atlas.h"
#include "LAB_stdinc.h"
#include "LAB_util.h"
#include "LAB_vec2.h"

#define LAB_ITEM_SIZE 32

typedef struct LAB_ItemTexSet
{
    size_t size, capacity;
    SDL_Surface** surfaces;
} LAB_ItemTexSet;

#define LAB_ItemTexSet_Create(s) LAB_ObjClear(s)
void LAB_ItemTexSet_Destroy(LAB_ItemTexSet* s);

SDL_Surface* LAB_ItemTexSet_Render(LAB_ItemTexSet* s, LAB_TexAtlas* atlas, LAB_Box2Z tex, LAB_Color tint);