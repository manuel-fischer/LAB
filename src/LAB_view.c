#include "LAB_view.h"

#include "LAB_view_mesh_build.h"

#include "LAB_debug_options.h"

#include "LAB_memory.h"
#include "LAB_error.h"
#include "LAB_math.h"
#include "LAB_debug.h"
#include "LAB_obj.h"

#include "LAB_world.h"
#include "LAB_block.h"
#include "LAB_window.h"
#include "LAB_gl.h"
#include "LAB_sdl.h"
#include "LAB_asset_manager.h"

#include "LAB_render_pass.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>

#include "LAB_perf.h"
#include "LAB_bits.h"
#include "LAB_opt.h"
#include "LAB_util.h"
#include "LAB_attr.h"

#include "LAB_vec_algo.h"

#include "LAB/gui.h"
#include "LAB/gui/component.h"
#include "LAB/gui/menu.h"
#include "LAB/gui/inventory.h"
#include "LAB_inventory.h"

#include "LAB_image.h"
#include "LAB_color_defs.h"

#include "LAB_view_render_box.h"

#include "LAB_loop.h"

#include "LAB_chunk_neighborhood.h" // TODO remove

#include "LAB_game_server.h" // TODO remove

#define HTL_PARAM LAB_VIEW_CHUNK_TBL
#include "HTL/hasharray.t.c"
#undef HTL_PARAM

#if 1
#define LAB_glGenQueries(count, ids) do { for(int i = 0; i < (count); ++i) (ids)[i] = 0; } while(0)
#define LAB_glDeleteQueries(count, ids) ((void)0)
#define LAB_glGetQueryObjectuiv(id, pname, param) ((*param) = 1)
#else
#define LAB_glGenQueries        glGenQueries
#define LAB_glDeleteQueries     glDeleteQueries
#define LAB_glGetQueryObjectuiv glGetQueryObjectuiv
#endif

/**
 *  TODO
 *  - when the only visible blocks of a chunk are at the chunk border, the cube that is rendered for the chunk z-fights with the z data of
 *    those rendered blocks
 */


///############################


LAB_STATIC void LAB_ViewChunkProc(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
LAB_STATIC bool LAB_ViewChunkKeepProc(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
LAB_STATIC void LAB_ViewChunkUnlinkProc(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
LAB_STATIC void LAB_ViewChunkMeshProc(void* user, LAB_World* world, LAB_Chunk* chunks[27]);
LAB_STATIC LAB_Vec3D LAB_View_Position_Proc(void* user, LAB_World* world);
const LAB_IView LAB_view_interface = 
{
    .chunkview    = &LAB_ViewChunkProc,
    .chunkmesh    = &LAB_ViewChunkMeshProc,
    .chunkkeep    = &LAB_ViewChunkKeepProc,
    .chunkunlink  = &LAB_ViewChunkUnlinkProc,
    .position     = &LAB_View_Position_Proc,
};


LAB_STATIC bool LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world, unsigned visibility);
LAB_STATIC int  LAB_ViewUpdateChunks(LAB_View* view); // return updated chunks
LAB_STATIC bool LAB_ViewUpdateChunk(LAB_View* view, LAB_ViewChunkEntry* e);
LAB_STATIC int  LAB_ViewRenderChunks(LAB_View* view, LAB_RenderPass pass);
LAB_STATIC bool LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_RenderPass pass);

//LAB_STATIC bool LAB_View_HasChunkEntryVisibleNeighbors(LAB_View* view, LAB_ViewChunkEntry* e);
//LAB_STATIC bool LAB_View_IsLocalChunk(LAB_View* view, int cx, int cy, int cz);
//LAB_STATIC unsigned LAB_View_ChunkVisibility(LAB_View* view, int cx, int cy, int cz);         // inclusive for same coordinates
//LAB_STATIC unsigned LAB_View_ChunkNeighborVisibility(LAB_View* view, int cx, int cy, int cz); // exclusive for same coordinates
LAB_STATIC void LAB_View_UpdateChunkSeethrough(LAB_View* view, LAB_ViewChunkEntry* e); // exclusive for same coordinates
#if LAB_VIEW_ENABLE_QUERY
#if !LAB_VIEW_QUERY_IMMEDIATELY
LAB_STATIC void LAB_View_FetchQueryChunks(LAB_View* view);
LAB_STATIC void LAB_View_FetchQueryChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
#endif
LAB_STATIC void LAB_View_OrderQueryChunks(LAB_View* view);
LAB_STATIC void LAB_View_OrderQueryChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
#endif
LAB_STATIC void LAB_ViewRemoveDistantChunks(LAB_View* view);
LAB_STATIC void LAB_ViewDestructChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
LAB_STATIC void LAB_ViewDestructFreeChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
/** unlink, destruct and free chunk **/
LAB_STATIC void LAB_ViewUnlinkChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);


LAB_STATIC void LAB_ViewUploadVBO(LAB_View* view, LAB_View_Mesh* mesh);
LAB_STATIC bool LAB_ViewBuildChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
LAB_STATIC void LAB_View_UploadChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
LAB_STATIC void LAB_View_UploadChunks(LAB_View* view);

LAB_STATIC void LAB_View_RenderBlockSelection(LAB_View* view);

//LAB_STATIC int LAB_View_CompareChunksIndirect(const void* a, const void* b);
//LAB_STATIC void LAB_View_SortChunks(LAB_View* view, uint32_t delta_ms);

LAB_STATIC void LAB_ViewRenderInit(LAB_View* view);


LAB_STATIC bool LAB_View_SetupCrosshair(LAB_View* view)
{
    SDL_Surface* s = LAB_ImageLoad("assets/crosshair.png");
    if(!s) return false;

    LAB_GL_ActivateTexture(&view->crosshair.tex);
    LAB_GL_UploadSurf(view->crosshair.tex, s);
    view->crosshair.size = (LAB_Vec2I) {s->w, s->h};

    LAB_SDL_FREE(SDL_FreeSurface, &s);

    return true;
}

bool LAB_View_Create(LAB_View* view, LAB_World* world, LAB_TexAtlas* atlas)
{
    memset(view, 0, sizeof *view);
    view->world = world;

    view->fov_factor = 1.0;

    //LAB_View_ChunkTBL_Create(&view->entry); // not nessesary because already set to 0 above

    view->atlas = atlas;
    LAB_ViewRenderInit(view);

    LAB_GuiManager_Create(&view->gui_mgr);

    LAB_GL_DBG_CHECK();

    /*LAB_OBJ_SDL(view->tbl_mutex = SDL_CreateMutex(),
                SDL_DestroyMutex(view->tbl_mutex),*/
    LAB_OBJ(LAB_ViewArray_Create(&view->chunk_array, 5),
            LAB_ViewArray_Destroy(&view->chunk_array),
    LAB_OBJ((LAB_GL_DBG_CHECK(), LAB_ViewRenderer_Create(&view->renderer)),
            LAB_ViewRenderer_Destroy(&view->renderer),
    LAB_OBJ((LAB_GL_DBG_CHECK(), LAB_BoxRenderer_Create(&view->box_renderer)),
            LAB_BoxRenderer_Destroy(&view->box_renderer),
    LAB_OBJ((LAB_GL_DBG_CHECK(), LAB_SurfaceRenderer_Create(&view->surface_renderer)),
            LAB_SurfaceRenderer_Destroy(&view->surface_renderer),

    LAB_OBJ((view->crosshair.tex.id = 0, true),
            LAB_GL_OBJ_FREE(glDeleteTextures, &view->crosshair.tex),
    LAB_OBJ(LAB_View_SetupCrosshair(view),
            (void)0,
    {

        glGenQueries(1, &view->upload_time_query);

        return 1;
    }););););););

    return 0;
}

void LAB_View_Destroy(LAB_View* view)
{

    //LAB_Free(view->sorted_chunks);

    //SDL_DestroyMutex(view->tbl_mutex);
    LAB_GuiManager_Destroy(&view->gui_mgr);

    /*LAB_ViewChunkEntry* e;
    HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    {
        // unlinking not nessesary
        // but when the world gets destroyed all links to the world should be removed
        LAB_ViewUnlinkChunk(view, e);
        //LAB_ViewDestructFreeChunk(view, e);
    });*/
    LAB_ASSERT(view->world == NULL);
    
    glDeleteQueries(1, &view->upload_time_query);

    LAB_View_Clear(view);
    //LAB_View_ChunkTBL_Destroy(&view->chunks);
    LAB_ViewRenderer_Destroy(&view->renderer);
    LAB_ViewArray_Destroy(&view->chunk_array);

    LAB_ViewCoordInfo_Destroy(&view->info);

    LAB_SDL_FREE(SDL_FreeSurface, &view->stats_display.surf);
    LAB_GL_FREE(glDeleteTextures, 1, &view->stats_display.tex.id);
    LAB_GL_FREE(glDeleteTextures, 1, &view->crosshair.tex.id);
}


void LAB_View_SetWorld(LAB_View* view, LAB_World* world)
{
    
    /*LAB_ViewChunkEntry* e;
    HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    {
        // unlinking not nessesary
        // but when the world gets destroyed all links to the world should be removed
        LAB_ViewUnlinkChunk(view, e);
        //LAB_ViewDestructFreeChunk(view, e);
    });
    LAB_View_ChunkTBL_Clear(&view->chunks);*/

    LAB_View_Clear(view);
    view->world = world;
}


void LAB_View_Clear(LAB_View* view)
{
    /*LAB_ViewChunkEntry* e;
    HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    {
        LAB_ViewUnlinkChunk(view, e);
    });
    LAB_View_ChunkTBL_Clear(&view->chunks);*/
    /*LAB_View_ChunkTBL_Destroy(&view->chunks);
    memset(&view->chunks, 0, sizeof view->chunks);*/

    /*LAB_Free(view->sorted_chunks);
    view->sorted_chunks = 0;
    view->sorted_chunks_capacity = 0;*/

    LAB_ViewChunkEntry* e;
    LAB_ViewArray_Clear(&view->chunk_array);
    LAB_ViewArray_DeleteList_EACH(&view->chunk_array, e,
    {
        LAB_ViewUnlinkChunk(view, e);
    });
}


