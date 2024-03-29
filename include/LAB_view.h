#pragma once
#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB/gui/manager.h"
#include "LAB_model.h" // LAB_Triangle
#include "LAB_model_order.h" // LAB_TriangleOrder
#include "LAB_world.h"
#include "LAB_memory.h"
#include "LAB_view_mesh.h"
#include "LAB_perf_info.h"
#include <SDL2/SDL_ttf.h>

#include "LAB_texture_atlas.h"

#include "LAB_view_chunk.h"
#include "LAB_view_array.h"
#include "LAB_htl_config.h"
#include "LAB_timelib.h"

#include "LAB_view_renderer.h"
#include "LAB_gl_types.h"
#include "LAB_view_render_box.h"
#include "LAB_view_render_sky.h"
#include "LAB_view_render_surface.h"
#include "LAB_view_pending_queries.h"

#include "LAB_vec2.h"
#include "LAB_vec3.h"


/*
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




/**
 *  Hooks
 *  - LAB_ViewChunkProc
 *  - LAB_ViewRenderProc
 *  - LAB_ViewTick
 * TODO: put them into an individual struct
 */

// DEPEND
//typedef struct LAB_World LAB_World;
typedef struct LAB_Chunk LAB_Chunk;
typedef struct LAB_Window LAB_Window;
typedef int LAB_ChunkUpdate;
// END DEPEND

#define LAB_CHUNK_DIRTY_BLOCKS
#define LAB_CHUNK_DIRTY_LIGHT



LAB_INLINE LAB_ChunkPos LAB_MakeChunkPos(int x, int y, int z)
{
    LAB_ChunkPos pos = {x, y, z};
    return pos;
}

#define LAB_VIEW_CHUNK_TBL_NAME              LAB_View_ChunkTBL
#define LAB_VIEW_CHUNK_TBL_KEY_TYPE          LAB_ChunkPos
#define LAB_VIEW_CHUNK_TBL_ENTRY_TYPE        LAB_ViewChunkEntry*
#define LAB_VIEW_CHUNK_TBL_KEY_FUNC(e)       (LAB_MakeChunkPos((*(e))->x, (*(e))->y, (*(e))->z))
#define LAB_VIEW_CHUNK_TBL_HASH_FUNC(k)      LAB_ChunkPosHash(k)
#define LAB_VIEW_CHUNK_TBL_COMP_FUNC(k1,k2)  LAB_ChunkPosComp(k1, k2)
#define LAB_VIEW_CHUNK_TBL_EMPTY_FUNC(e)     (*(e) == NULL)


#define HTL_PARAM LAB_VIEW_CHUNK_TBL
#include "HTL/hasharray.t.h"
#undef HTL_PARAM


//#define LAB_VIEW_USE_VBO           1u
#define LAB_VIEW_SHOW_HUD          2u
#define LAB_VIEW_SHOW_FPS_GRAPH    4u
#define LAB_VIEW_FLAT_SHADE        8u
//#define LAB_VIEW_BRIGHTER         16u

#define LAB_VIEW_SHOW_CHUNK_GRID  32u

// Coordinate info
typedef struct LAB_ViewCoordInfo
{
    int x, y, z;
    LAB_GL_Texture tex;
    LAB_Vec2Z tex_size;
    SDL_Surface* surf;
} LAB_ViewCoordInfo;

typedef struct LAB_ViewSortedChunkEntry
{
    LAB_ViewChunkEntry* entry;
    float distance;
} LAB_ViewSortedChunkEntry;


typedef struct LAB_ViewConfig
{
    uint32_t flags;
    uint32_t preload_dist,
             render_dist,
             keep_dist;
    // TODO: query enable/disable setting

    // Limits
    uint32_t max_update;

    uint32_t max_unload;

    uint32_t load_amount;
    uint32_t empty_load_amount;

    float exposure, saturation;
} LAB_ViewConfig;


