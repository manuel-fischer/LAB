#include "LAB_view.h"

#include "LAB_memory.h"
#include "LAB_error.h"
#include "LAB_math.h"
#include "LAB_debug.h"

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

#include "LAB_gui.h"
#include "LAB_gui_component.h"
#include "LAB_gui_menu.h"

#define HTL_PARAM LAB_VIEW_CHUNK_TBL
#include "HTL_hasharray.t.c"
#undef HTL_PARAM

/**
 *  TODO
 *  - when the only visible blocks of a chunk are at the chunk border, the cube that is rendered for the chunk z-fights with the z data of
 *    those rendered blocks
 */


///############################

LAB_STATIC bool LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world, unsigned visibility);
LAB_STATIC void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_IN LAB_Chunk* chunk_neighborhood[27], unsigned visibility);
LAB_STATIC void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_IN LAB_Chunk* chunk_neighborhood[27], int x, int y, int z, unsigned visibility);
LAB_STATIC void LAB_ViewUpdateChunks(LAB_View* view);
LAB_STATIC bool LAB_ViewUpdateChunk(LAB_View* view, LAB_ViewChunkEntry* e);
LAB_STATIC void LAB_ViewRenderChunks(LAB_View* view, LAB_RenderPass pass);
LAB_STATIC bool LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_RenderPass pass);

LAB_STATIC bool LAB_View_HasChunkEntryVisibleNeighbors(LAB_View* view, LAB_ViewChunkEntry* e);
LAB_STATIC bool LAB_View_HasChunkVisibleNeighbors(LAB_View* view, int x, int y, int z);
LAB_STATIC bool LAB_View_IsLocalChunk(LAB_View* view, int cx, int cy, int cz);
LAB_STATIC unsigned LAB_View_ChunkVisibility(LAB_View* view, int cx, int cy, int cz);         // inclusive for same coordinates
LAB_STATIC unsigned LAB_View_ChunkNeighborVisibility(LAB_View* view, int cx, int cy, int cz); // exclusive for same coordinates
#if !LAB_VIEW_QUERY_IMMEDIATELY
LAB_STATIC void LAB_View_FetchQueryChunks(LAB_View* view);
LAB_STATIC void LAB_View_FetchQueryChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
#endif
LAB_STATIC void LAB_View_OrderQueryChunks(LAB_View* view);
LAB_STATIC void LAB_View_OrderQueryChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
LAB_STATIC void LAB_ViewRemoveDistantChunks(LAB_View* view);
LAB_STATIC void LAB_ViewDestructChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
LAB_STATIC void LAB_ViewDestructFreeChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
/** unlink, destruct and free chunk **/
LAB_STATIC void LAB_ViewUnlinkChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);


LAB_STATIC void LAB_ViewUploadVBO(LAB_View* view, LAB_View_Mesh* mesh);
LAB_STATIC bool LAB_ViewBuildChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
LAB_STATIC void LAB_View_UploadChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
LAB_STATIC void LAB_View_UploadChunks(LAB_View* view);

LAB_STATIC void LAB_RenderBox(LAB_View* view, float x, float y, float z, float w, float h, float d);
LAB_STATIC void LAB_View_RenderBlockSelection(LAB_View* view);

LAB_STATIC int LAB_View_CompareChunksIndirect(const void* a, const void* b);
LAB_STATIC void LAB_View_SortChunks(LAB_View* view, uint32_t delta_ms);

LAB_STATIC LAB_Triangle* LAB_ViewMeshAlloc(LAB_View_Mesh* mesh, size_t add_size, size_t extra_size);

LAB_STATIC void LAB_ViewRenderInit(LAB_View* view);

bool LAB_ConstructView(LAB_View* view, LAB_World* world)
{
    LAB_InitAssets();

    memset(view, 0, sizeof *view);
    view->world = world;

    view->y = 1.5;

    //LAB_View_ChunkTBL_Create(&view->entry); // not nessesary because already set to 0 above

    LAB_FpsGraph_Create(&view->fps_graph,       LAB_RGB(255, 255, 128));
    LAB_FpsGraph_Create(&view->fps_graph_input, LAB_RGB(255, 128, 128));
    LAB_FpsGraph_Create(&view->fps_graph_world, LAB_RGB(128, 255, 128));
    LAB_FpsGraph_Create(&view->fps_graph_view,  LAB_RGB(128, 128, 255));

    LAB_GuiManager_Create(&view->gui_mgr);

    LAB_ViewRenderInit(view);

    return 1;
}

void LAB_DestructView(LAB_View* view)
{
    LAB_Free(view->sorted_chunks);

    LAB_GuiManager_Destroy(&view->gui_mgr);
    LAB_FpsGraph_Destroy(&view->fps_graph_view);
    LAB_FpsGraph_Destroy(&view->fps_graph_world);
    LAB_FpsGraph_Destroy(&view->fps_graph_input);
    LAB_FpsGraph_Destroy(&view->fps_graph);

    LAB_ViewChunkEntry* e;
    HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    {
        // unlinking not nessesary
        LAB_ViewDestructFreeChunk(view, e);
    });

    LAB_View_ChunkTBL_Destroy(&view->chunks);

    LAB_ViewCoordInfo_Destroy(&view->info);
}


void LAB_ViewChunkProc(void* user, LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update)
{
    LAB_View* view = (LAB_View*)user;

    // ignore far away chunks
    // TODO: if neighboring chunk is inside view
    // TODO: make behavior in LAB_ViewChunkKeep proc into another function without a void* argument
    if(!LAB_ViewChunkKeepProc(user, world, x, y, z)) return;

    /*LAB_ViewChunkEntry* entry = LAB_ViewGetChunkEntry(view, x, y, z);
    if(entry)
        entry->dirty = update;*/

    // Handle block update at border correctly
    for(int iz = -1; iz <= 1; ++iz)
    for(int iy = -1; iy <= 1; ++iy)
    for(int ix = -1; ix <= 1; ++ix)
    {
        LAB_ViewChunkEntry* entry = LAB_ViewFindChunkEntry(view, x+ix, y+iy, z+iz);
        if(entry)
            entry->dirty |= update;
    }
}

