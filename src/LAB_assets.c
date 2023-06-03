#include "LAB_assets.h"

#include "LAB_obj.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "LAB_sdl.h"
#include "LAB_image.h"
#include "LAB_bits.h"

static bool LAB_TextureAsset_load_resource(void* user, const char* resource_name, void* resource);
static void LAB_TextureAsset_destroy_resource(void* user, void* resource);

static const LAB_AssetMgr_Behavior LAB_TextureAsset_behavior =
{
    .resource_size = sizeof(SDL_Surface*),
    .load_resource = &LAB_TextureAsset_load_resource,
    .destroy_resource = LAB_TextureAsset_destroy_resource,
};


static bool LAB_TextureAsset_load_resource(void* user, const char* resource_name, void* resource)
{
    const size_t cell_size = 32; // << TODO
    //LAB_DBG_PRINTF("Load texture asset: \"%s\"\n", resource_name);

    SDL_Surface** surf = (SDL_Surface**)resource;
    // TODO: security checks for ../ etc !!!
    if(strcmp(resource_name, "blank") == 0)
    {
        LAB_SDL_ALLOC(SDL_CreateRGBSurfaceWithFormat, surf, 0, cell_size, cell_size, 32, SDL_PIXELFORMAT_RGBA32);
        if(*surf)
            LAB_MemSetColor((LAB_Color*)(*surf)->pixels, LAB_RGBAX(00000000), cell_size*cell_size);
    }
    else
    {
        *surf = LAB_ImageLoad_Fmt("assets/blocks/%s.png", resource_name);
    }

    if(!*surf) return 0;
    return 1;
}

static void LAB_TextureAsset_destroy_resource(void* user, void* resource)
{
    LAB_SDL_FREE(SDL_FreeSurface, (SDL_Surface**)resource);
}

bool LAB_Assets_Create(LAB_Assets* assets, LAB_TexAtlas* atlas, LAB_ModelSet* models, LAB_ItemTexSet* items)
{
    LAB_OBJ(LAB_AssetMgr_Create(&assets->textures, &LAB_TextureAsset_behavior, NULL),
            LAB_AssetMgr_Destroy(&assets->textures),

    LAB_OBJ(LAB_TexAlloc_Create(&assets->alloc),
            LAB_TexAlloc_Destroy(&assets->alloc),

    {
        assets->atlas = atlas;
        assets->models = models;
        assets->items = items;
        return 1;
    }););
    return 0;
}

void LAB_Assets_Destroy(LAB_Assets* assets)
{
    LAB_TexAlloc_Destroy(&assets->alloc);
    LAB_AssetMgr_Destroy(&assets->textures);
}


///// Texture Loading /////
SDL_Surface* LAB_Assets_LoadTexture(LAB_Assets* assets, const char* resource_name)
{
    SDL_Surface** surf = (SDL_Surface**)LAB_AssetMgr_GetByName(&assets->textures, resource_name);
    return surf ? *surf : NULL;
}



///// Texture Creation /////

static void LAB_Assets_ValidateTexture(SDL_Surface* surf, size_t cell_size)
{
    LAB_ASSERT_OR_ABORT(surf);
    LAB_ASSERT_OR_ABORT(surf->w == surf->h);
    LAB_ASSERT_OR_ABORT(LAB_IsPow2(surf->w));
    LAB_ASSERT_OR_ABORT(surf->w >= (int)cell_size);
}

LAB_TexRect LAB_Assets_NewComposedTexture(LAB_Assets* assets,
                                          const LAB_TextureComposite composite[/*NULLTERM*/])
{
    LAB_TexRect alloc = {{0, 0}, {0, 0}};

    size_t cell_size = assets->atlas->cell_size;

    LAB_ASSERT(composite[0].resource_name != NULL);

    SDL_Surface* surf;
    surf = LAB_Assets_LoadTexture(assets, composite[0].resource_name);
    LAB_Assets_ValidateTexture(surf, cell_size);

    size_t tex_size = surf->w / cell_size;
    LAB_Vec2Z pos;
    LAB_TexAlloc_Add(&assets->alloc, tex_size, LAB_Vec2Z_AsArray(&pos));
    alloc = LAB_Box2Z_FromOriginAndSize(pos, (LAB_Vec2Z) {tex_size, tex_size});

    LAB_ASSERT_OR_ABORT(LAB_TexAtlas_ClearAlloc(
        assets->atlas,
        pos.x*cell_size, pos.y*cell_size, surf->w,
        LAB_RGBA(0,0,0,0)
    ));

    for(int i = 0; composite[i].resource_name != NULL; ++i)
    {
        if(i != 0)
        {
            surf = LAB_Assets_LoadTexture(assets, composite[i].resource_name);

            LAB_Assets_ValidateTexture(surf, cell_size);
            LAB_ASSERT_OR_ABORT(tex_size*cell_size == (size_t)surf->w);
        }

        LAB_TexAtlas_DrawBlit(
            assets->atlas, pos.x*cell_size, pos.y*cell_size, surf->w,
            (LAB_Color*)surf->pixels,
            composite[i].black_tint, composite[i].white_tint
        );
    }

    return alloc;
}


LAB_TexRect LAB_Assets_NewTexture(LAB_Assets* assets,
                                  const char* resource_name)
{
    LAB_TexRect alloc = {{0, 0}, {0, 0}};

    size_t cell_size = assets->atlas->cell_size;

    SDL_Surface* surf;
    surf = LAB_Assets_LoadTexture(assets, resource_name);
    LAB_Assets_ValidateTexture(surf, cell_size);

    size_t tex_size = surf->w / cell_size;
    LAB_Vec2Z pos;
    LAB_TexAlloc_Add(&assets->alloc, tex_size, LAB_Vec2Z_AsArray(&pos));
    alloc = LAB_Box2Z_FromOriginAndSize(pos, (LAB_Vec2Z) {tex_size, tex_size});

    LAB_ASSERT_OR_ABORT(LAB_TexAtlas_DrawAlloc(
        assets->atlas,
        pos.x*cell_size, pos.y*cell_size, surf->w,
        (LAB_Color*)surf->pixels
    ));

    return alloc;
}

LAB_TexRect LAB_Assets_NewTintedTexture(LAB_Assets* assets,
                                        const char* resource_name,
                                        LAB_Color black_color, LAB_Color white_color)
{
    LAB_TextureComposite c[] = { { resource_name, black_color, white_color }, {0} };
    return LAB_Assets_NewComposedTexture(assets, c);
}


LAB_Model* LAB_Assets_NewModel(LAB_Assets* assets)
{
    return LAB_ModelSet_NewModel(assets->models);
}