void LAB_ViewChunkProc(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update)
{
    // TODO remove this callback
    return;
    LAB_ASSERT(LAB_Chunk_Access(chunk));

    LAB_View* view = (LAB_View*)user;

    // ignore far away chunks
    // TODO: if neighboring chunk is inside view
    // TODO: make behavior in LAB_ViewChunkKeep proc into another function without a void* argument
    if(!LAB_ViewChunkKeepProc(user, world, chunk, x, y, z)) return;
    LAB_ViewChunkEntry* entry;
    entry = chunk->view_user;
    if(!entry)
    {
        entry = LAB_ViewFindChunkEntry(view, x, y, z);
        if(!entry) return;
        // link view entry into the world
        entry->world_chunk = chunk;
        chunk->view_user = entry;
        //LAB_ASSERT_EQ(entry->dirty, ~0);
    }
    // TODO: Chunk recovery, Chunk might be in delete queue

    entry->dirty |= update;

    //LAB_View_UpdateChunkSeethrough(view, entry);
}

bool LAB_ViewChunkKeepProc(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_View* view = (LAB_View*)user;

    LAB_Vec3I chunk_pos = (LAB_Vec3I) { x, y, z };
    LAB_Vec3I viewer_pos = LAB_Pos3D2Chunk(view->pos);

    return (uint32_t)LAB_Vec3I_DistanceSq(chunk_pos, viewer_pos) <= view->cfg.keep_dist*view->cfg.keep_dist;//+3;
}

void LAB_ViewChunkUnlinkProc(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_ASSUME(chunk->view_user);

    LAB_ViewChunkEntry* entry = chunk->view_user;
    entry->world_chunk = NULL;
    // does not need to clear chunk->view_user, because chunk gets destroyed
}


LAB_Vec3D LAB_View_Position_Proc(void* user, LAB_World* world)
{
    LAB_View* view = (LAB_View*)user;

    return view->pos;
}



LAB_STATIC
void LAB_ViewChunkMeshProc(void* vview, LAB_World* world, LAB_Chunk* chunks[27])
{
    LAB_View* view = vview;
    LAB_Chunk* chunk = chunks[LAB_NB_CENTER];
    LAB_ViewChunkEntry* e = chunk->view_user;
    if(!e) return;

    if(!LAB_Access_TryLock(&e->is_accessed)) return;

    LAB_View_UpdateChunkSeethrough(view, e);
    LAB_ViewBuildChunk(view, e);
    LAB_Access_Unlock(&e->is_accessed);
}


/**
 *  Return 1 if the chunk was available
 */
LAB_STATIC bool LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world, unsigned visibility)
{
    LAB_Chunk* chunk_neighborhood[27];

    LAB_Chunk* chunk = chunk_entry->world_chunk;
    if(chunk == NULL) return 0;

    if(!chunk->light_generated) return 0;



    for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
        chunk_entry->render_passes[i].m_size = 0;

    if(LAB_Chunk_IsEmpty(chunk)) return 1;


    LAB_GetChunkNeighbors(chunk, chunk_neighborhood);
    for(int i = 0; i < 27; ++i)
        if(chunk_neighborhood[i] && !chunk_neighborhood[i]->light_generated) return 0;

    LAB_BlockNbHood blocknbh;
    LAB_LightNbHood lightnbh;
    LAB_BlockNbHood_GetRead(chunk_neighborhood, &blocknbh);
    LAB_LightNbHood_GetRead(chunk_neighborhood, &lightnbh);

    chunk_entry->visible_faces = visibility;

    bool success = LAB_View_Mesh_BuildChunk((LAB_View_Mesh_BuildArgs)
    {
        .cfg = {
            .flat_shade = !!(view->cfg.flags&LAB_VIEW_FLAT_SHADE)
        },
        .render_passes = chunk_entry->render_passes,
        .blocknbh = &blocknbh,
        .lightnbh = &lightnbh,
        .visibility = visibility,
    });
    (void)success;
    return true;
}


LAB_STATIC void LAB_ViewUploadVBO(LAB_View* view, LAB_View_Mesh* mesh)
{
    LAB_Triangle* mesh_data = mesh->data;

    if(!mesh->vbo)
    {
        LAB_GL_CHECK();
        LAB_GL_ALLOC(glCreateBuffers, 1, &mesh->vbo);
        LAB_GL_CHECK();
    }

    mesh->vbo_size = mesh->m_size;

    LAB_GL_CHECK();
    LAB_GL_IgnoreInfo(131185);
    //glNamedBufferStorage(mesh->vbo, mesh->vbo_size*sizeof *mesh_data, mesh_data, 0);
    glNamedBufferData(mesh->vbo, mesh->vbo_size*sizeof *mesh_data, mesh_data, GL_DYNAMIC_DRAW);
    LAB_GL_ResetIgnoreInfo();
    LAB_GL_CHECK();

    view->current_upload_amount += mesh->vbo_size*sizeof *mesh_data;
}

LAB_STATIC bool LAB_View_ChunkNeedsUpdate(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    if(!LAB_View_IsChunkInSight(view, chunk_entry->x, chunk_entry->y, chunk_entry->z))
        return 0;

    LAB_ASSERT(chunk_entry->world_chunk);
    if(!chunk_entry->world_chunk->light_generated) return false;

    // TODO: enshure light update after at most 1 sec
    /*if(     (chunk_entry->dirty&LAB_CHUNK_UPDATE_LOCAL)
        ||  (chunk_entry->dirty&LAB_CHUNK_UPDATE_LIGHT)
    )*/
    if(chunk_entry->dirty)
    {
        return true;
    }
    return false;
}

LAB_STATIC bool LAB_ViewBuildChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    // TODO:
    // - Optimizations
    //   - when blocks at the chunk-border from an other chunk
    //     changed, only update the neighboring 16x16 blocks --> possibly a 16x speedup
    //   - only build chunk if all neighbors are generated
    //   - only mesh faces, that are visible
    bool chunk_available = LAB_ViewBuildMesh(view, chunk_entry, view->world, 077);
    if(!chunk_available)
    {
        //chunk_entry->dirty = ~0;
        //chunk_entry->exist =  0;
        //printf("FAILED to build mesh for %i %i %i\n", chunk_entry->x, chunk_entry->y, chunk_entry->z);
        return false;
    }
    chunk_entry->dirty = 0;
    chunk_entry->upload_vbo = 1;

    return true;
}


LAB_STATIC void LAB_View_UpdateModelOrder(LAB_View* view, LAB_ViewChunkEntry* e)
{
    // allocate/reallocate memory for mesh_order
    // if old_mesh_count wasn't NULL previously
    //     if old_mesh_count < mesh_count then
    //         add indices counting upwards
    //     else
    //         go through mesh_order [0, old_mesh_count)
    //         (this is possible because mesh_capacity never shrinks
    //         and remove triangles that have indices >= mesh_count



    LAB_View_Mesh* alpha_pass = &e->render_passes[LAB_RENDER_PASS_ALPHA];

    if(e->alpha_mesh_size != alpha_pass->vbo_size)
    {
        if(alpha_pass->vbo_size > e->alpha_mesh_capacity)
        {
            size_t alpha_mesh_capacity = e->alpha_mesh_capacity;
            if(alpha_mesh_capacity == 0) alpha_mesh_capacity = 1<<3;
            while(alpha_pass->vbo_size > alpha_mesh_capacity) alpha_mesh_capacity <<= 1;
            // Never shrinks memory
            LAB_TriangleOrder* alpha_mesh_order = LAB_ReallocN(e->alpha_mesh_order, alpha_mesh_capacity, sizeof*alpha_mesh_order);
            if(!alpha_mesh_order)
            {
                // Memory error
                // render without indices
                LAB_Free(e->alpha_mesh_order);
                e->alpha_mesh_order = NULL;
                return;
            }
            e->alpha_mesh_order = alpha_mesh_order;
            e->alpha_mesh_capacity = alpha_mesh_capacity;

            /*e->alpha_mesh_size = alpha_pass->vbo_size;

            LAB_BuildModelOrder(e->mesh_order, e->alpha_mesh_size);*/
        }

        if(e->alpha_mesh_size != alpha_pass->vbo_size)
        {
            if(e->alpha_mesh_size < alpha_pass->vbo_size) // add additional indices
            {
                size_t index = 3*e->alpha_mesh_size;
                for(size_t i = e->alpha_mesh_size; i < alpha_pass->vbo_size; ++i)
                {
                    e->alpha_mesh_order[i].v[0] = index++;
                    e->alpha_mesh_order[i].v[1] = index++;
                    e->alpha_mesh_order[i].v[2] = index++;
                }
                LAB_ASSERT(index == 3*alpha_pass->vbo_size);
            }
            else // remove indices greater than the new size
            {
                // next empty slot
                size_t j = 0;
                size_t max = 3*alpha_pass->vbo_size;
                for(size_t i = 0; i < e->alpha_mesh_size; ++i)
                {
                    if(e->alpha_mesh_order[i].v[0] < max)
                    {
                        if(i!=j)
                        {
                            e->alpha_mesh_order[j] = e->alpha_mesh_order[i];
                        }
                        ++j;
                    }
                    else
                    {
                        // Do nothing
                    }
                }
                LAB_ASSERT(j == alpha_pass->vbo_size);
            }
            e->alpha_mesh_size = alpha_pass->vbo_size;
        }
    }



    // TODO lazy updating
    if(e->alpha_mesh_order)
    {
        LAB_Vec3I chunk_pos = { e->x, e->y, e->z };

        LAB_Vec3F cam = LAB_Vec3F_Sub(LAB_Vec3D2F_Cast(view->pos), LAB_Chunk2Pos3F(chunk_pos));
        LAB_SortModelOrder(e->alpha_mesh_order, alpha_pass->data, e->alpha_mesh_size, cam); // TODO: what if size!=vbo_size
    }
}