bool LAB_ViewChunkKeepProc(void* user, LAB_World* world, int x, int y, int z)
{
    LAB_View* view = (LAB_View*)user;

    int px = LAB_Sar(LAB_FastFloorF2I(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view->z), LAB_CHUNK_SHIFT);

    int dx = x-px;
    int dy = y-py;
    int dz = z-pz;
    unsigned int dist = dx*dx+dy*dy+dz*dz;
    return dist <= view->keep_dist*view->keep_dist+3;
}



/**
 *  Return 1 if the chunk was available
 */
LAB_STATIC bool LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world, unsigned visibility)
{
    LAB_Chunk* chunk_neighborhood[27];

    LAB_GetChunkNeighborhood(world, chunk_neighborhood,
                             chunk_entry->x, chunk_entry->y, chunk_entry->z,
                             LAB_CHUNK_EXISTING);

    if(chunk_neighborhood[1+3+9] == NULL) return 0;
    //chunk_entry->exist = chunk_neighborhood[1+3+9] != NULL;
    chunk_entry->exist = 1;
    for(int i = 0; i < 27; ++i)
        if(chunk_neighborhood[i] == NULL && (rand()&0x7) == 0) return 0;

    chunk_entry->visible_faces = visibility;
    LAB_ViewBuildMeshNeighbored(view, chunk_entry, chunk_neighborhood, visibility);
    return 1;
}


LAB_HOT
LAB_STATIC void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27], unsigned visibility)
{
    const int X = 1;
    const int Y = 3*1;
    const int Z = 3*3*1;

    LAB_View_Mesh* alpha_pass = &chunk_entry->render_passes[LAB_RENDER_PASS_ALPHA];
    size_t old_alpha_capacity = alpha_pass->capacity;
    size_t old_alpha_size = alpha_pass->size;

    for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
        chunk_entry->render_passes[i].size = 0;

    for(size_t z = 0; z < LAB_CHUNK_SIZE; ++z)
    for(size_t y = 0; y < LAB_CHUNK_SIZE; ++y)
    for(size_t x = 0; x < LAB_CHUNK_SIZE; ++x)
    {
        if(cnk3x3x3[X+Y+Z]->blocks[LAB_CHUNK_OFFSET(x, y, z)]->flags & LAB_BLOCK_VISUAL)
        {
            LAB_ViewBuildMeshBlock(view, chunk_entry, cnk3x3x3, x, y, z, visibility);
        }
    }

    // allocate/reallocate memory for mesh_order
    // if old_mesh_count wasn't NULL previously
    //     if old_mesh_count < mesh_count then
    //         add indices counting upwards
    //     else
    //         go through mesh_order [0, old_mesh_count)
    //         (this is possible because mesh_capacity never shrinks
    //         and remove triangles that have indices >= mesh_count
    if(old_alpha_capacity != alpha_pass->capacity || chunk_entry->mesh_order == NULL)
    {
        LAB_ASSUME(old_alpha_capacity <= alpha_pass->capacity);
        // Never shrinks memory
        LAB_TriangleOrder* mesh_order = LAB_ReallocN(chunk_entry->mesh_order, alpha_pass->capacity, sizeof*mesh_order);
        if(!mesh_order)
        {
            // Memory error
            // render without indices
            LAB_Free(chunk_entry->mesh_order);
            chunk_entry->mesh_order = NULL;
            return;
        }
        chunk_entry->mesh_order = mesh_order;


        LAB_BuildModelOrder(chunk_entry->mesh_order, alpha_pass->size);
    }
    else if(old_alpha_size != alpha_pass->size)
    {
        if(old_alpha_size < alpha_pass->size)
        {
            size_t index = 3*old_alpha_size;
            for(size_t i = old_alpha_size; i < alpha_pass->size; ++i)
            {
                chunk_entry->mesh_order[i].v[0] = index++;
                chunk_entry->mesh_order[i].v[1] = index++;
                chunk_entry->mesh_order[i].v[2] = index++;
            }
        }
        else
        {
            // next empty slot
            size_t j = 0;
            size_t max = 3*alpha_pass->size;
            for(size_t i = 0; i < old_alpha_size; ++i)
            {
                if(chunk_entry->mesh_order[i].v[0] < max)
                {
                    if(i!=j)
                    {
                        chunk_entry->mesh_order[j] = chunk_entry->mesh_order[i];
                    }
                    ++j;
                }
                else
                {
                    // Do nothing
                }
            }
        }
    }
}


// TODO move to world
LAB_HOT LAB_ALWAYS_INLINE
LAB_BlockFlags LAB_GetNeighborhoodBlockFlags(LAB_Chunk* neighborhood[27], int x, int y, int z)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
    if(LAB_UNLIKELY(chunk == NULL)) return 0;
    return chunk->blocks[block_index]->flags;
}


