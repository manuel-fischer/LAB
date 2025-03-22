#include "LAB_assets.h"

#include "LAB_obj.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "LAB_sdl.h"
#include "LAB_color_defs.h"
#include "LAB_bits.h"

static LAB_Err LAB_TextureAsset_load_resource(void* user, const char* resource_name, void* resource);
static void LAB_TextureAsset_destroy_resource(void* user, void* resource);

static const LAB_AssetMgr_Behavior LAB_TextureAsset_behavior =
{
    .resource_size = sizeof(SDL_Surface*),
    .load_resource = &LAB_TextureAsset_load_resource,
    .destroy_resource = LAB_TextureAsset_destroy_resource,
};


static LAB_Err LAB_TextureAsset_load_resource(void* user, const char* resource_name, void* resource)
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

    if(!*surf) return LAB_RAISE_SDL();
    return LAB_OK;
}

static void LAB_TextureAsset_destroy_resource(void* user, void* resource)
{
    LAB_SDL_FREE(SDL_FreeSurface, (SDL_Surface**)resource);
}

bool LAB_Assets_Create(LAB_Assets* assets, LAB_TexAtlas* atlas, LAB_ModelSet* models, LAB_ItemTexSet* items)
{
    LAB_OBJ_OK(LAB_AssetMgr_Create(&assets->textures, &LAB_TextureAsset_behavior, NULL),
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
LAB_ImageCView LAB_Assets_LoadTexture(LAB_Assets* assets, const char* resource_name)
{
    if(LAB_FAILED(assets->err)) return (LAB_ImageView) {0};

    SDL_Surface** psurf = (SDL_Surface**)LAB_AssetMgr_GetByName(&assets->err, &assets->textures, resource_name);
    if(LAB_FAILED(assets->err))
        return (LAB_ImageView) {0};

    return LAB_ImageView_CreateSDL(*psurf);
}



///// Texture Creation /////

LAB_STATIC
LAB_Err LAB_Assets_ValidateTexture(LAB_Assets* assets, LAB_ImageCView v)
{
    LAB_TRY(LAB_ASSERT_OR_RAISE(LAB_ImageView_IsOk(v)));
    LAB_TRY(LAB_ASSERT_OR_RAISE(v.w == v.h));
    LAB_TRY(LAB_ASSERT_OR_RAISE(LAB_IsPow2(v.w)));
    LAB_TRY(LAB_ASSERT_OR_RAISE(v.w >= assets->atlas->cell_size));
    return LAB_OK;
}

LAB_STATIC
LAB_Err LAB_Assets_ValidateTexture_Fit(LAB_Assets* assets, LAB_TexRect alloc, LAB_ImageCView v)
{
    LAB_TRY(LAB_Assets_ValidateTexture(assets, v));
    size_t tex_size = LAB_Box2Z_DX(alloc);
    LAB_TRY(LAB_ASSERT_OR_RAISE(tex_size*assets->atlas->cell_size == v.w));
    return LAB_OK;
}

LAB_STATIC
LAB_TexRect LAB_Assets_AllocTexture_For(LAB_Assets* assets, LAB_ImageCView v)
{
    const LAB_TexRect error_result = {{0, 0}, {0, 0}};

    LAB_ASSERT(!LAB_FAILED(assets->err));

    LAB_TexAlloc* alloc = &assets->alloc;
    LAB_TexAtlas* atlas = assets->atlas;
    size_t cell_size = atlas->cell_size;

    assets->err = LAB_Assets_ValidateTexture(assets, v);
    if(LAB_FAILED(assets->err)) return error_result;

    size_t size_cells = v.w / cell_size;

    LAB_TexAlloc_Result result = LAB_TexAlloc_Add(alloc, size_cells);
    if(!result.success) return error_result;

    LAB_TexRect alloc_rect = LAB_Box2Z_FromOriginAndSize(result.topleft_cell, (LAB_Vec2Z) {size_cells, size_cells});

    if(!LAB_TexAtlas_Alloc(atlas, alloc_rect)) return error_result;

    return alloc_rect;
}

LAB_TexRect LAB_Assets_NewComposedTexture(LAB_Assets* assets,
                                          const LAB_TextureComposite composite[/*NULLTERM*/])
{
    const LAB_TexRect error_result = {{0, 0}, {0, 0}};
    if(LAB_FAILED(assets->err)) return error_result;

    // load first texture to determine size
    LAB_ASSERT(composite[0].resource_name != NULL);
    LAB_ImageView img_view = LAB_Assets_LoadTexture(assets, composite[0].resource_name);
    if(LAB_FAILED(assets->err)) return error_result;

    LAB_TexRect alloc_rect = LAB_Assets_AllocTexture_For(assets, img_view);
    if(!LAB_TexRect_IsOk(alloc_rect)) return error_result;

    LAB_ImageView tex_view = LAB_TexAtlas_ClipImageView(assets->atlas, alloc_rect);

    LAB_Image_Tint2(tex_view, img_view,
                    composite[0].black_tint, composite[0].white_tint);

    for(int i = 1; composite[i].resource_name != NULL; ++i)
    {
        img_view = LAB_Assets_LoadTexture(assets, composite[i].resource_name);
        LAB_Assets_ValidateTexture_Fit(assets, alloc_rect, img_view);
        LAB_Image_Blit2(tex_view, img_view,
                        composite[i].black_tint, composite[i].white_tint);
    }

    return alloc_rect;
}


LAB_TexRect LAB_Assets_NewTexture(LAB_Assets* assets,
                                  const char* resource_name)
{
    return LAB_Assets_NewComposedTexture(assets, (LAB_TextureComposite[]){
        {resource_name, LAB_RGBI(0x000000), LAB_RGBI(0xffffff)},
        {NULL, 0, 0}
    });
}

LAB_TexRect LAB_Assets_NewTintedTexture(LAB_Assets* assets,
                                        const char* resource_name,
                                        LAB_Color black_color, LAB_Color white_color)
{
    return LAB_Assets_NewComposedTexture(assets, (LAB_TextureComposite[]){
        {resource_name, black_color, white_color},
        {NULL, 0, 0}
    });
}


LAB_Model* LAB_Assets_NewModel(LAB_Assets* assets)
{
    return LAB_ModelSet_NewModel(&assets->err, assets->models);
}
