#pragma once
/** \file LAB_asset_manager.h
 *
 *  Global asset management
 */

#include <SDL2/SDL_surface.h>
#include <string.h>
#include "LAB_util.h"
#include "LAB_htl_config.h"

/** \def LAB_TILE_SIZE
 *
 *  size in pixels of a texture tile
 *
 *  \todo remove this
 */
#define LAB_TILE_SIZE 32

/**
 *  boolean flag if mipmaps are enabled
 *
 *  \todo remove this
 */
#define LAB_MIPMAPS 1




typedef struct LAB_AssetMgrKey
{
    size_t hash;
    const char* str; // owning inside the table
                     // not owning outside
} LAB_AssetMgrKey;

typedef struct LAB_AssetMgrEntry
{
    LAB_AssetMgrKey key;
    size_t resource_index;
} LAB_AssetMgrEntry;

#define LAB_ASSET_MGR_TBL_NAME             LAB_AssetMgrTbl
#define LAB_ASSET_MGR_TBL_KEY_TYPE         LAB_AssetMgrKey*
#define LAB_ASSET_MGR_TBL_ENTRY_TYPE       LAB_AssetMgrEntry
#define LAB_ASSET_MGR_TBL_KEY_FUNC(e)      (&(e)->key)
#define LAB_ASSET_MGR_TBL_HASH_FUNC(k)     ((k)->hash)
#define LAB_ASSET_MGR_TBL_COMP_FUNC(k1,k2) (((k1)->hash == (k2)->hash && strcmp((k1)->str, (k2)->str) == 0)?0:1)
#define LAB_ASSET_MGR_TBL_EMPTY_FUNC(e)    ((e)->key.str == NULL)

#define HTL_PARAM LAB_ASSET_MGR_TBL
#include "HTL_hasharray.t.h"
#undef HTL_PARAM


/**
 *  Specify Behavior for a LAB_AssetMgr, specific to a
 *  resource type.
 */
typedef struct LAB_AssetMgr_Behavior
{
    size_t resource_size;
    bool (*load_resource)(void* user, const char* resource_name, void* resource);
    void (*destroy_resource)(void* user, void* resource);
} LAB_AssetMgr_Behavior;


/**
 *  Used for loading resources, after the loading process this
 *  can be disconnected from the resources. This applies when
 *  the user pointer is used to reference an other data-structure
 *  that composes the resources when calling load_resource.
 *  destroy_resource does nothing in this case (set to NULL), because the data
 *  is owned by the other data-structure and the mgr only wraps
 *  around that data structure providing high level name lookup.
 *
 *  LAB_AssetMgrs can be used concatenatedly, that is when resources
 *  are built from other resources. Eg. block models use block textures,
 *  There is an asset manager that loads the models, an other loads the
 *  textures that are used by a specific model. If multiple models use
 *  the same texture, both use the same one.
 *
 *  The main idea behind the asset manager is that resources with the same
 *  name are only loaded once but used eventually multiple times.
 *
 *  Once a resource has been loaded it is kept in the manager until it
 *  gets destroyed.
 *
 *  a resource entry in resource_vector can be an index/pointer into another
 *  array, some coordinates (for a texture in a texture map) or the data itself.
 *
 *  But they need to be kept constant, there is no way these change directly.
 *  Because resource_vector might move around in the address space, because
 *  of reallocation. So you shouldn't keep the pointers around that are provided
 *  by the resource parameter.
 */
typedef struct LAB_AssetMgr
{
    const LAB_AssetMgr_Behavior* behavior;
    void* user;

    LAB_AssetMgrTbl table;

    size_t resource_capacity;
    void*  resource_vector;

} LAB_AssetMgr;




bool LAB_AssetMgr_Create(LAB_AssetMgr* mgr, const LAB_AssetMgr_Behavior* behavior, void* user);
void LAB_AssetMgr_Destroy(LAB_AssetMgr* mgr);

void* LAB_AssetMgr_GetByName(LAB_AssetMgr* mgr, const char* resource_name);

#define LAB_AssetMgr_GetByIndex(mgr, index) LAB_PTR_OFFSET((mgr)->resource_vector, index, (mgr)->behavior->resource_size)

#define LAB_AssetMgr_GetByIndex_Typed(mgr, index, type) LAB_CORRECT_IF( \
    (mgr)->ressource_size == sizeof(type), \
    (type*)LAB_PTR_OFFSET((mgr)->ressource_vector, index, sizeof(type)) \
)