LAB_HOT LAB_ALWAYS_INLINE
LAB_STATIC void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27], int x, int y, int z, unsigned visibility)
{

#define GET_BLOCK(bx, by, bz) LAB_GetNeighborhoodBlock(cnk3x3x3, x+(bx), y+(by), z+(bz))
#define GET_BLOCK_FLAGS(bx, by, bz) (GET_BLOCK(bx, by, bz)->flags)

    LAB_Block* tmp_block;
#define IS_BLOCK_OPAQUE(bx, by, bz) (tmp_block=GET_BLOCK(bx, by, bz), (!!(tmp_block->flags&LAB_BLOCK_OPAQUE)) | ((tmp_block==block) & (!!(tmp_block->flags&LAB_BLOCK_OPAQUE_SELF))))

    LAB_Block* block = cnk3x3x3[1+3+9]->blocks[LAB_CHUNK_OFFSET(x, y, z)];

    int faces = 0;
    faces |=  1*(!(IS_BLOCK_OPAQUE(-1, 0, 0)));
    faces |=  2*(!(IS_BLOCK_OPAQUE( 1, 0, 0)));
    faces |=  4*(!(IS_BLOCK_OPAQUE( 0,-1, 0)));
    faces |=  8*(!(IS_BLOCK_OPAQUE( 0, 1, 0)));
    faces |= 16*(!(IS_BLOCK_OPAQUE( 0, 0,-1)));
    faces |= 32*(!(IS_BLOCK_OPAQUE( 0, 0, 1)));
    if(faces == 0) return;

    int lum_faces = 63; // all


    LAB_View_Mesh* mesh = &chunk_entry->render_passes[block->model->render_pass];

    //#define MAP_LIGHT(x) (view->flags&LAB_VIEW_BRIGHTER?LAB_HighColor2(x):(x))
    //#define MAP_LIGHT(x) (view->flags&LAB_VIEW_BRIGHTER?LAB_MixColor50(x, ~0):(x))
    //#define MAP_LIGHT(x) (view->flags&LAB_VIEW_BRIGHTER?~LAB_MulColor(LAB_MulColor(~(x), ~(x)), LAB_MulColor(~(x), ~(x))):(x))
    // before mixing
    //#define MAP_LIGHT_0(x) LAB_ColorHI4(x)
    #define MAP_LIGHT_0(x) (x)
    #define MAP_LIGHT(x) (view->flags&LAB_VIEW_BRIGHTER?~LAB_MulColor_Fast(LAB_MulColor_Fast(~(x), ~(x)), LAB_MulColor_Fast(~(x), ~(x))):(x))
    if(block->flags&LAB_BLOCK_NOSHADE)
    {
        const LAB_Model* model = block->model;
        if(!model) return;
        LAB_Triangle* tri;
        tri = LAB_ViewMeshAlloc(mesh, model->size, 0);
        if(LAB_UNLIKELY(tri == NULL)) return;
        int count = LAB_PutModelAt(tri, model, x, y, z, faces, visibility);
        mesh->size -= model->size-count;
    }
    else if((view->flags&LAB_VIEW_FLAT_SHADE)||(block->flags&LAB_BLOCK_FLAT_SHADE))
    {
        LAB_Color light_sides[7];

        int face;
        LAB_DIR_EACH(lum_faces, face,
        {
            light_sides[face] = MAP_LIGHT(MAP_LIGHT_0(LAB_GetNeighborhoodLight(cnk3x3x3, x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face), LAB_RGB(255, 255, 255))));
        });

        light_sides[6] = MAP_LIGHT(MAP_LIGHT_0(LAB_GetNeighborhoodLight(cnk3x3x3, x, y, z, LAB_RGB(255, 255, 255))));


        const LAB_Model* model = block->model;
        if(!model) return;
        LAB_Triangle* tri;
        tri = LAB_ViewMeshAlloc(mesh, model->size, 0);
        if(LAB_UNLIKELY(tri == NULL)) return;
        int count = LAB_PutModelShadedAt(tri, model, x, y, z, faces, visibility, light_sides);
        mesh->size -= model->size-count;
    }
    else
    {
        LAB_Color default_color = chunk_entry->y<-2 ? LAB_RGB(15, 15, 15) : LAB_RGB(255, 255, 255);
        LAB_Color light_sides[7][4];

        #define XX(xd, yd, zd) LAB_GetNeighborhoodLight(cnk3x3x3, x+ox+(xd), y+oy+(yd), z+oz+(zd), default_color)
        int face;
        LAB_DIR_EACH(lum_faces, face,
        {
            //const int* o = LAB_offset[face];
            int ox = LAB_OX(face);
            int oy = LAB_OY(face);
            int oz = LAB_OZ(face);

            int ax = LAB_AX(face>>1); // LAB_offsetA[face>>1][0];
            int ay = LAB_AY(face>>1); // LAB_offsetA[face>>1][1];
            int az = LAB_AZ(face>>1); // LAB_offsetA[face>>1][2];
            int bx = LAB_BX(face>>1); // LAB_offsetB[face>>1][0];
            int by = LAB_BY(face>>1); // LAB_offsetB[face>>1][1];
            int bz = LAB_BZ(face>>1); // LAB_offsetB[face>>1][2];

            LAB_Color tmp[9];
            for(int v = -1; v <= 1; ++v)
            for(int u = -1; u <= 1; ++u)
            {
                int index = 3*(1+v) + 1+u;
                if(v && u && (GET_BLOCK_FLAGS(     v*bx+ox,      v*by+oy,      v*bz+oz)&LAB_BLOCK_OPAQUE)
                          && (GET_BLOCK_FLAGS(u*ax     +ox, u*ay     +oy, u*az     +oz)&LAB_BLOCK_OPAQUE))
                {
                    //tmp[index] = LAB_MinColor(LAB_MaxColor(
                    //                XX(     v*bx,      v*by,      v*bz),
                    //                XX(u*ax     , u*ay     , u*az     )),
                    //                XX(u*ax+v*bx, u*ay+v*by, u*az+v*bz));
                    tmp[index] = MAP_LIGHT_0(LAB_MaxColor(
                                    XX(     v*bx,      v*by,      v*bz),
                                    XX(u*ax     , u*ay     , u*az     )));
                }
                else
                    tmp[index] = MAP_LIGHT_0(XX(u*ax+v*bx, u*ay+v*by, u*az+v*bz));
            }

            /*light_sides[face][0] = XX(    0,     0,     0);
            light_sides[face][1] = XX(   ax,    ay,    az);
            light_sides[face][2] = XX(bx   , by   , bz   );
            light_sides[face][3] = XX(bx+ax, by+ay, bz+az);*/

            light_sides[face][0] = MAP_LIGHT(LAB_MixColor4x25(tmp[0], tmp[1], tmp[3], tmp[4]));
            light_sides[face][1] = MAP_LIGHT(LAB_MixColor4x25(tmp[1], tmp[2], tmp[4], tmp[5]));
            light_sides[face][2] = MAP_LIGHT(LAB_MixColor4x25(tmp[3], tmp[4], tmp[6], tmp[7]));
            light_sides[face][3] = MAP_LIGHT(LAB_MixColor4x25(tmp[4], tmp[5], tmp[7], tmp[8]));

        });
        #undef XX

        light_sides[6][0] =
        light_sides[6][1] =
        light_sides[6][2] =
        light_sides[6][3] = MAP_LIGHT(LAB_GetNeighborhoodLight(cnk3x3x3, x, y, z, LAB_RGB(255, 255, 255)));


        const LAB_Model* model = block->model;
        if(!model) return;
        LAB_Triangle* tri;
        tri = LAB_ViewMeshAlloc(mesh, model->size, 0);
        if(LAB_UNLIKELY(tri == NULL)) return;
        int count = LAB_PutModelSmoothShadedAt(tri, model, x, y, z, faces, visibility, (const LAB_Color(*)[4])light_sides);
        mesh->size -= model->size-count;
    }


#undef GET_BLOCK_FLAGS
#undef GET_BLOCK

}