LAB_STATIC bool LAB_ViewUpdateChunk(LAB_View* view, LAB_ViewChunkEntry* e)
{
//    if(e->dirty & LAB_CHUNK_UPDATE_BLOCK)
//        LAB_View_UpdateChunkSeethrough(view, e);

    
    bool updated = false;

    if(LAB_Access_TryLock(&e->is_accessed))
    {
        if(e->world_chunk && LAB_View_ChunkNeedsUpdate(view, e))
        {
            //updated = LAB_ViewBuildChunk(view, e);

            if(LAB_Chunk_IsEmpty(e->world_chunk))
            {
                e->seethrough_faces = 63;
                e->dirty = 0;
            }
            else
            {
                updated = LAB_GameServer_PushChunkTask(view->server, e->world_chunk, LAB_CHUNK_STAGE_VIEW_MESH);
            }
        }
        else
            LAB_View_UpdateModelOrder(view, e);
        
        LAB_Access_Unlock(&e->is_accessed);
    }
    return updated;
}


LAB_STATIC int LAB_ViewUpdateChunks(LAB_View* view)
{
    //return 0;
    //uint64_t stoptime = LAB_NanoSeconds() + 3500*1000; // 3.5 ms
    uint64_t stoptime = LAB_NanoSeconds() + 1000*1000; // 1 ms
    //uint64_t stoptime = LAB_NanoSeconds() +  500*1000; // 3.5 ms
    unsigned rest_update = view->cfg.max_update;

    /*int px = LAB_Sar(LAB_FastFloorF2I(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view->z), LAB_CHUNK_SHIFT);


    int dist_sq = view->cfg.preload_dist*view->cfg.preload_dist + 3;*/
    // LAB_ViewChunkEntry* e;
    // for(size_t i = 0; i < view->chunks.size; ++i)
    // {
    //     e = view->sorted_chunks[i].entry;

    //     int cx, cy, cz;
    //     cx = e->x;
    //     cy = e->y;
    //     cz = e->z;
    //     if((cx-px)*(cx-px) + (cy-py)*(cy-py) + (cz-pz)*(cz-pz) <= dist_sq
    //        && (e->visible/* || !(rand() & 0x3f)*/))
    //         rest_update -= LAB_ViewUpdateChunk(view, e);

    //     if(stoptime < LAB_NanoSeconds()) break;
    //     if(!rest_update) break;

    // }//);

    LAB_ViewChunkEntry* e;
    size_t i = view->update_pointer;
    if(view->chunk_array.entries_sorted_count)
    while(1)
    {
        ++i;
        if(i >= view->chunk_array.entries_sorted_count)
        {
            i = 0;
            view->server->stats.view_update_cycles++;
        }

        e = view->chunk_array.entries_sorted[i];
        rest_update -= LAB_ViewUpdateChunk(view, e);

        if(stoptime < LAB_NanoSeconds()) break;
        if(!rest_update) break;
        if(i == view->update_pointer) break;

    }
    //if(view->cfg.max_update != rest_update) i = 0;
    view->update_pointer = i;
    return view->cfg.max_update - rest_update;
}
// TODO use glMultiDrawElements
LAB_STATIC bool LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_RenderPass pass)
{
    LAB_View_Mesh* mesh = &chunk_entry->render_passes[pass];
    if(mesh->vbo_size == 0)
        return 0;

    if(!chunk_entry->world_chunk)
        return 0;

    //if(view->flags & LAB_VIEW_NO_RENDER) return 0;
    if(!LAB_View_IsChunkInSight(view, chunk_entry->x, chunk_entry->y, chunk_entry->z))
        return 0;

    /*if(chunk_entry->render_delay)
    {
        chunk_entry->render_delay--;
        return 0;
    }*/

    if(!mesh->vbo) return 0;
    LAB_GL_CHECK();
    LAB_ViewRenderer_Blocks_SetCurrentBuffer(&view->renderer, (LAB_GL_Buffer){mesh->vbo});
    LAB_GL_CHECK();

    LAB_Vec3I chunk_pos = { chunk_entry->x, chunk_entry->y, chunk_entry->z };
    LAB_Vec3F translate = LAB_Vec3F_Sub(LAB_Chunk2Pos3F(chunk_pos), LAB_Vec3D2F_Cast(view->pos));

    LAB_ViewRenderer_Blocks_SetCam(&view->renderer, pass,
        LAB_Vec3F_Neg(translate),
        LAB_REDUCE_3(LAB_Mat4F_Chain,
            view->modlproj_mat,
            LAB_Mat4F_Translate3V(translate),
            LAB_Mat4F_Scale1(1.00001) // Reduces gaps/lines between chunks
        )
    );

    if(pass == LAB_RENDER_PASS_ALPHA && chunk_entry->alpha_mesh_order)
    {
        LAB_ASSERT(chunk_entry->alpha_mesh_size == mesh->vbo_size);
        glDrawElements(GL_TRIANGLES, 3*mesh->vbo_size, GL_UNSIGNED_INT, chunk_entry->alpha_mesh_order);
    }
    else
    {
        LAB_GL_CHECK();

        glDrawArrays(GL_TRIANGLES, 0, 3*mesh->vbo_size);

        LAB_GL_CHECK();
    }

    return 1;
}



LAB_STATIC
LAB_ColorHDR LAB_View_SkyColor(LAB_View* view)
{
    float f = LAB_MIN(LAB_MAX((view->pos.y+64+32)*(1./64.), 0), 1);
    f*=f;

    return LAB_ColorHDR_RGB_F(0.4*f, 0.7*f, 1.0*f);
}

LAB_STATIC
LAB_FogAttrs LAB_View_FogAttrs(LAB_View* view)
{
    float d = LAB_MAX(view->cfg.render_dist*16, 48);
    return (LAB_FogAttrs) {
        .fog_start = (d-16)*0.5,//(d-16)*0.7,
        .fog_end = d-16,
        .fog_color = LAB_View_SkyColor(view),
    };
}


int LAB_ViewRenderChunks(LAB_View* view, LAB_RenderPass pass)
{
    //if(view->flags & LAB_VIEW_NO_RENDER) return;

    int chunks_rendered = 0;

    LAB_GL_CHECK();

    int backwards = LAB_PrepareRenderPass(pass);

    LAB_GL_CHECK();

    LAB_ShadingAttrs shading = { .exposure=view->cfg.exposure, .saturation=view->cfg.saturation };
    LAB_FogAttrs fog = LAB_View_FogAttrs(view);

    LAB_ViewRenderer_Blocks_Prepare(&view->renderer, pass, view->atlas, shading, fog);

    LAB_GL_CHECK();

    LAB_Vec3I pc = LAB_Pos3D2Chunk(view->pos);

    int dist_sq = view->cfg.render_dist*view->cfg.render_dist + 3;
    LAB_ViewChunkEntry* e;
    // int start = backwards ? (int)view->chunks.size-1 : 0;
    // int stop  = backwards ? -1 : (int)view->chunks.size;
    // int step  = backwards ? -1 : 1;
    // for(int i = start; i != stop; i+=step)
    // {
    //     e = view->sorted_chunks[i].entry;
    //     if((e->x-px)*(e->x-px) + (e->y-py)*(e->y-py) + (e->z-pz)*(e->z-pz) <= dist_sq
    //        && e->sight_visible) // && LAB_View_IsChunkCompletelyInFrustum(view, e->x, e->y, e->z))
    //     {
    //         chunks_rendered += (int)LAB_ViewRenderChunk(view, e, pass);
    //     }
    // }
    LAB_ViewArray_EACH_NONEMPTY_SORTED(&view->chunk_array, LAB_LOOP_BACKWARD_IF(backwards), e,
    {
        LAB_Vec3I chunk_pos = LAB_VEC3_FROM(LAB_Vec3I, e);

        if(LAB_Vec3I_DistanceSq(chunk_pos, pc) <= dist_sq
           && e->sight_visible) // && LAB_View_IsChunkCompletelyInFrustum(view, e->x, e->y, e->z))
        {
            chunks_rendered += (int)LAB_ViewRenderChunk(view, e, pass);
        }
    });

    LAB_ViewRenderer_Blocks_Finish(&view->renderer, pass);

#if 0
    if(pass == 0)
        printf("chunks_rendered:");
    printf(" %3i", chunks_rendered);
    if(pass == LAB_RENDER_PASS_COUNT-1)
        printf("         \r");
#endif

    return chunks_rendered;
}




LAB_STATIC void LAB_ViewRenderChunkGrids(LAB_View* view)
{
    LAB_BoxRenderer_Prepare(&view->box_renderer, &view->renderer, view->modlproj_mat, view->pos);

    LAB_Vec3I pc = LAB_Pos3D2Chunk(view->pos);

    int dist_sq = view->cfg.render_dist*view->cfg.render_dist + 3;
    LAB_ViewChunkEntry* e;

    LAB_ViewArray_EACH_SORTED(&view->chunk_array, LAB_LOOP_FORWARD, e,
    {
        LAB_Vec3I chunk_pos = LAB_VEC3_FROM(LAB_Vec3I, e);

        if(LAB_Vec3I_DistanceSq(chunk_pos, pc) <= dist_sq
           && e->sight_visible) // && LAB_View_IsChunkCompletelyInFrustum(view, e->x, e->y, e->z))
        {
            if(!e->world_chunk) continue;
            LAB_Chunk* c = e->world_chunk;

            LAB_Color color = LAB_RGB(255, 255, 255);

            int condition = 6; //2;
            switch(condition)
            {
                case 0:
                {
                    /*if(   e->neighbors[0] && e->neighbors[1]
                        && e->neighbors[2] && e->neighbors[3]
                        && e->neighbors[4] && e->neighbors[5]) continue;*/
                } break;

                case 1:
                {
                    bool has_mesh = false;
                    for(int pass = 0; pass < LAB_RENDER_PASS_COUNT; ++pass)
                    {
                        if(e->render_passes[pass].vbo_size)
                        {
                            has_mesh = true;
                            break;
                        }
                    }
                    if(!has_mesh) continue;
                } break;

                case 2:
                {
                    if(!e->dirty) continue;
                } break;

                case 3:
                {
                    if(c) continue;
                } break;

                case 4:
                {
                    if(c) continue;
                } break;

                case 5:
                {
                    // TODO: Note: unthreaded access, it could be changed non-atomically
                    if(c && !c->queue_prev && !c->access_mode) continue;

                    switch(c ? c->access_mode : 0)
                    {
                        case -1: color = LAB_RGB(255,   0,   0); break;
                        case  0: color = LAB_RGB(255, 255, 255); break;
                        default: color = LAB_RGB(  0, 255,   0); break;
                    }

                } break;

                case 6:
                {
                    // TODO: Note: unthreaded access, it could be changed non-atomically
                    if(c && !c->queue_prev) continue;

                    switch(c ? c->update_stage : ~0u)
                    {
                        case LAB_CHUNK_STAGE_GENERATE:  color = LAB_RGB(255,   0,   0); break;
                        case LAB_CHUNK_STAGE_LIGHT:     color = LAB_RGB(  0, 255,   0); break;
                        case LAB_CHUNK_STAGE_VIEW_MESH: color = LAB_RGB(  0,   0, 255); break;

                        default: color = LAB_RGB(255, 255, 255); break;
                    }

                } break;
            }

            float dx = LAB_CHUNK_SIZE*e->x;//-view->x;
            float dy = LAB_CHUNK_SIZE*e->y;//-view->y;
            float dz = LAB_CHUNK_SIZE*e->z;//-view->z;

            LAB_GL_UniformColor(view->renderer.lines.uni_color, color);
            LAB_RenderBox(&view->box_renderer, LAB_Box3F_FromOriginAndSize((LAB_Vec3F){dx, dy, dz}, (LAB_Vec3F){16, 16, 16}));
            //LAB_RenderBox(view, dx+0.5, dy+0.5, dz+0.5, 16-1.0, 16-1.0, 16-1.0);
            //LAB_RenderBox(view, dx+71.5, dy+7.5, dz+7.5, 1.0, 1.0, 1.0);
        }
    });
}