typedef struct LAB_View
{
    // TODO move to entity class
    LAB_Vec3D pos;
    LAB_Vec3D angle;
    LAB_Vec3D velocity;

    double fov_factor;

    bool on_ground;

    uint32_t time_ms;

    // Cache
    //LAB_View_ChunkTBL chunks;
    //SDL_mutex* tbl_mutex;

    // sorted from near to far chunks
    //size_t sorted_chunks_capacity;
    //LAB_ViewSortedChunkEntry* sorted_chunks;

    LAB_ViewArray chunk_array;

    LAB_World* world;

    LAB_ViewConfig cfg;

    LAB_ViewCoordInfo info;

    struct
    {
        LAB_GL_Texture tex;
        LAB_Vec2I size;
    } crosshair;

    struct
    {
        LAB_GL_Texture tex;
        LAB_Vec2Z tex_size;
        bool reupload;
        SDL_Surface* surf;
    } stats_display;

    struct
    {
        size_t recovered_count;
        size_t deleted;
    } stats;

    int w, h; // window size

    LAB_PerfInfo* perf_info;
    struct LAB_GameServer* server; // TODO remove

    LAB_GuiManager gui_mgr;


    LAB_TexAtlas* atlas;

    LAB_Mat4F projection_mat, modelview_mat, modlproj_mat;

    LAB_GL_UInt upload_time_query;
    LAB_Nanos upload_time;
    size_t upload_amount;
    size_t current_upload_amount;

    size_t update_pointer;
    size_t delete_index;

    LAB_ViewRenderer renderer;
    LAB_SkyRenderer sky_renderer;
    LAB_BoxRenderer box_renderer;
    LAB_SurfaceRenderer surface_renderer;

    LAB_View_PendingQueries frames;

} LAB_View;

extern const LAB_IView LAB_view_interface;

/**
 *  Create view, with given world
 */
bool LAB_View_Create(LAB_View* view, LAB_World* world, LAB_TexAtlas* atlas);

/**
 *  Destruct view
 *  The view can be filled with 0 bytes
 */
void LAB_View_Destroy(LAB_View* view);
void LAB_View_SetWorld(LAB_View* view, LAB_World* world);

void LAB_View_Clear(LAB_View* view);


void LAB_ViewRenderProc(void* user, LAB_Window* window);


void LAB_ViewRender(LAB_View* view);

void LAB_ViewInvalidateEverything(LAB_View* view, int free_buffers);

LAB_Vec3F LAB_View_GetDirection(LAB_View* view);

void LAB_ViewTick(LAB_View* view, uint32_t delta_ms);
void LAB_ViewLoadNearChunks(LAB_View* view);



void LAB_ViewRenderHud(LAB_View* view);


LAB_ViewChunkEntry* LAB_ViewFindChunkEntry(LAB_View* view, int x, int y, int z);
LAB_ViewChunkEntry* LAB_ViewNewChunkEntry(LAB_View* view, int x, int y, int z);



/**
 *  Superset of Completely/Partly in Frustum
 */
bool LAB_View_IsChunkInSight(LAB_View* view, int cx, int cy, int cz);
/**
 *  Subset of InSight, superset of Completely in Frustum
 */
bool LAB_View_IsChunkPartlyInFrustum(LAB_View* view, int cx, int cy, int cz);
/**
 *  Subset of InSight, sub of Partly in Frustum
 */
bool LAB_View_IsChunkCompletelyInFrustum(LAB_View* view, int cx, int cy, int cz);


// TODO use Create_Zero convention when an object is assumed to be created from zero memory
//      use Destroy_Zero convention when the destruction of an object results in
//          the object being zeroed out
void LAB_ViewCoordInfo_Create_Zero(LAB_ViewCoordInfo* info);
void LAB_ViewCoordInfo_Destroy(LAB_ViewCoordInfo* info);



void LAB_View_ShowGuiMenu(LAB_View* view);
void LAB_View_ShowGuiInventory(LAB_View* view, LAB_BlockID* block);