LAB_STATIC LAB_Triangle* LAB_ViewMeshAlloc(LAB_View_Mesh* mesh, size_t add_size, size_t extra_size)
{
    size_t mesh_count, new_mesh_count, mesh_capacity;

    mesh_count = mesh->size;
    new_mesh_count = mesh_count+add_size;
    mesh_capacity = mesh->capacity;

    //if(LAB_UNLIKELY(new_mesh_count+extra_size > mesh_capacity))
    if(new_mesh_count+extra_size > mesh_capacity)
    {
        if(mesh_capacity == 0) mesh_capacity = 1;
        while(new_mesh_count+extra_size > mesh_capacity) mesh_capacity *= 2;
        LAB_Triangle* mesh_data = LAB_ReallocN(mesh->data, mesh_capacity, sizeof *mesh_data);
        if(!mesh_data) {
            return NULL;
        }
        mesh->data = mesh_data;
        mesh->capacity=mesh_capacity;
    }
    mesh->size=new_mesh_count;

    return &mesh->data[mesh_count];
}

LAB_STATIC void LAB_ViewUploadVBO(LAB_View* view, LAB_View_Mesh* mesh)
{
    LAB_Triangle* mesh_data = mesh->data;

    if(!mesh->vbo)
    {
        LAB_GL_ALLOC(glGenBuffers, 1, &mesh->vbo);
    }

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->size*sizeof *mesh_data, mesh_data, GL_DYNAMIC_DRAW);
}

LAB_STATIC bool LAB_ViewBuildChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    // TODO: only build chunk if all neighbors are generated
    if(!LAB_View_IsChunkInSight(view, chunk_entry->x, chunk_entry->y, chunk_entry->z))
        return 0;


    unsigned visibility = LAB_View_ChunkVisibility(view, chunk_entry->x, chunk_entry->y, chunk_entry->z);

    // TODO: enshure light update after at most 1 sec
    if(     (chunk_entry->dirty&LAB_CHUNK_UPDATE_LOCAL)
        || ((chunk_entry->dirty&LAB_CHUNK_UPDATE_LIGHT) && (rand()&0x1f)==0)
        || ((chunk_entry->visible_faces&visibility) != visibility) // some faces not visible
        || ( chunk_entry->visible_faces!=visibility && (rand()&0x1f)==0)) // some faces not hidden -> free memory
    {
        bool chunk_available = LAB_ViewBuildMesh(view, chunk_entry, view->world, visibility);
        if(!chunk_available)
        {
            //chunk_entry->dirty = ~0;
            //chunk_entry->exist =  0;
            //printf("FAILED to build mesh for %i %i %i\n", chunk_entry->x, chunk_entry->y, chunk_entry->z);
            return 0;
        }
        chunk_entry->dirty = 0;
        chunk_entry->exist = 1;
        chunk_entry->upload_vbo = 1;

        if(view->flags & LAB_VIEW_USE_VBO)
            //LAB_ViewUploadVBO(view, chunk_entry);
            LAB_ViewUpdateChunk(view, chunk_entry);

        return 1;
    }
    return 0;
}

LAB_STATIC bool LAB_ViewUpdateChunk(LAB_View* view, LAB_ViewChunkEntry* e)
{
    bool updated = LAB_ViewBuildChunk(view, e);
    if(e->mesh_order && (updated || (rand()&0x1f) == 0))
    //if(e->mesh_order)
    {
        LAB_View_Mesh* alpha_pass = &e->render_passes[LAB_RENDER_PASS_ALPHA];
        float cam[3];
        cam[0] = view->x-e->x*16;
        cam[1] = view->y-e->y*16;
        cam[2] = view->z-e->z*16;
        LAB_SortModelOrder(e->mesh_order, alpha_pass->data, alpha_pass->size, cam);
    }
    return updated;
}


LAB_STATIC void LAB_ViewUpdateChunks(LAB_View* view)
{
    unsigned rest_update = view->max_update;

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
        if((cx-px)*(cx-px) + (cy-py)*(cy-py) + (cz-pz)*(cz-pz) <= dist_sq
           && e->visible)
            rest_update -= LAB_ViewUpdateChunk(view, e);
    });
}
// TODO use glMultiDrawElements
LAB_STATIC bool LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_RenderPass pass)
{
    //if(view->flags & LAB_VIEW_NO_RENDER) return 0;
    if(!LAB_View_IsChunkInSight(view, chunk_entry->x, chunk_entry->y, chunk_entry->z))
        return 0;

    if(!chunk_entry->exist)
        return 0;

    LAB_View_Mesh* mesh = &chunk_entry->render_passes[pass];

    if(mesh->size == 0) return 0;

    if(view->flags & LAB_VIEW_USE_VBO)
    {
        if(!mesh->vbo) return 0;
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    }

    glPushMatrix();
    glTranslatef(LAB_CHUNK_SIZE*chunk_entry->x-view->x, LAB_CHUNK_SIZE*chunk_entry->y-view->y, LAB_CHUNK_SIZE*chunk_entry->z-view->z);
    glScalef(1.00001, 1.00001, 1.00001); // Reduces gaps/lines between chunks
    //glScalef(0.9990, 0.9990, 0.9990);

    LAB_Triangle* mesh_data;

    mesh_data = view->flags & LAB_VIEW_USE_VBO ? 0 /* Origin of vbo is at 0 */ : mesh->data;

    glVertexPointer(3, LAB_GL_TYPEOF(mesh_data->v[0].x), sizeof *mesh_data->v, &mesh_data->v[0].x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof *mesh_data->v, &mesh_data->v[0].color);
    glTexCoordPointer(2, LAB_GL_TYPEOF(mesh_data->v[0].u), sizeof *mesh_data->v, &mesh_data->v[0].u);

    if(pass == LAB_RENDER_PASS_ALPHA && chunk_entry->mesh_order)
    {
        glDrawElements(GL_TRIANGLES, 3*mesh->size, GL_UNSIGNED_INT, chunk_entry->mesh_order);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, 3*mesh->size);
    }
    glPopMatrix();

    return 1;
}