LAB_STATIC void LAB_View_UploadChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    LAB_ASSERT(chunk_entry->is_accessed);

    if(!chunk_entry->upload_vbo)
        return;

    if(!chunk_entry->world_chunk)
        return;


    if(!LAB_View_IsChunkInSight(view, chunk_entry->x, chunk_entry->y, chunk_entry->z))
        return;


    for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
    {
        LAB_View_Mesh* mesh = &chunk_entry->render_passes[i];

        if(mesh->m_size == 0)
        {
            if(mesh->vbo)
            {
                LAB_GL_CHECK();
                LAB_GL_FREE(glDeleteBuffers, 1, &mesh->vbo);
                LAB_GL_CHECK();
                mesh->vbo = 0;
                mesh->vbo_size = 0;
            }
            continue;
        }

        LAB_ViewUploadVBO(view, mesh);
    }

    LAB_View_UpdateModelOrder(view, chunk_entry);

    //chunk_entry->render_delay = 20;
    chunk_entry->upload_vbo = 0;
}

LAB_STATIC void LAB_View_UploadChunks(LAB_View* view)
{
    LAB_Nanos stoptime = LAB_NanoSeconds() + 1000*1000;

    //GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    LAB_GL_CHECK();


    //GLuint elapsed = 0;
    /*if(view->current_upload_amount)
    {
        glGetQueryObjectuiv(view->upload_time_query, GL_QUERY_RESULT, &elapsed);
        LAB_GL_CHECK();

        view->upload_amount += view->current_upload_amount;
        view->upload_time += elapsed;

        if(view->upload_amount > 2*view->upload_time) // > 2 GB/s = 2 B/ns
        {
            view->upload_amount = 2;
            view->upload_time = 1;
        }

        if(view->upload_time > 1000*1000*1000)
        {
            view->upload_time >>= 1;
            view->upload_amount >>= 1;
        }
    }*/
    view->upload_amount = 2;
    view->upload_time = 1;

    //glBeginQuery(GL_TIME_ELAPSED, view->upload_time_query);
    LAB_GL_CHECK();
    view->current_upload_amount = 0;

    LAB_Vec3I pc = LAB_Pos3D2Chunk(view->pos);

    int dist_sq = view->cfg.render_dist*view->cfg.render_dist + 3;
    LAB_ViewChunkEntry* e;
    //HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    LAB_ViewArray_EACH_NONEMPTY_SORTED(&view->chunk_array, LAB_LOOP_FORWARD, e,
    {
        LAB_Vec3I chunk_pos = LAB_VEC3_FROM(LAB_Vec3I, e);

        if(LAB_Vec3I_DistanceSq(chunk_pos, pc) <= dist_sq
           && e->visible) // NOTE: when also invisible chunks are updated, this should be removed
        {
            if(LAB_Access_TryLock(&e->is_accessed))
            {
                LAB_View_UploadChunk(view, e);
                LAB_Access_Unlock(&e->is_accessed);

                //glFinish(); // Upload it
                //glWaitSync(sync, 0, GL_TIMEOUT_IGNORED);//100*1000);
                //GLenum result = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 100*1000);
                LAB_GL_CHECK();
                //if(result != GL_CONDITION_SATISFIED)
                    //break;
            }
        }
        if(stoptime < LAB_NanoSeconds()) break;

        if(view->upload_amount)
            if(view->current_upload_amount*view->upload_time/view->upload_amount > 300*1000)
                break;
    });

    LAB_GL_CHECK();

    //glEndQuery(GL_TIME_ELAPSED);


    //glDeleteSync(sync);
    //glFinish(); // Upload all
}




#if LAB_VIEW_ENABLE_QUERY
#if !LAB_VIEW_QUERY_IMMEDIATELY
LAB_STATIC void LAB_View_FetchQueryChunks(LAB_View* view)
{
    LAB_ViewChunkEntry* e;
    HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    {
        if(e->query_id)
            LAB_View_FetchQueryChunk(view, e);
    });
}

LAB_STATIC void LAB_View_FetchQueryChunk(LAB_View* view, LAB_ViewChunkEntry* entry)
{

    unsigned available LAB_INIT_DBG(= 2), visible;
    LAB_glGetQueryObjectuiv(entry->query_id, GL_QUERY_RESULT_AVAILABLE, &available);
    LAB_ASSUME(available < 2);
    if(available)
    {
        LAB_glGetQueryObjectuiv(entry->query_id, GL_QUERY_RESULT, &visible);
        entry->visible = visible;
        if(visible) entry->sight_visible = 1;
        else if(LAB_View_IsChunkCompletelyInFrustum(view, entry->x, entry->y, entry->z)) entry->sight_visible = 0;

        LAB_GL_FREE(LAB_glDeleteQueries, 1, &entry->query_id);
        entry->query_id = 0;
    }
    else
    {
        // Query not ready
        #if 0
        entry->sight_visible = entry->visible = 1;

        //if((rand() & 7) == 0)
        {
            LAB_GL_FREE(LAB_glDeleteQueries, 1, &entry->query_id);
            entry->query_id = 0;
        }
        #endif
    }
    LAB_GL_CHECK();
}
#endif


LAB_STATIC void LAB_View_OrderQueryChunks(LAB_View* view)
{
    int px = LAB_Sar(LAB_FastFloorF2I(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view->z), LAB_CHUNK_SHIFT);


    int dist_sq = view->render_dist*view->render_dist + 3;
    LAB_ViewChunkEntry* e;
    HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    {
        int cx, cy, cz;
        cx = e->x;
        cy = e->y;
        cz = e->z;
        int c_dist_sq = (cx-px)*(cx-px) + (cy-py)*(cy-py) + (cz-pz)*(cz-pz);


        //if(c_dist_sq <= 1*1+1*1+1*1)
        if(LAB_View_IsLocalChunk(view, cx, cy, cz))
            e->visible = e->sight_visible = 1;

        else if(c_dist_sq <= dist_sq)
        {
            if(!LAB_View_HasChunkEntryVisibleNeighbors(view, e))
            {
                e->do_query = 0;
                e->visible = 0;
            }
            else
            {
                int probability_update = LAB_MAX(255/c_dist_sq, 31);
                //int probability_update = LAB_MAX(255-c_dist_sq, 31);

                if(e->do_query || (rand()&0xff) <= probability_update)
                {
                    LAB_View_OrderQueryChunk(view, e);
                }
            }
        }
    });
}

LAB_STATIC void LAB_View_OrderQueryChunk(LAB_View* view, LAB_ViewChunkEntry* entry)
{
    if(!LAB_View_IsChunkInSight(view, entry->x, entry->y, entry->z)) return;

    #if LAB_VIEW_QUERY_IMMEDIATELY
    static unsigned query_id = 0;
    if(query_id == 0)
    {
        LAB_GL_ALLOC(glGenQueries, 1, &query_id);
    }
    #else
    //LAB_ASSUME(entry->query_id == 0);
    if(entry->query_id != 0) return;
    LAB_GL_ALLOC(glGenQueries, 1, &entry->query_id);
    // TODO what if query couldn't be allocated?
    unsigned query_id = entry->query_id;
    #endif
    if(!query_id)
    {
        entry->sight_visible = entry->visible = 1;
        return;
    }
    LAB_ASSUME(query_id != 0);
    //LAB_ASSUME(glIsQuery(query_id));
    //if(!glIsQuery(query_id)) printf("%i is not a query\n", query_id);


    // TODO fixed sized buffer of queries, glGetQueryObject called
    //      in the next frame, in LAB_View_Tick
    glPushMatrix();
    #if !LAB_DBG_CHUNK_QUERY
    glColorMask(0, 0, 0, 0);
    #else
    static uint32_t stipple[32] = {0};
    if(stipple[0] == 0)
        for(int i = 0; i < 32; i+=2)
        {
            stipple[i] = ~0xaaaaaaaa;
            stipple[i+1] = ~0x55555555;
        }
    glEnable(GL_POLYGON_STIPPLE);
    glPolygonStipple((unsigned char*)stipple);
    #endif
    glDepthMask(0);



    glBeginQuery(GL_ANY_SAMPLES_PASSED, query_id);

        glTranslatef(16*entry->x-view->x, 16*entry->y-view->y, 16*entry->z-view->z);
        glScalef(16, 16, 16);
        static const float box[6*2*3][3] = {
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0},
            {1, 1, 0}, {0, 1, 0}, {1, 0, 0},

            {1, 0, 1}, {0, 0, 1}, {1, 1, 1},
            {0, 1, 1}, {1, 1, 1}, {0, 0, 1},


            {0, 0, 0}, {0, 1, 0}, {0, 0, 1},
            {0, 1, 1}, {0, 0, 1}, {0, 1, 0},

            {1, 1, 0}, {1, 0, 0}, {1, 1, 1},
            {1, 0, 1}, {1, 1, 1}, {1, 0, 0},


            {0, 0, 0}, {0, 0, 1}, {1, 0, 0},
            {1, 0, 1}, {1, 0, 0}, {0, 0, 1},

            {0, 1, 1}, {0, 1, 0}, {1, 1, 1},
            {1, 1, 0}, {1, 1, 1}, {0, 1, 0},
        };

    #if LAB_DBG_CHUNK_QUERY
        if(entry->do_query)
            glColor4f(0, 1, 0, 1);
        else
            glColor4f(0, 0, 1, 1);
    #endif

        glVertexPointer(3, GL_FLOAT, 0, box);
        glDrawArrays(GL_TRIANGLES, 0, 6*2*3);

    glEndQuery(GL_ANY_SAMPLES_PASSED);
    //if(!glIsQuery(query_id)) printf("%i is not a query\n", query_id);
    LAB_ASSUME(glIsQuery(query_id));

    #if LAB_DBG_CHUNK_QUERY
    glDisable(GL_POLYGON_STIPPLE);
    #endif

    glDepthMask(1);
    glColorMask(1, 1, 1, 1);
    glPopMatrix();

    #if LAB_VIEW_QUERY_IMMEDIATELY
    unsigned visible = 1;
    LAB_glGetQueryObjectuiv(query_id, GL_QUERY_RESULT, &visible);
    entry->visible = visible;
    if(visible) entry->sight_visible = 1;
    else if(LAB_View_IsChunkCompletelyInFrustum(view, entry->x, entry->y, entry->z)) entry->sight_visible = 0;
    #endif

    entry->do_query = 0;
    LAB_GL_CHECK();
}
#endif




















