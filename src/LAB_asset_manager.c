#include "LAB_asset_manager.h"

#include "LAB_stdinc.h"
#include "LAB_gl.h"
#include "LAB_sdl.h"

#include "LAB_opt.h"
#include "LAB_color.h"
#include "LAB_memory.h"
#include "LAB_debug.h"
#include "LAB_image.h"

#include <SDL2/SDL_image.h>

SDL_Surface* LAB_block_terrain = NULL;
unsigned     LAB_block_terrain_gl_id = 0;

// TODO remove this
#include "LAB_texture_atlas.h"
LAB_TexAtlas LAB_block_atlas;

LAB_STATIC void LAB_GL_GenerateMipmap2D(size_t w, size_t h, LAB_Color* data, int num_mipmaps);

void LAB_InitAssets(void)
{
    static bool init = 0;
    LAB_ASSERT(init == 0);
    LAB_ASSERT_OR_ABORT(LAB_TexAtlas_Create(&LAB_block_atlas, 32));
    LAB_Temp_RecreateTerrain(&LAB_block_atlas);

    LAB_block_terrain = LAB_ImageData2SDL(LAB_block_atlas.w, LAB_block_atlas.h, LAB_block_atlas.data);

    LAB_TexAtlas_MakeMipmap(&LAB_block_atlas);


    glEnable(GL_TEXTURE_2D);
    LAB_TexAtlas_Upload2GL(&LAB_block_atlas);
    LAB_block_terrain_gl_id = LAB_block_atlas.gl_id;

    LAB_TexAtlas_LoadTexMatrix(&LAB_block_atlas);


    #if 0
    // TODO: move global state into its own structure
    if(LAB_block_terrain) return;

    SDL_Surface* img = LAB_ImageLoad("assets/terrain.png");
    if(!img) return;

    glEnable(GL_TEXTURE_2D);
    LAB_GL_ALLOC(glGenTextures, 1, &LAB_block_terrain_gl_id);
    glBindTexture(GL_TEXTURE_2D, LAB_block_terrain_gl_id);

    #if LAB_MIPMAPS
    // Mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    const int num_mipmaps = 5;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_mipmaps);

    LAB_Fix0Alpha(img->w, img->h, (LAB_Color*)img->pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    //glGenerateMipmap(GL_TEXTURE_2D);
    LAB_GL_GenerateMipmap2D(img->w, img->h, img->pixels, num_mipmaps);
    #else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    #endif

    glMatrixMode(GL_TEXTURE);
    glScalef((float)LAB_TILE_SIZE / (float)img->w, (float)LAB_TILE_SIZE / (float)img->h, 1);

    LAB_block_terrain = img;
    LAB_GL_CHECK();
    #endif
}

void LAB_QuitAssets(void)
{
    //LAB_GL_FREE(glDeleteTextures, 1, &LAB_block_terrain_gl_id);
    LAB_SDL_FREE(SDL_FreeSurface, &LAB_block_terrain);

    /*LAB_block_terrain_gl_id = 0;
    LAB_block_terrain = NULL;*/
    LAB_TexAtlas_Destroy(&LAB_block_atlas);
}

LAB_STATIC void LAB_GL_GenerateMipmap2D(size_t w, size_t h, LAB_Color* data, int num_mipmaps)
{
    //LAB_Color* tmp_buf = LAB_Malloc(w*h*sizeof*tmp_buf);
    LAB_Color* tmp_buf = LAB_Malloc((w/2*h/2+w/4*w/4)*sizeof*tmp_buf);
    LAB_Color* tmp_buf_2 = tmp_buf+(w/2*h/2);

    LAB_Color* new_data = tmp_buf;

    for(int i = 1; i <= num_mipmaps; ++i)
    {
        w/=2; h/=2;
        LAB_MakeMipmap2D(w, h, data, new_data);
        if(i != num_mipmaps) LAB_Fix0Alpha(w, h, new_data);
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, new_data);
        LAB_ImageSave_Fmt(w, h, new_data, "dbg_terrain_%i.png", i);
        //data = new_data;
        if(new_data == tmp_buf)
            new_data = tmp_buf_2, data = tmp_buf;
        else
            data = tmp_buf_2, new_data = tmp_buf;
    }

    LAB_Free(tmp_buf);
}












#define HTL_PARAM LAB_ASSET_MGR_TBL
#include "HTL_hasharray.t.c"
#undef HTL_PARAM



bool LAB_AssetMgr_Create(LAB_AssetMgr* mgr, const LAB_AssetMgr_Behavior* behavior, void* user)
{
    memcpy(&mgr->behavior, behavior, sizeof *behavior);
    mgr->user = user;
    LAB_AssetMgrTbl_Create(&mgr->table);
    mgr->resource_capacity = 0;
    mgr->resource_vector   = NULL;

    return true;
}

void LAB_AssetMgr_Destroy(LAB_AssetMgr* mgr)
{
    LAB_AssetMgrEntry* entry;
    HTL_HASHARRAY_EACH(LAB_AssetMgrTbl, &mgr->table, entry,
    {
        LAB_Free((void*)entry->key.str);
    });

    if(mgr->behavior.destroy_resource)
        for(size_t i = 0; i < mgr->table.size; ++i)
        {
            mgr->behavior.destroy_resource(mgr->user, LAB_AssetMgr_GetResource(mgr, i));
        }

    LAB_AssetMgrTbl_Destroy(&mgr->table);
}

void* LAB_AssetMgr_Load(LAB_AssetMgr* mgr, const char* resource_name)
{
    LAB_AssetMgrKey key;
    key.hash = LAB_StrHash(resource_name);
    key.str  = resource_name;
    LAB_AssetMgrEntry* entry = LAB_AssetMgrTbl_PutAlloc(&mgr->table, &key);

    if(entry == NULL) return NULL;
    if(LAB_AssetMgrTbl_IsEntry(&mgr->table,entry))
        return LAB_AssetMgr_GetResource(mgr, entry->resource_index);

    const char* resource_name_dup = LAB_StrDup(resource_name);
    if(resource_name_dup == NULL)
        goto fail;


    {
        size_t index = mgr->table.size-1;
        LAB_ASSUME(index <= mgr->resource_capacity);
        if(index == mgr->resource_capacity)
        {
            size_t new_cap = mgr->resource_capacity;
            if(new_cap == 0) new_cap = 1;

            void* new_data = LAB_ReallocN(mgr->resource_vector, new_cap, mgr->behavior.resource_size);
            if(new_data == NULL)
                goto fail;

            mgr->resource_capacity = new_cap;
            mgr->resource_vector = new_data;
        }

        if(!mgr->behavior.load_resource(mgr->user, resource_name, LAB_AssetMgr_GetResource(mgr, index)))
            goto fail;

        // SUCCESS
        entry->key.hash = key.hash;
        entry->key.str  = resource_name_dup;

        return LAB_AssetMgr_GetResource(mgr, index);
    }

fail:
    LAB_Free((void*)resource_name_dup);
    LAB_AssetMgrTbl_Discard(&mgr->table, entry);
    return NULL;
}





