void LAB_ViewRenderChunks(LAB_View* view, LAB_RenderPass pass)
{
    //if(view->flags & LAB_VIEW_NO_RENDER) return;

    int chunks_rendered = 0;

    int backwards = LAB_PrepareRenderPass(pass);

    int px = LAB_Sar(LAB_FastFloorF2I(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view->z), LAB_CHUNK_SHIFT);

    int dist_sq = view->render_dist*view->render_dist + 3;
    LAB_ViewChunkEntry* e;
    int start = backwards ? (int)view->chunks.size-1 : 0;
    int stop  = backwards ? -1 : (int)view->chunks.size;
    int step  = backwards ? -1 : 1;
    for(int i = start; i != stop; i+=step)
    {
        e = view->sorted_chunks[i].entry;
        if((e->x-px)*(e->x-px) + (e->y-py)*(e->y-py) + (e->z-pz)*(e->z-pz) <= dist_sq
           && e->visible)
        {
            chunks_rendered += (int)LAB_ViewRenderChunk(view, e, pass);
        }
    }

    if(view->flags & LAB_VIEW_USE_VBO)
        glBindBuffer(GL_ARRAY_BUFFER, 0);

#if 0
    if(pass == 0)
        printf("chunks_rendered:");
    printf(" %3i", chunks_rendered);
    if(pass == LAB_RENDER_PASS_COUNT-1)
        printf("         \r");
#endif
}




LAB_STATIC void LAB_View_UploadChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    if(!LAB_View_IsChunkInSight(view, chunk_entry->x, chunk_entry->y, chunk_entry->z))
        return;

    if(!chunk_entry->exist)
        return;

    if((view->flags & LAB_VIEW_USE_VBO) && chunk_entry->upload_vbo)
    {
        for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
        {
            LAB_View_Mesh* mesh = &chunk_entry->render_passes[i];

            if(mesh->size == 0) continue;

            LAB_ViewUploadVBO(view, mesh);
        }
        chunk_entry->upload_vbo = 0;
    }
}

LAB_STATIC void LAB_View_UploadChunks(LAB_View* view)
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
        if((cx-px)*(cx-px) + (cy-py)*(cy-py) + (cz-pz)*(cz-pz) <= dist_sq
           && e->visible)
            LAB_View_UploadChunk(view, e);
    });
}




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
    #if 0
    unsigned visible = 2;
    glGetQueryObjectuiv(entry->query_id, GL_QUERY_RESULT, &visible);
    LAB_ASSUME(visible < 2); // either 0 or 1
    entry->visible = !!visible;
    //entry->visible = 1; // DBG

    glDeleteQueries(1, &entry->query_id); LAB_GL_DEBUG_FREE(1);
    entry->query_id = 0;
    LAB_GL_CHECK();
    #else
    unsigned available, visible;
    glGetQueryObjectuiv(entry->query_id, GL_QUERY_RESULT_AVAILABLE, &available);
    if(available)
    {
        glGetQueryObjectuiv(entry->query_id, GL_QUERY_RESULT, &visible);
        entry->visible = visible;

        LAB_GL_FREE(glDeleteQueries, 1, &entry->query_id);
        entry->query_id = 0;
    }
    else
    {
        //printf("Query not ready\n");
    }
    //printf("available = %i\n", available);
    LAB_GL_CHECK();
    #endif
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
            e->visible = 1;
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
    unsigned query_id = entry->query_id;
    #endif
    LAB_ASSUME(query_id != 0);
    //LAB_ASSUME(glIsQuery(query_id));
    //if(!glIsQuery(query_id)) printf("%i is not a query\n", query_id);


    // TODO fixed sized buffer of queries, glGetQueryObject called
    //      in the next frame, in LAB_View_Tick
    glPushMatrix();
    #define DBG_QUERY 0
    #if !DBG_QUERY
    glColorMask(0, 0, 0, 0);
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

    #if DBG_QUERY
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

    glDepthMask(1);
    glColorMask(1, 1, 1, 1);
    glPopMatrix();

    #if LAB_VIEW_QUERY_IMMEDIATELY
    unsigned visible = 1;
    glGetQueryObjectuiv(query_id, GL_QUERY_RESULT, &visible);
    entry->visible = visible;
    #endif

    entry->do_query = 0;
    LAB_GL_CHECK();
}

















LAB_STATIC void LAB_RenderBox(LAB_View* view, float x, float y, float z, float w, float h, float d)
{
//#define O (-0.001f)
//#define I ( 1.001f)
#define O 0
#define I 1
    static const float box[] = {
        O, O, O, /*--*/ I, O, O,
        O, O, O, /*--*/ O, I, O,
        O, O, O, /*--*/ O, O, I,

        I, I, O, /*--*/ O, I, O,
        I, I, O, /*--*/ I, O, O,
        I, I, O, /*--*/ I, I, I,

        O, I, I, /*--*/ I, I, I,
        O, I, I, /*--*/ O, O, I,
        O, I, I, /*--*/ O, I, O,

        I, O, I, /*--*/ O, O, I,
        I, O, I, /*--*/ I, I, I,
        I, O, I, /*--*/ I, O, O,
    };
#undef I
#undef O

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x-view->x, y-view->y, z-view->z);
    glScalef(w, h, d);
    glDisable(GL_LINE_SMOOTH);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, box);
    glLineWidth(2);
    glDepthRange(0, 0.9999);
    glDrawArrays(GL_LINES, 0, sizeof(box)/sizeof(*box)/3);
    glDepthRange(0, 1);
    glPopMatrix();
}


LAB_STATIC void LAB_View_RenderBlockSelection(LAB_View* view)
{
    int target[3]; // targeted block
    int prev[3]; // previous block
    float hit[3]; // hit pos

    // view-pos
    float vpos[3];
    // view-dir
    float dir[3];

    vpos[0] = view->x;
    vpos[1] = view->y;
    vpos[2] = view->z;
    LAB_ViewGetDirection(view, dir);

    if(LAB_TraceBlock(view->world, 10, vpos, dir, LAB_CHUNK_GENERATE, LAB_BLOCK_INTERACTABLE, target, prev, hit))
    {
        glEnable(GL_BLEND);
        //glEnable(GL_LINE_SMOOTH);
        //glHint(GL_LINE_SMOOTH, GL_NICEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0, 0, 0, 0.7);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glScalef(0.99, 0.99, 0.99);

        if(memcmp(target, prev, sizeof target) != 0)
            LAB_RenderBox(view, target[0], target[1], target[2], 1, 1, 1);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        //glColor4f(0, 0, 0, 0.1);
        //LAB_RenderBlockSelection(prev[0], prev[1], prev[2]);
    }
}