LAB_STATIC void LAB_View_RenderBlockSelection(LAB_View* view)
{
    LAB_Vec3F vpos = LAB_Vec3D2F_Cast(view->pos); // view-pos
    LAB_Vec3F dir = LAB_View_GetDirection(view); // view-dir

    LAB_TraceBlock_Result trace;
    if((trace = LAB_TraceBlock(view->world, 10, vpos, dir, LAB_BLOCK_INTERACTABLE)).has_hit)
    {
        if(!LAB_Vec3I_Equals(trace.hit_block, trace.prev_block))
        {
            LAB_BoxRenderer_Prepare(&view->box_renderer, &view->renderer,
                LAB_REDUCE_3(LAB_Mat4F_Chain,
                    view->projection_mat,
                    LAB_Mat4F_Scale1(0.99),
                    view->modelview_mat
                ),
                view->pos
            );

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            LAB_Block* b;
            LAB_Vec3F pos, size;

            b = LAB_GetBlockP_FromMainThread(view->world, trace.hit_block.x, trace.hit_block.y, trace.hit_block.z);
            LAB_GL_UniformColor(view->renderer.lines.uni_color, LAB_RGBA(0, 0, 0, 180));
            pos = LAB_Vec3F_Add(LAB_Vec3I2F(trace.hit_block), LAB_Vec3F_FromArray(b->bounds[0]));
            size = LAB_Vec3F_Sub(LAB_Vec3F_FromArray(b->bounds[1]), LAB_Vec3F_FromArray(b->bounds[0]));
            LAB_RenderBox(&view->box_renderer, LAB_Box3F_FromOriginAndSize(pos, size));

            /*LAB_GL_UniformColor(view->renderer.lines.uni_color, LAB_RGBA(0, 0, 0, 64));
            pos = LAB_Vec3I2F(trace.prev_block);
            size = (LAB_Vec3F) {1, 1, 1};
            LAB_RenderBox(&view->box_renderer, LAB_Box3F_FromOriginAndSize(pos, size));*/
        }
    }
}

void LAB_ViewRenderHud(LAB_View* view)
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
    LAB_SurfaceRenderer_Prepare(&view->surface_renderer, &view->renderer);
    LAB_Vec2I screen_size = {view->w, view->h};


    // Crosshair
    {
        glBindTexture(GL_TEXTURE_2D, view->crosshair.tex.id);
        LAB_Vec2I tex_size = view->crosshair.size;
        LAB_Vec2I pos = {(screen_size.x - tex_size.x) / 2, (screen_size.y - tex_size.y) / 2};
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
        LAB_RenderSurface_At(&view->surface_renderer, view->crosshair.tex, screen_size, pos, tex_size, 1, LAB_COLOR_WHITE);
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        LAB_Color c = LAB_RGB(77, 77, 77);
        LAB_RenderSurface_At(&view->surface_renderer, view->crosshair.tex, screen_size, LAB_Vec2I_Add(pos, (LAB_Vec2I){1,-1}), tex_size, 1, c);
    }

    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
    {
        TTF_Font* font = view->gui_mgr.mono_font;
        //TTF_Font* font = LAB_GuiMonospaceFont();
        //if(!font) return;

        int rerender = 0;
        int px, py, pz;
        LAB_Vec3I_Unpack(&px, &py, &pz, LAB_Pos3D2Block(view->pos));
        if(view->info.surf == NULL)
        {
            //LAB_SDL_ALLOC(SDL_CreateRGBSurface, &view->info.surf, 0, INFO_WIDTH, INFO_HEIGHT, 32, 0, 0, 0, 0);
            //if(!view->info.surf) return;
            rerender = 1;
        }
        else if(px!=view->info.x || py!=view->info.y || pz!=view->info.z)
        {
            rerender = 1;
            view->info.x = px;
            view->info.y = py;
            view->info.z = pz;
        }


        LAB_GL_ActivateTexture(&view->info.tex);
        if(rerender)
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "%i %i %i", px, py, pz);

            SDL_Color fg = { 255, 255, 255, 255 };
            SDL_Color bg = {   0,   0,   0, 255 };

            LAB_SDL_FREE(SDL_FreeSurface, &view->info.surf);
            LAB_SDL_ALLOC(TTF_RenderUTF8_Shaded, &view->info.surf, font, buf, fg, bg);
            if(!view->info.surf) return;

            LAB_GL_UploadSurf(view->info.tex, view->info.surf);
        }

        float scale = 1;
        LAB_Vec2I pos = {0, view->h-scale*view->info.surf->h};
        LAB_Vec2I tex_size = {view->info.surf->w, view->info.surf->h};
        LAB_RenderSurface_At(&view->surface_renderer, view->info.tex, screen_size, pos, tex_size, scale, LAB_COLOR_WHITE);

        if(view->cfg.flags & LAB_VIEW_SHOW_FPS_GRAPH && view->stats_display.surf)
        {
            LAB_GL_ActivateTexture(&view->stats_display.tex);
            if(view->stats_display.reupload)
            {
                LAB_GL_UploadSurf(view->stats_display.tex, view->stats_display.surf);
                view->stats_display.reupload = false;
            }

            LAB_Vec2I pos2 = {0, view->h-view->info.surf->h-view->stats_display.surf->h};
            LAB_Vec2I tex_size2 = {view->stats_display.surf->w, view->stats_display.surf->h};
            LAB_RenderSurface_At(&view->surface_renderer, view->stats_display.tex, screen_size, pos2, tex_size2, scale, LAB_COLOR_WHITE);
        }
    }
}

void LAB_ViewRenderInit(LAB_View* view)
{
    glShadeModel(GL_SMOOTH);
}

void LAB_ViewRenderProc(void* user, LAB_Window* window)
{
    LAB_View* view = (LAB_View*)user;

    int w, h;
    SDL_GetWindowSize(window->window, &w, &h);
    view->w = w; view->h = h;


    uint64_t t0 = LAB_NanoSeconds();
    LAB_ViewRender(view);
    uint64_t t1 = LAB_NanoSeconds();
    uint64_t d_01;
    d_01 = t1-t0;
    LAB_FpsGraph_SetSample(&view->perf_info->fps_graphs[LAB_TG_VIEW_RENDER],  (float)d_01/1000000.f);
}

void LAB_ViewRender(LAB_View* view)
{
    LAB_GL_CHECK();

    // NO access to world here -> world can update whilst rendering

    // Block rendering settings
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    LAB_GL_CHECK();

    // Sky color
    LAB_ColorHDR sky_color = LAB_View_SkyColor(view);

#ifdef TODO
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, sky_color);
    #if 1
    glFogi(GL_FOG_MODE, GL_LINEAR);
    float d = LAB_MAX(view->cfg.render_dist*16, 48);
    glFogf(GL_FOG_START, d-32);
    //glFogf(GL_FOG_START, (d-16)*0.8);
    glFogf(GL_FOG_START, (d-16)*0.7);
    glFogf(GL_FOG_END, d-16);
    #elif 0
    glFogf(GL_FOG_DENSITY, 0.05);
    #else
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.1/view->render_dist);
    #endif
    if(1) // TODO: check if GL_NV_fog_distance is available
    {
        //glEnable(GL_EYE_RADIAL_NV);
        glFogi(GL_FOG_DISTANCE_MODE_NV, GL_EYE_RADIAL_NV);
    }
