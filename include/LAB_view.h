#pragma once
#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_fps_graph.h"
#include "LAB_gui_manager.h"
#include "LAB_model.h" // LAB_Triangle
#include "LAB_model_order.h" // LAB_TriangleOrder
#include "LAB_world.h"
#include "LAB_memory.h"
#include "LAB_view_mesh.h"
#include <SDL2/SDL_ttf.h>

#define LAB_VIEW_QUERY_IMMEDIATELY 0

/**
 *  Hooks
 *  - LAB_ViewChunkProc
 *  - LAB_ViewRenderProc
 *  - LAB_ViewTick
 */

// DEPEND
//typedef struct LAB_World LAB_World;
typedef struct LAB_Chunk LAB_Chunk;
typedef struct LAB_Window LAB_Window;
typedef int LAB_ChunkUpdate;
// END DEPEND

#define LAB_CHUNK_DIRTY_BLOCKS
#define LAB_CHUNK_DIRTY_LIGHT

typedef struct LAB_ViewChunkEntry
{
    int x, y, z;


    /*size_t mesh_count, mesh_capacity;
    LAB_Triangle* mesh;
    LAB_TriangleOrder* mesh_order;*

    unsigned vbo;*/
    //unsigned vbos[LAB_RENDER_PASS_COUNT];

    LAB_View_Mesh render_passes[LAB_RENDER_PASS_COUNT];
    LAB_TriangleOrder* mesh_order; // for LAB_RENDER_PASS_ALPHA

    unsigned dirty:2,       // chunk needs update
             exist:1,       // chunk exists in world
             visible:1,     // chunk is visible when in sight
             do_query:1,    // visibility unknown, a query should be submitted
             upload_vbo:1,  // vbos changed, need reupload, gets set to 0 in last render pass

             occupied:1;    // table entry occupied

    unsigned pad:1;
    unsigned visible_faces:6; // the faces that are currently rendered

    #if !LAB_VIEW_QUERY_IMMEDIATELY
    unsigned query_id; // 0 for no query done in the last frame
                       // gets generated in OrderQueryBlock
                       // gets deleted in FetchQueryBlock
                       // TODO: maybe use an array and allocate
                       //       a fixed number of query objects
    #endif
} LAB_ViewChunkEntry;



LAB_INLINE LAB_ChunkPos LAB_MakeChunkPos(int x, int y, int z)
{
    LAB_ChunkPos pos = {x, y, z};
    return pos;
}

#define LAB_VIEW_CHUNK_TBL_NAME              LAB_View_ChunkTBL
#define LAB_VIEW_CHUNK_TBL_KEY_TYPE          LAB_ChunkPos
#define LAB_VIEW_CHUNK_TBL_ENTRY_TYPE        LAB_ViewChunkEntry
#define LAB_VIEW_CHUNK_TBL_KEY_FUNC(e)       (LAB_MakeChunkPos((e)->x, (e)->y, (e)->z))
#define LAB_VIEW_CHUNK_TBL_HASH_FUNC(k)      LAB_ChunkPosHash(k)
#define LAB_VIEW_CHUNK_TBL_COMP_FUNC(k1,k2)  LAB_ChunkPosComp(k1, k2)

#define LAB_VIEW_CHUNK_TBL_EMPTY_FUNC(e)     (!(e)->occupied)

#define LAB_VIEW_CHUNK_TBL_CALLOC            LAB_Calloc
#define LAB_VIEW_CHUNK_TBL_FREE              LAB_Free

/*
 * through profiling a load factor of 1/2 is better than this ratio:
 *
 *   #define LAB_VIEW_CHUNK_TBL_LOAD_NUM          3
 *   #define LAB_VIEW_CHUNK_TBL_LOAD_DEN          4
 *
 * It might be better to use a sorted array and then use binary search on that,
 * because already pointers to the chunk entries are stored in a sorted array to
 * be used for sorted rendering.
 *  This might allow to implement functions like FindNear, which starts searching
 * at a given position
 *
 * TODO: sorted array datatype in HTL
 *   - sorted by a key
 *   - reordering by changing key func
 *   - inserting multiple elements at once
 *     - iteration backwards and copying elements from
 *       the old back to the new back
 *   - there should be an insertion without sorting, because
 *     the array might be reordered by a completely different keyfunction
 *     afterwards -> InsertAndReorder
 * -> sorted by distance to the camera
 *    - manhattan distance (city block distance) is suitable
 *      - faster to compute than euclidian distance
 *      - abs -- possibly branchful
 *      - ordering of neighboring chunks is kept
 *    - Chebyshev distance (max component distance) >>not<< suitable
 *      - max, abs -- possibly branchful
 *      - ordering of neighboring chunks not always kept
 *        - chunks next to each other tangentially to the camera have
 *          equal ordering
 *
*/
#define LAB_VIEW_CHUNK_TBL_LOAD_NUM          1
#define LAB_VIEW_CHUNK_TBL_LOAD_DEN          2
#define LAB_VIEW_CHUNK_TBL_GROW_FACTOR       2
#define LAB_VIEW_CHUNK_TBL_INITIAL_CAPACITY  16

#define LAB_VIEW_CHUNK_TBL_CACHE_LAST        1

#define HTL_PARAM LAB_VIEW_CHUNK_TBL
#include "HTL_hasharray.t.h"
#undef HTL_PARAM


#define LAB_VIEW_USE_VBO           1u
#define LAB_VIEW_SHOW_HUD          2u
#define LAB_VIEW_SHOW_FPS_GRAPH    4u
#define LAB_VIEW_FLAT_SHADE        8u
#define LAB_VIEW_BRIGHTER         16u

#define LAB_VIEW_SHOW_CHUNK_GRID  32u

// Coordinate info
typedef struct LAB_ViewInfo
{
    int x, y, z;
    unsigned gl_texture;
    SDL_Surface* surf;
} LAB_ViewInfo;

typedef struct LAB_ViewSortedChunkEntry
{
    LAB_ViewChunkEntry* entry;
    float distance;
} LAB_ViewSortedChunkEntry;

typedef struct LAB_View
{
    // TODO move to entity class
    // View position
    double x, y, z;
    // View angle
    double ax, ay, az; // TODO cache view direction

    // Velocity
    double vx, vy, vz;

    int on_ground;

    // Cache
    LAB_View_ChunkTBL chunks;

    LAB_ViewSortedChunkEntry* sorted_chunks;


    LAB_World* world;

    uint32_t flags;
    uint32_t preload_dist,
             render_dist,
             keep_dist;
    // TODO: query enable/disable setting

    uint32_t max_update;

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
bool LAB_ViewChunkKeepProc(void* user, LAB_World* world, int x, int y, int z);
void LAB_ViewRenderProc(void* user, LAB_Window* window);

LAB_ViewChunkEntry* LAB_ViewGetChunkEntry(LAB_View* view, int x, int y, int z);
void LAB_ViewInvalidateEverything(LAB_View* view, int free_buffers);

void LAB_ViewGetDirection(LAB_View* view, LAB_OUT float dir[3]);

void LAB_ViewTick(LAB_View* view, uint32_t delta_ms);
void LAB_ViewLoadNearChunks(LAB_View* view);



void LAB_ViewRenderHud(LAB_View* view);


LAB_ViewChunkEntry* LAB_ViewFindChunkEntry(LAB_View* view, int x, int y, int z);
LAB_ViewChunkEntry* LAB_ViewNewChunkEntry(LAB_View* view, int x, int y, int z);


bool LAB_View_IsChunkInSight(LAB_View* view, int cx, int cy, int cz);
