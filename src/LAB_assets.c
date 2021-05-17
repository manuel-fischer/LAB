#include "LAB_assets.h"

#include "LAB_obj.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "LAB_sdl.h"
#include "LAB_image.h"

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
    LAB_DBG_PRINTF("Load texture asset: \"%s\"\n", resource_name);

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

bool LAB_Assets_Create(LAB_Assets* assets)
{
    LAB_OBJ(LAB_AssetMgr_Create(&assets->textures, &LAB_TextureAsset_behavior, NULL),
            LAB_AssetMgr_Destroy(&assets->textures),

        return 1;
    );
    return 0;
}

void LAB_Assets_Destroy(LAB_Assets* assets)
{
    LAB_AssetMgr_Destroy(&assets->textures);
}


SDL_Surface* LAB_Assets_LoadTexture(LAB_Assets* assets, const char* resource_name)
{
    return *(SDL_Surface**)LAB_AssetMgr_GetByName(&assets->textures, resource_name);
}




#include "LAB_texture_atlas.h"


void LAB_Temp_RecreateTerrain(LAB_TexAtlas* atlas)
{
    struct { size_t x, y; LAB_Color black_tint, white_tint; const char* name; } textures[] =
    {
        { 0, 0, 0, 0, "stone"         },
        { 1, 0, 0, 0, "cobble"        },
        //{ 1, 0, 0, LAB_RGBAX(88dd55ff), "fallen_leaves"         },
        { 2, 0, 0, 0, "oak_log"       },
        { 3, 0, 0, 0, "planks"        },
        { 4, 0, 0, 0, "metal"         },
        //{ 4, 0, LAB_RGBX(804020), LAB_RGBX(ffc060), "metal"         },
        //{ 4, 0, 0, LAB_RGBAX(509a8f80), "barrier_item"         },
        { 5, 0, 0, 0, "birch_log"     },
        { 0, 1, 0, 0, "smooth_stone"  },
        { 1, 1, 0, 0, "bricks"        },
        { 2, 1, 0, 0, "grass"         },
        { 3, 1, 0, 0, "sand"          },
        { 4, 1, 0, 0, "dirt"          },
        //{ 0, 2, 0, 0, "cold_light"    },
        { 0, 2, 0, 0, "blank"         },
        { 0, 2, LAB_RGBX(86bedd), LAB_RGBX(ffffff), "light"    },
        //{ 1, 2, 0, 0, "warm_light"    },
        { 1, 2, 0, 0, "blank"    },
        { 1, 2, LAB_RGBX(ddbe86), LAB_RGBX(ffffff), "light"    },
        { 2, 2, 0, 0, "leaves"        },
        { 3, 2, 0, 0, "tall_grass"    },
        //{ 4, 2, },
        { 0, 3, 0, 0, "blank"         },
        //{ 0, 3, 0, LAB_RGB(220, 197, 162), "torch_stick" },
        { 0, 3, 0, LAB_RGB(120, 97, 80), "torch_stick" },
        { 0, 3, 0, LAB_RGBX(ffffff),       "torch_flame" },


        { 1, 3, 0, 0, "glass"         },
        { 2, 3, 0, 0, "crystal"       },
        { 3, 3, 0, 0, "taller_grass"  },
        //{ 4, 3, },
        //{ 5, 3, },
        { 2, 4, 0, 0, "fallen_leaves" },
        { 5, 4, 0, 0, "blank"         },
        { 5, 4, 0, LAB_RGBX(0f961e), "tulip_stem" },
        { 5, 4, LAB_RGBX(400020), LAB_RGBX(ff2010), "tulip_bloom" },
        { 6, 4, 0, 0, "blank"            },
        { 6, 4, 0, LAB_RGBX(0f961e), "tulip_stem" },
        { 6, 4, LAB_RGBX(805000), LAB_RGBX(ffff00), "tulip_bloom" },
        { 0, 7, 0, 0, "light_item"    },
        { 1, 7, 0, 0, "barrier_item"  },
    };

    LAB_Assets assets;
    LAB_Assets_Create(&assets);
    
    for(size_t i = 0; i < sizeof(textures)/sizeof(textures[0]); ++i)
    {
        size_t cell_size = 32;
        SDL_Surface* surf;
        surf = LAB_Assets_LoadTexture(&assets, textures[i].name);
        
        LAB_ASSERT_OR_ABORT(surf);
        LAB_ASSERT_OR_ABORT(surf->w == surf->h);
        LAB_ASSERT_OR_ABORT(LAB_IsPow2(surf->w));


        size_t pos[2] = { textures[i].x, textures[i].y };
        //LAB_ASSERT_OR_ABORT(LAB_TexAlloc_Add(&alloc, surf->w/cell_size, pos));
        LAB_DBG_PRINTF("%s at %zu, %zu\n", textures[i].name, pos[0], pos[1]);
        //LAB_MemSetColor((LAB_Color*)surf->pixels, LAB_RGBX(0000ff), i);

        if(textures[i].black_tint || textures[i].white_tint)
            LAB_TexAtlas_DrawBlit(atlas, pos[0]*cell_size, pos[1]*cell_size, surf->w, (LAB_Color*)surf->pixels, textures[i].black_tint, textures[i].white_tint);
        else
            LAB_ASSERT_OR_ABORT(LAB_TexAtlas_DrawAlloc(atlas, pos[0]*cell_size, pos[1]*cell_size, surf->w, (LAB_Color*)surf->pixels));
    }

    LAB_Assets_Destroy(&assets);
}