#endif

    LAB_GL_CHECK();
    glUseProgram(0); // avoid recompilation
    LAB_GL_CHECK();
    glClearColor(LAB_HDR_RED_VAL(sky_color), LAB_HDR_GRN_VAL(sky_color), LAB_HDR_BLU_VAL(sky_color), 1);
    LAB_GL_CHECK();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LAB_GL_CHECK();

    // Setup projection matrix

    float ratio = view->h?(float)view->w/(float)view->h:1;
    float nearp = 0.075f;
    float fov = view->fov_factor;
    float far = view->cfg.render_dist*16+32;
    view->projection_mat = LAB_Mat4F_Frustum(-fov*nearp*ratio, fov*nearp*ratio, -fov*nearp, fov*nearp, nearp, far);


    // Setup world matrix
    view->modelview_mat = LAB_REDUCE_3(LAB_Mat4F_Chain,
        LAB_Mat4F_Rotate3Deg(view->angle.z, 0, 0, 1),
        LAB_Mat4F_Rotate3Deg(view->angle.x, 1, 0, 0),
        LAB_Mat4F_Rotate3Deg(view->angle.y, 0, 1, 0)
    );

    view->modlproj_mat = LAB_Mat4F_Chain(view->projection_mat, view->modelview_mat);


    LAB_GL_CHECK();

    if(view->cfg.flags&LAB_VIEW_SHOW_CHUNK_GRID)
    {
        LAB_GL_CHECK();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        LAB_GL_CHECK();
        LAB_ViewRenderChunkGrids(view);
    }

    LAB_GL_CHECK();

    // Block rendering settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    LAB_GL_CHECK();

    glBindTexture(GL_TEXTURE_2D, view->atlas->gl_id);


    //LAB_Nanos upload_start = LAB_NanoSeconds();
    LAB_GL_CHECK();
    LAB_View_UploadChunks(view);
    LAB_GL_CHECK();
    //LAB_PerfInfo_FinishNS(view->perf_info, LAB_TG_VIEW_RENDER_UPLOAD, upload_start);

    int render_count = 0;
    LAB_PerfInfo_Push(view->perf_info, LAB_TG_VIEW_RENDER_CHUNKS);

    render_count += LAB_ViewRenderChunks(view, LAB_RENDER_PASS_SOLID);
    render_count += LAB_ViewRenderChunks(view, LAB_RENDER_PASS_MASKED);
    render_count += LAB_ViewRenderChunks(view, LAB_RENDER_PASS_BLIT);

//    LAB_Nanos query_start = LAB_NanoSeconds();
    #if LAB_VIEW_ENABLE_QUERY
    {
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glScalef(0.999, 0.999, 0.999);
        glMatrixMode(GL_MODELVIEW);
        glColor4f(1, 1, 1, 1);

        LAB_View_OrderQueryChunks(view);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_TEXTURE_2D);
    }
    #endif
//    LAB_PerfInfo_FinishNS(view->perf_info, LAB_TG_VIEW_RENDER_QUERY, query_start);


    render_count += LAB_ViewRenderChunks(view, LAB_RENDER_PASS_ALPHA);

    LAB_PerfInfo_Pop(view->perf_info);
    LAB_FpsGraph_SetSample(&view->perf_info->fps_graphs[LAB_TG_VIEW_RENDER_COUNT], 32+render_count/32.f);

    if(view->cfg.flags & LAB_VIEW_SHOW_HUD)
        LAB_View_RenderBlockSelection(view);


    // Render crosshair, coordinates and debug info
    if(view->cfg.flags & LAB_VIEW_SHOW_HUD)
        LAB_ViewRenderHud(view);

    // Gui rendering settings
    LAB_SurfaceRenderer_Prepare(&view->surface_renderer, &view->renderer);
    glEnable(GL_BLEND);
    LAB_GuiManager_Render(&view->gui_mgr, &view->surface_renderer, view->w, view->h);

    if(view->cfg.flags & LAB_VIEW_SHOW_FPS_GRAPH)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glLineWidth(2);
        glEnable(GL_LINE_SMOOTH);
        LAB_PerfInfo_Render(view->perf_info, &view->renderer);
    }

    LAB_GL_CHECK();
}


void LAB_ViewRemoveDistantChunks(LAB_View* view)
{
    LAB_Nanos stoptime = LAB_NanoSeconds() + 300*1000;

    //if(!LAB_WorldServer_LockTimeout(view->server, 100*1000)) return;
    //LAB_WorldServer_Lock(view->server);

    LAB_Vec3I pc = LAB_Pos3D2Chunk(view->pos);

    int dist_sq = view->cfg.keep_dist*view->cfg.keep_dist + 3;

    unsigned rest_unload = view->cfg.max_unload;

    LAB_ViewChunkEntry* e;
    LAB_ViewArray_DeleteList_EACH(&view->chunk_array, e,
    {
        LAB_Chunk* w_chunk = e->world_chunk;
        if(w_chunk) LAB_GameServer_Lock1Chunk(view->server, w_chunk);
        LAB_ViewUnlinkChunk(view, e);
        view->stats.deleted++;
        --rest_unload;
        if(w_chunk) LAB_GameServer_Unlock1Chunk(view->server, w_chunk);
        if(stoptime < LAB_NanoSeconds()) return;
        //if(!rest_unload) return;
    });
    //return;

    //LAB_ViewArray_EACH_SORTED(&view->chunk_array, LAB_LOOP_BACKWARD, e,
    size_t v = LAB_ViewArray_Volume(&view->chunk_array);
    LAB_ASSERT(v);
    size_t s = view->delete_index % v;
    size_t i = s;
    do
    {
        LAB_ASSERT(i < v);
        e = view->chunk_array.entries[i];
        if(e && LAB_Vec3I_DistanceSq(LAB_VEC3_FROM(LAB_Vec3I, e), pc) > dist_sq)
        {
            LAB_Chunk* w_chunk = e->world_chunk;
            if(w_chunk) LAB_GameServer_Lock1Chunk(view->server, w_chunk);
            LAB_ViewUnlinkChunk(view, e);
            view->chunk_array.entries[i] = NULL;
            view->chunk_array.entries_count--;
            view->stats.deleted++;
            --rest_unload;
            if(w_chunk) LAB_GameServer_Unlock1Chunk(view->server, w_chunk);
        }
        if(stoptime < LAB_NanoSeconds()) break;
        //if(!rest_unload) break;

        i++;
        if(i == v) i = 0;
    }
    while(i != s);
    view->delete_index = i;


    //LAB_WorldServer_Unlock(view->server);
}





LAB_ViewChunkEntry* LAB_ViewFindChunkEntry(LAB_View* view, int x, int y, int z)
{
    LAB_ChunkPos pos = {x, y, z};
    //LAB_ViewChunkEntry** entry = LAB_View_ChunkTBL_Get(&view->chunks, pos);
    //return entry?*entry:NULL;

    return LAB_ViewArray_Get(&view->chunk_array, pos);
}

/**
 *  Assume: x,y,z is not contained in view->chunks
 */
LAB_ViewChunkEntry* LAB_ViewNewChunkEntry(LAB_View* view, int x, int y, int z)
{
    LAB_ChunkPos pos = {x, y, z};
    //LAB_ViewChunkEntry** entry;
    //entry = LAB_View_ChunkTBL_PutAlloc(&view->chunks, pos);
    //if(LAB_UNLIKELY(!entry)) return NULL;
    LAB_ViewChunkEntry* e;
    // e = *entry;
    LAB_ASSERT(!LAB_ViewArray_Get(&view->chunk_array, pos));
    e = LAB_Calloc(1, sizeof*e);
    if(!e) return NULL;

    // memset 0 -> calloc
    e->x = x;
    e->y = y;
    e->z = z;
    /*for(int face = 0; face < 6; ++face)
    {
        LAB_ViewChunkEntry* neighbor = LAB_ViewFindChunkEntry(view, x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face));
        if(neighbor)
        {
            (*entry)->neighbors[face]   = neighbor;
            neighbor->neighbors[face^1] = *entry;
        }
    }*/
    #if !LAB_VIEW_ENABLE_QUERY
    e->sight_visible = e->visible = 1;
    #endif
    LAB_ViewArray_Set(&view->chunk_array, e);
    LAB_ASSERT(LAB_ViewArray_Get(&view->chunk_array, pos) == e);
    return e;
}


/*LAB_ViewChunkEntry* LAB_ViewGetChunkEntry(LAB_View* view, int x, int y, int z)
{
    LAB_ViewChunkEntry* chunk_entry;
    chunk_entry = LAB_ViewFindChunkEntry(view, x, y, z);
    if(chunk_entry != NULL) return chunk_entry;
    return LAB_ViewNewChunkEntry(view, x, y, z);

}*/

void LAB_ViewInvalidateEverything(LAB_View* view, int free_buffers)
{
    LAB_ViewChunkEntry* e;

    LAB_ViewArray_EACH(&view->chunk_array, e,
    {
        e->dirty = ~0;
    });

    if(free_buffers)
    {
        LAB_ViewArray_EACH(&view->chunk_array, e,
        {
            for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
            {
                LAB_View_Mesh* m = &e->render_passes[i];
                if(m->vbo)
                {
                    LAB_GL_FREE(glDeleteBuffers, 1, &m->vbo);
                    m->vbo = 0;
                    m->vbo_size = 0;
                }
            }
        });
    }
}



LAB_Vec3F LAB_View_GetDirection(LAB_View* view)
{
    LAB_Vec3F dir;

    float ax, ay;
    float sax, cax, say, cay;

    ax = view->angle.x*LAB_PI/180.f;
    ay = view->angle.y*LAB_PI/180.f;

    // ax = 0; //DBG

    sax = sin(ax);
    cax = cos(ax);
    say = sin(ay);
    cay = cos(ay);

    #if 1
    dir.x = cax*say;
    dir.y = -sax;
    dir.z = -cax*cay;
    #else
    float dx, dy, dz;
    dx = cax*say;
    dy = -sax;
    dz = -cax*cay;

    float az;
    float saz, caz;

    az = view->az*LAB_PI/180.f;

    saz = sin(az);
    caz = cos(az);

    dir.x =  dx*caz + dy*saz;
    dir.y = -dx*saz + dy*caz;
    dir.z =  dz;
    #endif

    return dir;
}


void LAB_ViewTick(LAB_View* view, uint32_t delta_ms)
{
    LAB_Vec3I pc = LAB_Pos3D2Chunk(view->pos);
    LAB_ChunkPos origin_chunk = LAB_ChunkVec2ChunkPos(pc);
    LAB_ViewArray_SetOrigin(&view->chunk_array, origin_chunk);
    LAB_ViewArray_Resize(&view->chunk_array, view->cfg.keep_dist);
    LAB_ViewArray_Collect(&view->chunk_array);


    LAB_PerfInfo_Push(view->perf_info, LAB_TG_VIEW_LOAD);
    LAB_ViewLoadNearChunks(view); // uses sorted_chunks
    LAB_PerfInfo_Pop(view->perf_info);

//    LAB_PerfInfo_Push(view->perf_info, LAB_TG_VIEW_UPDATE);
    //view->perf_info->current_time = LAB_NanoSeconds();
    LAB_ViewRemoveDistantChunks(view); // sorted_chunks has invalid content
    #if !LAB_VIEW_QUERY_IMMEDIATELY && LAB_VIEW_ENABLE_QUERY
    LAB_View_FetchQueryChunks(view);
    #endif
    
    //LAB_View_SortChunks(view, delta_ms); // sorted_chunks has valid content again

    LAB_PerfInfo_Push(view->perf_info, LAB_TG_VIEW_UPDATE);
    int update_count = LAB_ViewUpdateChunks(view);
    LAB_PerfInfo_Pop(view->perf_info);
    (void)update_count;
//    LAB_FpsGraph_SetSample(&view->perf_info->fps_graphs[LAB_TG_MESH], 32+update_count*2);


    
    //LAB_FpsGraph_AddSample(&view->fps_graph, delta_ms);
    LAB_GuiManager_Tick(&view->gui_mgr);
    //LAB_View_SortChunks(view, delta_ms); // sorted_chunks has valid content again
}


