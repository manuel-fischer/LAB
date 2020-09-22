#pragma once
#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_fps_graph.h"
#include "LAB_gui_manager.h"
#include "LAB_model.h" // LAB_Triangle
#include <SDL2/SDL_ttf.h>

/**
 *  Hooks
 *  - LAB_ViewChunkProc
 *  - LAB_ViewRenderProc
 *  - LAB_ViewTick
 */

// DEPEND
typedef struct LAB_World LAB_World;
typedef struct LAB_Chunk LAB_Chunk;
typedef struct LAB_Window LAB_Window;
typedef int LAB_ChunkUpdate;
// END DEPEND

#define LAB_CHUNK_DIRTY_BLOCKS
#define LAB_CHUNK_DIRTY_LIGHT

typedef struct LAB_ViewChunkEntry
{
    int x, y, z;


    size_t mesh_count, mesh_capacity;
    LAB_Triangle* mesh;

    unsigned dirty:2, exist:1, visible:1, do_query:1;

    unsigned vbo;
    unsigned query_id; // 0 for no query done in the last frame
                       // gets generated in OrderQueryBlock
                       // gets deleted in FetchQueryBlock
                       // TODO: maybe use an array and allocate
                       //       a fixed number of query objects

} LAB_ViewChunkEntry;







/*#define LAB_VIEW_CHUNK_MAP_NAME             LAB_ViewChunkMap
#define LAB_VIEW_CHUNK_MAP_KEY_TYPE         LAB_ChunkPos
#define LAB_VIEW_CHUNK_MAP_VALUE_TYPE       LAB_ViewChunkEntry
#define LAB_VIEW_CHUNK_MAP_HASH_FUNC        LAB_ChunkPosHash
#define LAB_VIEW_CHUNK_MAP_COMP_FUNC        LAB_ChunkPosComp
#define LAB_VIEW_CHUNK_MAP_CALLOC           LAB_Calloc
#define LAB_VIEW_CHUNK_MAP_FREE             LAB_Free
#define LAB_VIEW_CHUNK_MAP_LOAD_NUM         3
#define LAB_VIEW_CHUNK_MAP_LOAD_DEN         4
#define LAB_VIEW_CHUNK_MAP_INITIAL_CAPACITY 16
#define LAB_VIEW_CHUNK_MAP_NULL_REPR        entry->value->used != 0

#define HTL_PARAM LAB_VIEW_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM*/


#define LAB_VIEW_USE_VBO           1u
#define LAB_VIEW_SHOW_HUD          2u
#define LAB_VIEW_SHOW_FPS_GRAPH    4u
#define LAB_VIEW_FLAT_SHADE        8u
#define LAB_VIEW_BRIGHTER         16u

typedef struct LAB_ViewInfo
{
    int x, y, z;
    unsigned gl_texture;
    SDL_Surface* surf;
} LAB_ViewInfo;

typedef struct LAB_View
{
    // View position
    double x, y, z;
    // View angle
    double ax, ay, az;

    // Velocity
    double vx, vy, vz;

    // Cache
    size_t chunk_count, chunk_capacity;
    LAB_ViewChunkEntry* chunks;

    //LAB_ViewChunkMap chunks;

    LAB_World* world;

    uint32_t flags;
    uint32_t preload_dist,
             render_dist,
             keep_dist;
    // TODO: query enable/disable setting

    uint32_t max_update,
             rest_update;

    uint32_t load_amount;

    LAB_ViewInfo info;
    int w, h; // window size

    LAB_FpsGraph fps_graph;
    LAB_GuiManager gui_mgr;
} LAB_View;


/**
 *  Create view, with given world
 */
bool LAB_ConstructView(LAB_View* view, LAB_World* world);

/**
 *  Destruct view
 *  The view can be filled with 0 bytes
 */
void LAB_DestructView(LAB_View* view);


void LAB_ViewChunkProc(void* user, LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update);
void LAB_ViewRenderProc(void* user, LAB_Window* window);

LAB_ViewChunkEntry* LAB_ViewGetChunkEntry(LAB_View* view, int x, int y, int z);
void LAB_ViewInvalidateEverything(LAB_View* view, int free_buffers);

void LAB_ViewGetDirection(LAB_View* view, LAB_OUT float dir[3]);

void LAB_ViewTick(LAB_View* view, uint32_t delta_ms);
void LAB_ViewLoadNearChunks(LAB_View* view);



void LAB_ViewRenderHud(LAB_View* view);


LAB_ViewChunkEntry* LAB_ViewFindChunkEntry(LAB_View* view, int x, int y, int z);
LAB_ViewChunkEntry* LAB_ViewNewChunkEntry(LAB_View* view, int x, int y, int z);



