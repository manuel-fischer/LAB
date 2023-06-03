#include "LAB_asset_manager.h"

#include "LAB_stdinc.h"
#include "LAB_gl.h"
#include "LAB_sdl.h"

#include "LAB_opt.h"
#include "LAB_color.h"
#include "LAB_memory.h"
#include "LAB_debug.h"
#include "LAB_image.h"
#include "LAB_util.h"

#include <SDL2/SDL_image.h>


#define HTL_PARAM LAB_ASSET_MGR_TBL
#include "HTL/hasharray.t.c"
#undef HTL_PARAM


bool LAB_AssetMgr_Create(LAB_AssetMgr* mgr, const LAB_AssetMgr_Behavior* behavior, void* user)
{
    mgr->behavior = behavior;
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

    if(mgr->behavior->destroy_resource)
        for(size_t i = 0; i < mgr->table.size; ++i)
        {
            mgr->behavior->destroy_resource(mgr->user, LAB_AssetMgr_GetByIndex(mgr, i));
        }

    LAB_AssetMgrTbl_Destroy(&mgr->table);
    LAB_Free(mgr->resource_vector);
}

void* LAB_AssetMgr_GetByName(LAB_AssetMgr* mgr, const char* resource_name)
{
    LAB_AssetMgrKey key;
    key.hash = LAB_StrHash(resource_name);
    key.str  = resource_name;
    LAB_AssetMgrEntry* entry = LAB_AssetMgrTbl_PutAlloc(&mgr->table, &key);

    if(entry == NULL) return NULL;
    if(LAB_AssetMgrTbl_IsEntry(&mgr->table,entry))
        return LAB_AssetMgr_GetByIndex(mgr, entry->resource_index);

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
            else new_cap <<= 1;

            void* new_data = LAB_ReallocN(mgr->resource_vector, new_cap, mgr->behavior->resource_size);
            if(new_data == NULL)
                goto fail;

            mgr->resource_capacity = new_cap;
            mgr->resource_vector = new_data;
        }

        if(!mgr->behavior->load_resource(mgr->user, resource_name, LAB_AssetMgr_GetByIndex(mgr, index)))
            goto fail;

        // SUCCESS
        entry->key.hash = key.hash;
        entry->key.str  = resource_name_dup;
        entry->resource_index = index;

        return LAB_AssetMgr_GetByIndex(mgr, index);
    }

fail:
    LAB_Free((void*)resource_name_dup);
    LAB_AssetMgrTbl_Discard(&mgr->table, entry);
    return NULL;
}





