/*LAB_STATIC bool LAB_View_HasChunkVisibleNeighbors(LAB_View* view, int x, int y, int z)
{
    int faces = LAB_View_ChunkNeighborVisibility(view, x, y, z);

    int i;
    LAB_DIR_EACH(faces, i,
    {
        //const int* off = LAB_offset[i];
        int xx = x + LAB_OX(i);
        int yy = y + LAB_OY(i);
        int zz = z + LAB_OZ(i);
        LAB_ViewChunkEntry* neighbor = LAB_ViewFindChunkEntry(view, xx, yy, zz);
        if(neighbor && neighbor->visible) return 1;
    });
    return 0;
}*/

/*
LAB_STATIC bool LAB_View_HasChunkEntryVisibleNeighbors(LAB_View* view, LAB_ViewChunkEntry* entry)
{
    int faces = LAB_View_ChunkNeighborVisibility(view, entry->x, entry->y, entry->z);

    int i;
    LAB_DIR_EACH(faces, i,
    {
        LAB_ViewChunkEntry* neighbor = entry->neighbors[i];
        if(neighbor && neighbor->visible) return 1;
    });
    return 0;
}
*/

#if 0
LAB_STATIC bool LAB_View_IsLocalChunk(LAB_View* view, int cx, int cy, int cz)
{
    int   vxi, vyi, vzi;
    int   vcx, vcy, vcz;
    float vxc, vyc, vzc;

    vxi = LAB_FastFloorF2I(view->x);
    vyi = LAB_FastFloorF2I(view->y);
    vzi = LAB_FastFloorF2I(view->z);

    vcx = LAB_Sar(vxi, LAB_CHUNK_SHIFT);
    vcy = LAB_Sar(vyi, LAB_CHUNK_SHIFT);
    vcz = LAB_Sar(vzi, LAB_CHUNK_SHIFT);

    vxc = vxi & LAB_CHUNK_MASK;
    vyc = vyi & LAB_CHUNK_MASK;
    vzc = vzi & LAB_CHUNK_MASK;

    /**/ if(vxc <   1) { if(cx != vcx && cx != vcx-1) return 0; }
    else if(vxc >= 15) { if(cx != vcx && cx != vcx+1) return 0; }
    else               { if(cx != vcx               ) return 0; }

    /**/ if(vyc <   1) { if(cy != vcy && cy != vcy-1) return 0; }
    else if(vyc >= 15) { if(cy != vcy && cy != vcy+1) return 0; }
    else               { if(cy != vcy               ) return 0; }

    /**/ if(vzc <   1) { if(cz != vcz && cz != vcz-1) return 0; }
    else if(vzc >= 15) { if(cz != vcz && cz != vcz+1) return 0; }
    else               { if(cz != vcz               ) return 0; }

    return 1;
}
#endif


bool LAB_View_IsChunkInSight(LAB_View* view, int cx, int cy, int cz)
{
    /*//if(cy < -2 || cy >= 0) return 0; // DBG
    // TODO: might be inaccurate for large coordinates
    float dir[3];
    LAB_Vec3F_ToArray(dir, LAB_View_GetDirection(view));

    float treshold = view->x*dir[0] + view->y*dir[1] + view->z*dir[2];

    dir[0] *= 16.f;
    dir[1] *= 16.f;
    dir[2] *= 16.f;

    // Distance of the nearest corner of the chunk to the plane
    // with the normal vector dir, that has a distance treshold
    // to the origin
    float dist = cx*dir[0] + cy*dir[1] + cz*dir[2];
    if(dir[0] > 0) dist += dir[0];
    if(dir[1] > 0) dist += dir[1];
    if(dir[2] > 0) dist += dir[2];

    return dist >= treshold;*/

    return LAB_View_IsChunkPartlyInFrustum(view, cx, cy, cz);
}



bool LAB_View_IsChunkPartlyInFrustum(LAB_View* view, int cx, int cy, int cz)
{
    // TODO: if the chunk is near the camera, edge points can be located outside
    //       of the screen, while parts of the chunk are visible to the screen
    int bits_outside_inside = 0;

    for(int i = 0; i < 8; ++i)
    {
        LAB_Vec3I chunk_corner = { cx+!!(i&1), cy+!!(i&2), cz+!!(i&4) };
        LAB_Vec3F offset = LAB_Vec3F_Sub(LAB_Chunk2Pos3F(chunk_corner), LAB_Vec3D2F_Cast(view->pos));

        LAB_Vec3F screen_pos = LAB_ProjectPoint(view->modlproj_mat, offset);

        /**/ if(screen_pos.x < -1) bits_outside_inside |=  1;
        else if(screen_pos.x >  1) bits_outside_inside |=  2;
        else                       bits_outside_inside |=  4;

        /**/ if(screen_pos.y < -1) bits_outside_inside |=  8;
        else if(screen_pos.y >  1) bits_outside_inside |= 16;
        else                       bits_outside_inside |= 32;

        if(screen_pos.z > 0) bits_outside_inside |= 64;

        if(!(screen_pos.z <  0 ||
             screen_pos.x < -1 || screen_pos.x > 1 ||
             screen_pos.y < -1 || screen_pos.y > 1)) return 1;
    }

    if(bits_outside_inside
        && ((bits_outside_inside& 3 ) ==  3  || (bits_outside_inside& 4 ))
        && ((bits_outside_inside&030) == 030 || (bits_outside_inside&040))) return 1;

    return 0;
}

bool LAB_View_IsChunkCompletelyInFrustum(LAB_View* view, int cx, int cy, int cz)
{
    for(int i = 0; i < 8; ++i)
    {
        LAB_Vec3I chunk_corner = { cx+!!(i&1), cy+!!(i&2), cz+!!(i&4) };
        LAB_Vec3F offset = LAB_Vec3F_Sub(LAB_Chunk2Pos3F(chunk_corner), LAB_Vec3D2F_Cast(view->pos));

        LAB_Vec3F screen_pos = LAB_ProjectPoint(view->modlproj_mat, offset);

        if(screen_pos.z <  0 ||
           screen_pos.x < -1 || screen_pos.x > 1 ||
           screen_pos.y < -1 || screen_pos.y > 1) return 0;
    }
    return 1;
}


// Difference of following functions: comparison operators: <= >= vs < >
/*LAB_STATIC unsigned LAB_View_ChunkVisibility(LAB_View* view, int cx, int cy, int cz)
{
    int px = LAB_Sar(LAB_FastFloorF2I(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view->z), LAB_CHUNK_SHIFT);

    unsigned faces = 0;
    if(cx<=px) faces  = LAB_DIR_EAST;
    if(cx>=px) faces |= LAB_DIR_WEST;
    if(cy<=py) faces |= LAB_DIR_UP;
    if(cy>=py) faces |= LAB_DIR_DOWN;
    if(cz<=pz) faces |= LAB_DIR_SOUTH;
    if(cz>=pz) faces |= LAB_DIR_NORTH;
    return faces;
}*/

/*LAB_STATIC unsigned LAB_View_ChunkNeighborVisibility(LAB_View* view, int cx, int cy, int cz)
{
    int px = LAB_Sar(LAB_FastFloorF2I(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view->z), LAB_CHUNK_SHIFT);

    unsigned faces = 0;
    if(cx<px) faces  = LAB_DIR_EAST;
    if(cx>px) faces |= LAB_DIR_WEST;
    if(cy<py) faces |= LAB_DIR_UP;
    if(cy>py) faces |= LAB_DIR_DOWN;
    if(cz<pz) faces |= LAB_DIR_SOUTH;
    if(cz>pz) faces |= LAB_DIR_NORTH;
    return faces;
}*/

LAB_STATIC void LAB_View_UpdateChunkSeethrough(LAB_View* view, LAB_ViewChunkEntry* e)
{
    LAB_Chunk* chunk = e->world_chunk;
    if(!chunk) return;

    if(LAB_Chunk_IsEmpty(e->world_chunk))
    {
        e->seethrough_faces = 63;
        return;
    }

    int faces = 0;

    LAB_Chunk_Blocks* blocks = e->world_chunk->buf_blocks;
    LAB_ASSERT(blocks);

    LAB_UNROLL(6)
    for(int face = 0; face < 6; ++face)
    {
        const size_t  c = LAB_FACE_POSITIVE(face) ? 15 << (LAB_FACE_AXIS(face)*4) : 0;
        const size_t da = LAB_AXF(face) | LAB_AYF(face)<<4 | LAB_AZF(face)<<8;
        const size_t db = LAB_BXF(face) | LAB_BYF(face)<<4 | LAB_BZF(face)<<8;

        #define AX_CNT(xyz) (!!((xyz)&0x00f) + !!((xyz)&0x0f0) + !!((xyz)&0xf00))
        LAB_ASSERT(AX_CNT(c)  <= 1);
        LAB_ASSERT(AX_CNT(da) == 1);
        LAB_ASSERT(AX_CNT(db) == 1);
        LAB_ASSERT(!(da&0x00f) || !(db&0x00f));
        LAB_ASSERT(!(da&0x0f0) || !(db&0x0f0));
        LAB_ASSERT(!(da&0xf00) || !(db&0xf00));
        LAB_ASSERT(!(da&0x00f) || !(db&0x00f));
        LAB_ASSERT(!(da&0x0f0) || !(db&0x0f0));
        LAB_ASSERT(!(da&0xf00) || !(db&0xf00));

        for(size_t ib = 0, b = 0; ib < 16; ++ib, b+=db)
        for(size_t ia = 0, a = 0; ia < 16; ++ia, a+=da)
        {
            LAB_ASSERT((c|a|b) < 16*16*16);
            LAB_BlockID bid = blocks->blocks[c|a|b];
            if(!(LAB_BlockP(bid)->flags & LAB_BLOCK_OPAQUE))
            {
                faces |= 1 << face;
                goto finish_face;
            }
        }

        finish_face:;
    }
    e->seethrough_faces = faces;
}

