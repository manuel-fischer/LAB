#pragma once

#include "LAB_asset_manager.h"
#include "LAB_texture_atlas.h"
#include "LAB_model_set.h"
#include "LAB_render_item.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_Assets
{
    LAB_AssetMgr  textures;
    LAB_TexAlloc  alloc;

    LAB_TexAtlas* atlas;
    LAB_ModelSet* models;
    LAB_ItemTexSet* items; // item textures
} LAB_Assets;

bool LAB_Assets_Create(LAB_Assets* assets, LAB_TexAtlas* atlas, LAB_ModelSet* models, LAB_ItemTexSet* items);
void LAB_Assets_Destroy(LAB_Assets* assets);

SDL_Surface* LAB_Assets_LoadTexture(LAB_Assets* assets, const char* resource_name);


typedef struct LAB_TextureComposite
{
    const char* resource_name;
    LAB_Color black_tint, white_tint;
} LAB_TextureComposite;

typedef LAB_Box2Z LAB_TexRect;
LAB_INLINE bool LAB_TexRect_IsOk(LAB_TexRect a) { return a.a.x != a.b.x; }

LAB_TexRect LAB_Assets_NewComposedTexture(LAB_Assets* assets,
                                          const LAB_TextureComposite composite[/*NULLTERM*/]);

LAB_TexRect LAB_Assets_NewTexture(LAB_Assets* assets,
                                  const char* resource_name);

LAB_TexRect LAB_Assets_NewTintedTexture(LAB_Assets* assets,
                                        const char* resource_name,
                                        LAB_Color black_color, LAB_Color white_color);


LAB_Model* LAB_Assets_NewModel(LAB_Assets* assets);

LAB_INLINE
SDL_Surface* LAB_Assets_RenderItem(LAB_Assets* assets, LAB_Box2Z tex, LAB_Color c)
{
    return LAB_ItemTexSet_Render(assets->items, assets->atlas, tex, c);
}