void LAB_ViewRenderHud(LAB_View* view)
{
    LAB_View_RenderBlockSelection(view);

    float pix = 1.f/(float)view->h;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);

    #if 0
    static const float crosshair[2*3*4] = {
            0, -0.1,
         0.05, -0.2,
        -0.05, -0.2,
        //
         0.1,     0,
         0.2,  0.05,
         0.2, -0.05,
        //
            0,  0.1,
        -0.05,  0.2,
         0.05,  0.2,
        //
        -0.1,     0,
        -0.2, -0.05,
        -0.2,  0.05,
    };
    #else
    #define A 0.2
    #define B 0.01
    static const float crosshair[] = {
         B,  A,
         B, -A,
        -B, -A,
        //
        -B, -A,
        -B,  A,
         B,  A,
        //
        //
        -B,  B,
        -A, -B,
        -A,  B,
        //
        -A, -B,
        -B,  B,
        -B, -B,
        //
         A,  B,
         B, -B,
         B,  B,
        //
         B, -B,
         A,  B,
         A, -B,
    };
    #endif

    //glColor3f(1,1,1);
    glTranslatef(0,0,-5);
    //glTranslatef(0,0,-1);
    //glScalef(1.f/5.f, 1/5.f, 1);
    glVertexPointer(2, LAB_GL_TYPEOF(crosshair[0]), 0, crosshair);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
    glColor3f(1,1,1);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(crosshair)/(2*sizeof(crosshair[0])));
    glTranslatef(2*5*pix, -2*5*pix, 0);
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glColor3f(0.3,0.3,0.3);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(crosshair)/(2*sizeof(crosshair[0])));

    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
    glColor3f(1,1,1);
    {
        TTF_Font* font = view->gui_mgr.mono_font;
        //TTF_Font* font = LAB_GuiMonospaceFont();
        //if(!font) return;

        int rerender = 0;
        int px, py, pz;
        px = LAB_FastFloorF2I(view->x);
        py = LAB_FastFloorF2I(view->y);
        pz = LAB_FastFloorF2I(view->z);
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


        LAB_GL_ActivateTexture(&view->info.gl_texture);
        if(rerender)
        {
            if(view->info.surf != NULL) LAB_SDL_FREE(SDL_FreeSurface, &view->info.surf);
            //LAB_SDL_FREE(SDL_FreeSurface, &view->info.surf);

            char buf[64];
            snprintf(buf, sizeof(buf), "%i %i %i", px, py, pz);
            //char buf[64];
            //snprintf(buf, sizeof(buf), "%i %i %i - %i", px, py, pz,
            //         LAB_gl_debug_alloc_count);
            //char buf[512];
            //snprintf(buf, sizeof(buf), "%i %i %i",
            //         px, py, pz,
            //         view->world->chunks.size,
            //         view->chunk_capacity);

            SDL_Color fg = { 255, 255, 255, 255 };
            SDL_Color bg = {   0,   0,   0, 255 };

            LAB_SDL_ALLOC(TTF_RenderUTF8_Shaded, &view->info.surf, font, buf, fg, bg);
            if(!view->info.surf) return;


            LAB_GL_UploadSurf(view->info.gl_texture, view->info.surf);
        }
        //static unsigned scale_i = 0;
        //scale_i++; scale_i &= 0xff;
        //int scale = scale_i > 0x80 ? 2 : 1;
        int scale = 1;
        LAB_GL_DrawSurf(view->info.gl_texture, 0, view->h-scale*view->info.surf->h, scale*view->info.surf->w, scale*view->info.surf->h, view->w, view->h);
    }
}

void LAB_ViewRenderInit(LAB_View* view)
{
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // TODO
}

void LAB_ViewRenderProc(void* user, LAB_Window* window)
{
    // NO access to world here -> world can update whilst rendering
    LAB_View* view = (LAB_View*)user;

    // Block rendering settings
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // Sky color
    float f = LAB_MIN(LAB_MAX((view->y+64+32)*(1./64.), 0), 1);
    f*=f;
    glClearColor(0.5*f, 0.7*f, 0.9*f, 1);
    /*if(view->y <= -16*4)
        //glClearColor(0.03, 0.03, 0.03, 1);
        glClearColor(0.0, 0.0, 0.0, 1);
    else
        glClearColor(0.5, 0.7, 0.9, 1);*/

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int w, h;
    SDL_GetWindowSize(window->window, &w, &h);
    view->w = w; view->h = h;
    float ratio = h?(float)w/(float)h:1;
    float nearp = 0.075f;
    float fov = 1;
    float far = view->render_dist*16+32;
    glFrustum(-fov*nearp*ratio, fov*nearp*ratio, -fov*nearp, fov*nearp, nearp, far);

    // Setup world matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(view->az, 0, 0, 1);
    glRotatef(view->ax, 1, 0, 0);
    glRotatef(view->ay, 0, 1, 0);
    //glRotatef(view->az, 0, 0, 1);
    //glTranslatef(-view->x, -view->y, -view->z);

    if(view->flags&LAB_VIEW_SHOW_CHUNK_GRID)
    {
        float xx, yy, zz;
        xx = LAB_FastFloorF2I(view->x)&~15;
        yy = LAB_FastFloorF2I(view->y)&~15;
        zz = LAB_FastFloorF2I(view->z)&~15;
        glEnable(GL_DEPTH_TEST);
        glColor3f(1, 1, 1);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glScalef(0.99, 0.99, 0.99);

        LAB_RenderBox(view, xx, yy, zz, 16, 16, 16);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    // Block rendering settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, LAB_block_terrain_gl_id);

    /*if(view->flags&)
    {
        glEnable(GL_POLYGON_OFFSET_EXT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1, -1);
    }
    else
    {
        glDisable(GL_POLYGON_OFFSET_EXT);
    }*/

    glEnableClientState(GL_VERTEX_ARRAY); // TODO once
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //glDisable(GL_BLEND);

   // glEnable(GL_ALPHA_TEST);
    //glAlphaFunc(GL_GEQUAL, 1/255.f);
   // glAlphaFunc(GL_GEQUAL, 32/255.f);
    //glAlphaFunc(GL_GEQUAL, 64/255.f);


    if(view->flags & LAB_VIEW_USE_VBO)
        LAB_View_UploadChunks(view);


    LAB_ViewRenderChunks(view, LAB_RENDER_PASS_SOLID);
    LAB_ViewRenderChunks(view, LAB_RENDER_PASS_MASKED);
    LAB_ViewRenderChunks(view, LAB_RENDER_PASS_BLIT);

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

    LAB_ViewRenderChunks(view, LAB_RENDER_PASS_ALPHA);


    // TODO: remove this
    glDisable(GL_ALPHA_TEST);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);


    // Render Crosshair
    if(view->flags & LAB_VIEW_SHOW_HUD)
        LAB_ViewRenderHud(view);

    // Gui rendering settings
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glColor4f(1,1,1,1);
    LAB_GuiManager_Render(&view->gui_mgr, view->w, view->h);

    if(view->flags & LAB_VIEW_SHOW_FPS_GRAPH)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glScalef(2*(float)view->w/(float)view->h, 2, 1);
        glTranslatef(-0.5,-0.5,-1);
        glLineWidth(2);
        glEnable(GL_LINE_SMOOTH);
        LAB_FpsGraph_Render_Prepare();
        LAB_FpsGraph_Render_Base();
        LAB_FpsGraph_Render(&view->fps_graph);
        LAB_FpsGraph_Render(&view->fps_graph_input);
        LAB_FpsGraph_Render(&view->fps_graph_world);
        LAB_FpsGraph_Render(&view->fps_graph_view);
        glPopMatrix();
    }

    LAB_GL_CHECK();
}