void LAB_ViewLoadNearChunks(LAB_View* view)
{
    // PROBLEM: unbalanced loading
    // TODO: new chunk loading design: add hook to world, that asks the view to load another chunk -> allows to
    //       load multiple chunks, if some chunks were empty (weird approach)
    //    OR keep the view managing the loading, use queries here (ids to the chunk loading requests)
    //       - drive the loading by an array that has all possibly rendered chunk(coordinate)s preallocated and sorted
    //         - special resorting, if current chunk has changed
    //         - if some entry in the array has a query, load the chunk from the world
    //    OR - limit loading by time instead of a simple counter
    //         - empty chunks (above the surface) are faster to load

    // TODO: check if gen-queue is full: quit

    LAB_Vec3I pc = LAB_Pos3D2Chunk(view->pos);

    // to be able to use view->sorded_chunks to check for nearest non-loaded chunks
    // we need to save the size here, otherwise it might be one to big by the loading of
    // the local chunk
    //int sorted_size = view->chunks.size;

    // Always load local chunk
    // not counted to the load limit
    {
        LAB_ViewChunkEntry* entry = LAB_ViewFindChunkEntry(view, pc.x, pc.y, pc.z);

        if(entry == NULL)
        {
            LAB_Chunk* chunk = LAB_GenerateChunk(view->world, pc.x, pc.y, pc.z);
            if(chunk)
            {
                if(chunk->view_user)
                {
                    LAB_ViewChunkEntry* e = chunk->view_user;
                    LAB_ViewArray_Recover(&view->chunk_array, e);
                    view->stats.recovered_count++;
                }
                else
                {
                    entry = LAB_ViewNewChunkEntry(view, pc.x, pc.y, pc.z);
                    if(entry == NULL) return; // NO MEMORY
                    entry->dirty = ~0;
                    LAB_ASSUME(!entry->world_chunk);
                    LAB_ASSUME(!chunk->view_user);

                    //entry->do_query = 1;
                    entry->visible = 1;

                    chunk->view_user = entry;
                    entry->world_chunk = chunk;
                }
            }
        }
    }

    int load_amount = view->cfg.load_amount; // should be configurable

    LAB_Vec3I block_pos = LAB_Pos3D2Block(view->pos);
    LAB_Block* b = LAB_GetBlockP_FromMainThread(view->world, block_pos.x, block_pos.y, block_pos.z);
    bool is_xray = !!(b->flags & LAB_BLOCK_OPAQUE);

    uint64_t stoptime = LAB_NanoSeconds() + 3000*1000; // 1 ms

    LAB_ViewChunkEntry* c;
    //LAB_ViewArray_EACH_SORTED(&view->chunk_array, LAB_LOOP_FORWARD, c,

    static
    size_t si = 0; // TODO
    if(si >= view->chunk_array.entries_sorted_count) si = 0;

    size_t i = si;

    unsigned int xray_faces = is_xray ? 63 : 0;

    // find first chunk with missing neighbors
    // Loop
    /*for(i = si; i < view->chunk_array.entries_sorted_count; ++i)
    {
        break;
        c = view->chunk_array.entries_sorted[i];

        if(LAB_UNLIKELY(!c->world_chunk))
        {
            goto empty_found;
        }

        int faces = LAB_OUTWARDS_FACES(c->x, c->y, c->z, px, py, pz);
        faces &= c->seethrough_faces | xray_faces;
        int face;
        LAB_DIR_EACH(faces, face,
        {
            int xx = c->x+LAB_OX(face), yy = c->y+LAB_OY(face), zz = c->z+LAB_OZ(face);
            LAB_ChunkPos pp = { xx, yy, zz };

            if(LAB_UNLIKELY(!LAB_ViewArray_Get(&view->chunk_array, pp)))
                goto empty_found;
        });
    }
    empty_found:;*/

    int dbg_waiting_count = 0;
    unsigned int dbg_maxdist = 0;
    for(; i < view->chunk_array.entries_sorted_count; ++i)
    {
        c = view->chunk_array.entries_sorted[i];

        /*if(c->visible && (    LAB_View_IsChunkPartlyInFrustum(view, c->x, c->y, c->z)
                           || LAB_View_IsLocalChunk(view, c->x, c->y, c->z)))*/
        {
            if(!c->world_chunk)
            {
                --load_amount;
                c->do_query = 1;
                LAB_Chunk* chunk = LAB_GenerateChunk(view->world, c->x, c->y, c->z);
                if(chunk)
                {
                    chunk->view_user = c;
                    c->world_chunk = chunk;

                    if(chunk->light_generated)
                    {
                        LAB_GameServer_PushChunkTask(view->server, chunk, LAB_CHUNK_STAGE_VIEW_MESH);
                    }
                }
                else
                {
                    dbg_waiting_count++;
                    continue;
                }
            }
            if(!load_amount) break;
            if(stoptime < LAB_NanoSeconds()) break;

            int faces = LAB_OUTWARDS_FACES(c->x, c->y, c->z, pc.x, pc.y, pc.z);
            faces &= c->seethrough_faces | xray_faces;
            int face;
            LAB_DIR_EACH(faces, face,
            {
                int xx = c->x+LAB_OX(face), yy = c->y+LAB_OY(face), zz = c->z+LAB_OZ(face);
                LAB_ChunkPos pp = { xx, yy, zz };

                //if(c->neighbors[face])
                if(LAB_ViewArray_Get(&view->chunk_array, pp))
                    continue;

                if(!LAB_View_IsChunkPartlyInFrustum(view, xx, yy, zz))
                    continue;

                int dx = xx-pc.x;
                int dy = yy-pc.y;
                int dz = zz-pc.z;
                unsigned int dist = dx*dx+dy*dy+dz*dz;
                if(dist > view->cfg.preload_dist*view->cfg.preload_dist + 3)
                    continue;

                if(dist > dbg_maxdist) dbg_maxdist = dist;


                LAB_Chunk* chunk = LAB_GenerateChunk(view->world, xx, yy, zz);
                if(chunk)
                {
                    if(chunk->view_user)
                    {
                        LAB_ViewChunkEntry* e = chunk->view_user;
                        LAB_ViewArray_Recover(&view->chunk_array, e);
                        view->stats.recovered_count++;
                    }
                    else
                    {
                        LAB_ViewChunkEntry* entry = LAB_ViewNewChunkEntry(view, xx, yy, zz);
                        LAB_ASSERT_OR_WARN(entry);
                        if(entry == NULL) break; // NO MEMORY
                        entry->dirty = ~0;
                        --load_amount;
                        LAB_ASSUME(!entry->world_chunk);
                        LAB_ASSUME(!chunk->view_user);

                        //entry->do_query = 1;

                        chunk->view_user = entry;
                        entry->world_chunk = chunk;
                    }
                }
                if(!load_amount) break;
            });
        }
    }
    /*LAB_DbgPrintf("%5i, %3i, %3i, %2i, %2i, %5.1f, %c\n",
                    i,
                    view->cfg.load_amount - load_amount,
                    dbg_waiting_count,
                    view->cfg.preload_dist,
                    view->cfg.keep_dist,
                    sqrt(dbg_maxdist),
                    (stoptime < LAB_NanoSeconds() ? 'X' : '1')
                    );*/
    si = i;

    if(view->cfg.load_amount - load_amount)
        si = 0;


}


void LAB_ViewDestructChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
        LAB_View_Mesh_Destroy(&chunk_entry->render_passes[i]);

    if(chunk_entry->alpha_mesh_order)
        LAB_Free(chunk_entry->alpha_mesh_order);

    #if !LAB_VIEW_QUERY_IMMEDIATELY && LAB_VIEW_ENABLE_QUERY
    if(chunk_entry->query_id)
    {
        LAB_GL_FREE(LAB_glDeleteQueries, 1, &chunk_entry->query_id);
    }
    #endif
}

void LAB_ViewDestructFreeChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    LAB_ViewDestructChunk(view, chunk_entry);
    LAB_Free(chunk_entry);
}

void LAB_ViewUnlinkChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    /*for(int face = 0; face < 6; ++face)
    {
        LAB_ViewChunkEntry* neighbor = chunk_entry->neighbors[face];
        if(neighbor)
        {
            neighbor->neighbors[face^1] = NULL;
        }
    }*/
    if(chunk_entry->world_chunk)
    {
        LAB_ASSERT(chunk_entry->world_chunk->view_user == chunk_entry);
        chunk_entry->world_chunk->view_user = NULL;
    }

    LAB_ViewDestructFreeChunk(view, chunk_entry);
}



void LAB_ViewCoordInfo_Create_Zero(LAB_ViewCoordInfo* info)
{
    //memset(info, 0, sizeof(info));
    // TODO LAB_ASSUME LAB_MemCheck(info, 0, sizeof(info));
}

void LAB_ViewCoordInfo_Destroy(LAB_ViewCoordInfo* info)
{
    LAB_GL_FREE(glDeleteTextures, 1, &info->tex.id);
    LAB_SDL_FREE(SDL_FreeSurface, &info->surf);
}






void LAB_View_ShowGuiMenu(LAB_View* view)
{
    LAB_GuiMenu* gui = LAB_Malloc(sizeof *gui);
    if(!gui) return;
    LAB_GuiMenu_Create(gui, view->world);
    LAB_GuiManager_ShowDialog(&view->gui_mgr, (LAB_GuiComponent*)gui);
}

void LAB_View_ShowGuiInventory(LAB_View* view, LAB_BlockID* block)
{
    LAB_GuiInventory* gui = LAB_Malloc(sizeof *gui);
    if(!gui) return;
    LAB_GuiInventory_Create(gui, &LAB_cheat_inventory, block);
    LAB_GuiManager_ShowDialog(&view->gui_mgr, (LAB_GuiComponent*)gui);
}