void LAB_ViewRemoveDistantChunks(LAB_View* view)
{
    int px = LAB_Sar(LAB_FastFloorF2I(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view->z), LAB_CHUNK_SHIFT);

    int dist_sq = view->keep_dist*view->keep_dist + 3;

    LAB_ViewChunkEntry* e;
    HTL_HASHARRAY_REMOVE_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
                               (e->x-px)*(e->x-px) + (e->y-py)*(e->y-py) + (e->z-pz)*(e->z-pz) > dist_sq,
                               {LAB_ViewUnlinkChunk(view, e);});
}





LAB_ViewChunkEntry* LAB_ViewFindChunkEntry(LAB_View* view, int x, int y, int z)
{
    LAB_ChunkPos pos = {x, y, z};
    LAB_ViewChunkEntry** entry = LAB_View_ChunkTBL_Get(&view->chunks, pos);
    return entry?*entry:NULL;
}

/**
 *  Assume: x,y,z is not contained in view->chunks
 */
LAB_ViewChunkEntry* LAB_ViewNewChunkEntry(LAB_View* view, int x, int y, int z)
{
    LAB_ChunkPos pos = {x, y, z};
    LAB_ViewChunkEntry** entry;
    entry = LAB_View_ChunkTBL_PutAlloc(&view->chunks, pos);
    if(LAB_UNLIKELY(!entry)) return NULL;
    *entry = LAB_Calloc(1, sizeof**entry);
    if(!*entry)
    {
        LAB_View_ChunkTBL_Discard(&view->chunks, entry);
        return NULL;
    }

    // memset 0 -> calloc
    (*entry)->x = x;
    (*entry)->y = y;
    (*entry)->z = z;
    for(int face = 0; face < 6; ++face)
    {
        LAB_ViewChunkEntry* neighbor = LAB_ViewFindChunkEntry(view, x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face));
        if(neighbor)
        {
            (*entry)->neighbors[face]   = neighbor;
            neighbor->neighbors[face^1] = *entry;
        }
    }
    return *entry;
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

    HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    {
        e->dirty = ~0;
    });

    if(free_buffers)
    {
        HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
        {
            for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
            {
                unsigned* p_vbo = &e->render_passes[i].vbo;
                if(*p_vbo)
                {
                    LAB_GL_FREE(glDeleteBuffers, 1, p_vbo);
                    *p_vbo = 0;
                }
            }
        });
    }
}



void LAB_ViewGetDirection(LAB_View* view, LAB_OUT float dir[3])
{
    float ax, ay;
    float sax, cax, say, cay;

    ax = view->ax*LAB_PI/180.f;
    ay = view->ay*LAB_PI/180.f;

    // ax = 0; //DBG

    sax = sin(ax);
    cax = cos(ax);
    say = sin(ay);
    cay = cos(ay);

    #if 1
    dir[0] = cax*say;
    dir[1] = -sax;
    dir[2] = -cax*cay;
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

    dir[0] =  dx*caz + dy*saz;
    dir[1] = -dx*saz + dy*caz;
    dir[2] =  dz;
    #endif
}


LAB_STATIC int LAB_View_CompareChunksIndirect(const void* a, const void* b)
{
    const LAB_ViewSortedChunkEntry* e1 = a,* e2 = b;

    return e1->distance < e2->distance ? -1 : e1->distance > e2->distance ? 1 : 0;
}

LAB_STATIC void LAB_View_SortChunks(LAB_View* view, uint32_t delta_ms)
{
    LAB_ViewSortedChunkEntry* sorted_chunks = LAB_ReallocN(view->sorted_chunks, view->chunks.size, sizeof*sorted_chunks);
    if(!sorted_chunks)
    {
        view->sorted_chunks = NULL;
        perror("You might reduce render distance");
        abort();
    }
    view->sorted_chunks = sorted_chunks;
    int i = 0;

    LAB_ViewChunkEntry* e;
    float dir[3];
    LAB_ViewGetDirection(view, dir);
    HTL_HASHARRAY_EACH_DEREF(LAB_View_ChunkTBL, &view->chunks, e,
    {
        view->sorted_chunks[i].entry = e;

        float x, y, z;
        x = 16*e->x - view->x; if(x<0) { if(x<-16) x+=16; else x=0; }
        y = 16*e->y - view->y; if(y<0) { if(y<-16) y+=16; else y=0; }
        z = 16*e->z - view->z; if(z<0) { if(z<-16) z+=16; else z=0; }
        view->sorted_chunks[i].distance = x*x+y*y+z*z;

        ++i;
    });

    qsort(view->sorted_chunks, view->chunks.size, sizeof(*view->sorted_chunks), LAB_View_CompareChunksIndirect);
}


void LAB_ViewTick(LAB_View* view, uint32_t delta_ms)
{
    LAB_ViewRemoveDistantChunks(view);
    LAB_ViewLoadNearChunks(view);
    #if !LAB_VIEW_QUERY_IMMEDIATELY
    LAB_View_FetchQueryChunks(view);
    #endif
    LAB_ViewUpdateChunks(view);
    //LAB_FpsGraph_AddSample(&view->fps_graph, delta_ms);
    LAB_GuiManager_Tick(&view->gui_mgr);
    LAB_View_SortChunks(view, delta_ms);
}


LAB_STATIC bool LAB_View_HasChunkVisibleNeighbors(LAB_View* view, int x, int y, int z)
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
}


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


bool LAB_View_IsChunkInSight(LAB_View* view, int cx, int cy, int cz)
{
    //if(cy < -2 || cy >= 0) return 0; // DBG
    // TODO: might be inaccurate for large coordinates
    float dir[3];
    LAB_ViewGetDirection(view, dir);

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

    return dist >= treshold;
}


// Difference of following functions: comparison operators: <= >= vs < >
LAB_STATIC unsigned LAB_View_ChunkVisibility(LAB_View* view, int cx, int cy, int cz)
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
}

LAB_STATIC unsigned LAB_View_ChunkNeighborVisibility(LAB_View* view, int cx, int cy, int cz)
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

    int px = LAB_Sar(LAB_FastFloorF2I(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view->z), LAB_CHUNK_SHIFT);

    /**

        pyramid index i;

        8 Quadrants
        the following are crosssections

        x 0 0 0 | 0 0 0 x     x 0 0 0 | 0 0 0 x
        3 x 0 0 | 0 0 x 1     3 x 0 0 | 0 0 x 1
        3 3 x 0 | 0 x 1 1     3 3 y x | x y 1 1
        3 3 3 y | y 1 1 1     3 3 x 4 | 4 x 1 1
        --------+--------     --------+--------
        3 3 3 y | y 1 1 1     3 3 x 4 | 4 x 1 1
        3 3 x 2 | 2 x 1 1     3 3 y x | x y 1 1
        3 x 2 2 | 2 2 x 1     3 x 2 2 | 2 2 x 1
        x 2 2 2 | 2 2 2 x     x 2 2 2 | 2 2 2 x


        x 0 0 0 | 0 0 0 x     y x x x | x x x y
        3 y x x | x x y 1     x 4 4 4 | 4 4 4 x
        3 x 4 4 | 4 4 x 1     x 4 4 4 | 4 4 4 x
        3 x 4 4 | 4 4 x 1     x 4 4 4 | 4 4 4 x
        --------+--------     --------+--------
        3 x 4 4 | 4 4 x 1     x 4 4 4 | 4 4 4 x
        3 x 4 4 | 4 4 x 1     x 4 4 4 | 4 4 4 x
        3 y x x | x x y 1     x 4 4 4 | 4 4 4 x
        x 2 2 2 | 2 2 2 x     y x x x | x x x y



        Each sector consists of three pyramids
        (with a right angle corner at each center of a surface
         and the tip at the origin, the other corners are either
         at the center of an edge of the cube or at the corner of
         the cube, all three pyramids share that corner)
        and 3 diagnoal triangular planes (x) connecting
        those pyramids, there is a diagonal line (y)
        at the intersection of all triangular planes

        logically the xs are iterated twice and
        the ys are iterated thrice
        the plane is guarded, if i <
        the line is guarded, if i is 0

        a: the offset in the lower axis (ordering: x <<< y <<< z)
           that is not the outward direction (r)
        b: the offset in the higher axis
    **/

    int load_amount = view->load_amount; // should be configurable

    for(int r = 0; r <= (int)view->preload_dist; ++r)
    {
        //for(int a = 0; a <= r/*+(i<=0)*/; ++a)
        //for(int b = 0; b <= r/*+(i<=1)*/; ++b)
        for(int i = 0; i <= r; ++i)
        for(int j = 0; j <= i; ++j)
        {
            for(int k = 0; k <= 1; ++k)
            {
                int a, b;
                if(k && i==j) break /*k*/;
                a = k ? i : j;
                b = k ? j : i;

                for(int f = 0; f < 3; ++f)
                {
                    int x, y, z;
                    switch(f)
                    {
                        case 0: x=r; y=a; z=b; break /*switch*/;
                        case 1: x=a; y=b; z=r; break /*switch*/;
                        case 2: x=b; y=r; z=a; break /*switch*/;
                        default: LAB_UNREACHABLE();
                    }
                    for(int q = 0; q < 8; ++q)
                    {
                        // ~x == -x-1
                        int xx, yy, zz;
                        if(q&1 && x==0) continue; else xx = px+(q&1 ? -x : x);
                        if(q&2 && y==0) continue; else yy = py+(q&2 ? -y : y);
                        if(q&4 && z==0) continue; else zz = pz+(q&4 ? -z : z);

                        if(!LAB_View_IsChunkInSight(view, xx, yy, zz))
                        {
                            LAB_ASSUME(r > 0);
                            continue;
                        }

                        //if(r > 1 && !LAB_View_HasChunkVisibleNeighbors(view, xx, yy, zz))
                        //    continue;

                        LAB_ViewChunkEntry* entry = LAB_ViewFindChunkEntry(view, xx, yy, zz);
                        if(entry == NULL)
                        {
                            entry = LAB_ViewNewChunkEntry(view, xx, yy, zz);
                            if(entry == NULL) return; // NO MEMORY
                            entry->dirty = ~0;
                            --load_amount;
                            LAB_ASSUME(!entry->exist);
                        }
                        if(!entry->exist)
                        {
                            entry->do_query = 1;
                            (void)LAB_GetChunk(view->world, xx, yy, zz, LAB_CHUNK_GENERATE_LATER);
                            if(!load_amount) return;
                        }

                        //(void)LAB_GetChunk(view->world, -xx-1, yy, zz, LAB_CHUNK_GENERATE_LATER);
                    }
                    if(a==r) break /*f*/;
                    if(b==r && f==1) break /*f*/;
                }
            }
        }
    }
}


void LAB_ViewDestructChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
        LAB_View_Mesh_Destroy(&chunk_entry->render_passes[i]);

    if(chunk_entry->mesh_order)
        LAB_Free(chunk_entry->mesh_order);

    #if !LAB_VIEW_QUERY_IMMEDIATELY
    if(chunk_entry->query_id)
    {
        LAB_GL_FREE(glDeleteQueries, 1, &chunk_entry->query_id);
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
    for(int face = 0; face < 6; ++face)
    {
        LAB_ViewChunkEntry* neighbor = chunk_entry->neighbors[face];
        if(neighbor)
        {
            neighbor->neighbors[face^1] = NULL;
        }
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
    LAB_GL_FREE(glDeleteTextures, 1, &info->gl_texture);
    LAB_SDL_FREE(SDL_FreeSurface, &info->surf);
}
