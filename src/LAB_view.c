#include "LAB_view.h"

#include "LAB_memory.h"
#include "LAB_error.h"
#include "LAB_math.h"
#include "LAB_debug.h"

#include "LAB_world.h"
#include "LAB_block.h"
#include "LAB_window.h"
#include "LAB_gl.h"
#include "LAB_asset_manager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

#include "LAB_perf.h"
#include "LAB_bits.h"
#include "LAB_opt.h"
#include "LAB_util.h"

#include "LAB_gui.h"
#include "LAB_gui_component.h"
#include "LAB_gui_menu.h"

/*#define HTL_PARAM LAB_VIEW_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM*/

///############################

static bool LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world);
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_IN LAB_Chunk* chunk_neighborhood[27]);
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_IN LAB_Chunk* chunk_neighborhood[27], int x, int y, int z);
static void LAB_ViewRenderChunks(LAB_View* view);
static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
static bool LAB_View_HasChunkVisibleNeighbors(LAB_View* view, int x, int y, int z);
static void LAB_View_FetchQueryChunks(LAB_View* view);
static void LAB_View_FetchQueryChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
static void LAB_View_OrderQueryChunks(LAB_View* view);
static void LAB_View_OrderQueryChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
static void LAB_ViewRemoveDistantChunks(LAB_View* view);
static void LAB_ViewDestructChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);

static LAB_Triangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size, size_t extra_size);

bool LAB_ConstructView(LAB_View* view, LAB_World* world)
{
    LAB_InitAssets();

    memset(view, 0, sizeof *view);
    view->world = world;

    view->ax = 22;
    view->y = 1.5;

    LAB_FpsGraph_Create(&view->fps_graph);
    LAB_GuiManager_Create(&view->gui_mgr);

    return 1;
}

void LAB_DestructView(LAB_View* view)
{
    LAB_GuiManager_Destroy(&view->gui_mgr);
    LAB_FpsGraph_Destroy(&view->fps_graph);

    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        LAB_ViewDestructChunk(view, &view->chunks[i]);
    }
    LAB_Free(view->chunks);
}


void LAB_ViewChunkProc(void* user, LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update)
{
    LAB_View* view = (LAB_View*)user;

    // TODO: ignore far away chunks

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

    int px = LAB_Sar((int)floorf(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar((int)floorf(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar((int)floorf(view->z), LAB_CHUNK_SHIFT);

    int dx = x-px;
    int dy = y-py;
    int dz = z-pz;
    unsigned int dist = dx*dx+dy*dy+dz*dz;
    return dist <= view->keep_dist*view->keep_dist;
}



/**
 *  Return 1 if the chunk was available
 */
static bool LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world)
{
    LAB_Chunk* chunk_neighborhood[27];

    LAB_GetChunkNeighborhood(world, chunk_neighborhood,
                             chunk_entry->x, chunk_entry->y, chunk_entry->z,
                             LAB_CHUNK_EXISTING);

    if(chunk_neighborhood[1+3+9] == NULL) return 0;
    LAB_ViewBuildMeshNeighbored(view, chunk_entry, chunk_neighborhood);
    return 1;
}


LAB_HOT
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27])
{
    const int X = 1;
    const int Y = 3*1;
    const int Z = 3*3*1;

    chunk_entry->mesh_count = 0;

    for(size_t z = 0; z < LAB_CHUNK_SIZE; ++z)
    for(size_t y = 0; y < LAB_CHUNK_SIZE; ++y)
    for(size_t x = 0; x < LAB_CHUNK_SIZE; ++x)
    {
        if(cnk3x3x3[X+Y+Z]->blocks[LAB_CHUNK_OFFSET(x, y, z)]->flags & LAB_BLOCK_VISUAL)
        {
            LAB_ViewBuildMeshBlock(view, chunk_entry, cnk3x3x3, x, y, z);
        }
    }
}


LAB_HOT LAB_INLINE
LAB_BlockFlags LAB_GetNeighborhoodBlockFlags(LAB_Chunk* neighborhood[27], int x, int y, int z)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
    if(LAB_UNLIKELY(chunk == NULL)) return 0;
    return chunk->blocks[block_index]->flags;
}


LAB_HOT LAB_INLINE
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27], int x, int y, int z)
{

#define GET_BLOCK(bx, by, bz) LAB_GetNeighborhoodBlock(cnk3x3x3, x+(bx), y+(by), z+(bz))
#define GET_BLOCK_FLAGS(bx, by, bz) (GET_BLOCK(bx, by, bz)->flags)

    LAB_Block* block = cnk3x3x3[1+3+9]->blocks[LAB_CHUNK_OFFSET(x, y, z)];

    int faces = 0;
    faces |=  1*(!(GET_BLOCK_FLAGS(-1, 0, 0)&LAB_BLOCK_OPAQUE));
    faces |=  2*(!(GET_BLOCK_FLAGS( 1, 0, 0)&LAB_BLOCK_OPAQUE));
    faces |=  4*(!(GET_BLOCK_FLAGS( 0,-1, 0)&LAB_BLOCK_OPAQUE));
    faces |=  8*(!(GET_BLOCK_FLAGS( 0, 1, 0)&LAB_BLOCK_OPAQUE));
    faces |= 16*(!(GET_BLOCK_FLAGS( 0, 0,-1)&LAB_BLOCK_OPAQUE));
    faces |= 32*(!(GET_BLOCK_FLAGS( 0, 0, 1)&LAB_BLOCK_OPAQUE));
    if(faces == 0) return;

    #define MAP_LIGHT(x) (view->flags&LAB_VIEW_BRIGHTER?LAB_HighColor2(x):(x))
    if((view->flags&LAB_VIEW_FLAT_SHADE)||(block->flags&LAB_BLOCK_FLAT_SHADE))
    {
        LAB_Color light_sides[6];
        for(int face_itr=faces; face_itr; face_itr &= face_itr-1)
        {
            int face = LAB_Ctz(face_itr);
            const int* o = LAB_offset[face];
            light_sides[face] = MAP_LIGHT(LAB_GetNeighborhoodLight(cnk3x3x3, x+o[0], y+o[1], z+o[2], LAB_RGB(255, 255, 255)));
        }


        const LAB_Model* model = block->model;
        if(!model) return;
        LAB_Triangle* tri;
        tri = LAB_ViewMeshAlloc(chunk_entry, model->size, 0);
        if(LAB_UNLIKELY(tri == NULL)) return;
        int count = LAB_PutModelShadedAt(tri, model, x, y, z, faces, light_sides);
        chunk_entry->mesh_count -= model->size-count;
    }
    else
    {
        LAB_Color light_sides[6][4];
        for(int face_itr=faces; face_itr; face_itr &= face_itr-1)
        {
            int face = LAB_Ctz(face_itr);
            const int* o = LAB_offset[face];

            #define XX(xd, yd, zd) LAB_GetNeighborhoodLight(cnk3x3x3, x+o[0]+(xd), y+o[1]+(yd), z+o[2]+(zd), LAB_RGB(16, 16, 16))
            int ax = LAB_offsetA[face>>1][0];
            int ay = LAB_offsetA[face>>1][1];
            int az = LAB_offsetA[face>>1][2];
            int bx = LAB_offsetB[face>>1][0];
            int by = LAB_offsetB[face>>1][1];
            int bz = LAB_offsetB[face>>1][2];

            LAB_Color tmp[9];
            for(int v = -1; v <= 1; ++v)
            for(int u = -1; u <= 1; ++u)
            {
                int index = 3*(1+v) + 1+u;
                tmp[index] = XX(u*ax+v*bx, u*ay+v*by, u*az+v*bz);
            }

            /*light_sides[face][0] = XX(    0,     0,     0);
            light_sides[face][1] = XX(   ax,    ay,    az);
            light_sides[face][2] = XX(bx   , by   , bz   );
            light_sides[face][3] = XX(bx+ax, by+ay, bz+az);*/

            light_sides[face][0] = MAP_LIGHT(LAB_MixColor4x25(tmp[0], tmp[1], tmp[3], tmp[4]));
            light_sides[face][1] = MAP_LIGHT(LAB_MixColor4x25(tmp[1], tmp[2], tmp[4], tmp[5]));
            light_sides[face][2] = MAP_LIGHT(LAB_MixColor4x25(tmp[3], tmp[4], tmp[6], tmp[7]));
            light_sides[face][3] = MAP_LIGHT(LAB_MixColor4x25(tmp[4], tmp[5], tmp[7], tmp[8]));

            #undef XX
        }


        const LAB_Model* model = block->model;
        if(!model) return;
        LAB_Triangle* tri;
        tri = LAB_ViewMeshAlloc(chunk_entry, model->size, 0);
        if(LAB_UNLIKELY(tri == NULL)) return;
        int count = LAB_PutModelSmoothShadedAt(tri, model, x, y, z, faces, light_sides);
        chunk_entry->mesh_count -= model->size-count;
    }


#undef GET_BLOCK_FLAGS
#undef GET_BLOCK

}


static LAB_Triangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size, size_t extra_size)
{
    size_t mesh_count, new_mesh_count, mesh_capacity;

    mesh_count = chunk_entry->mesh_count;
    new_mesh_count = mesh_count+add_size;
    mesh_capacity = chunk_entry->mesh_capacity;

    //if(LAB_UNLIKELY(new_mesh_count+extra_size > mesh_capacity))
    if(new_mesh_count+extra_size > mesh_capacity)
    {
        if(mesh_capacity == 0) mesh_capacity = 1;
        while(new_mesh_count+extra_size > mesh_capacity) mesh_capacity *= 2;
        LAB_Triangle* mesh = LAB_ReallocN(chunk_entry->mesh, mesh_capacity, sizeof *mesh);
        if(!mesh) {
            return NULL;
        }
        chunk_entry->mesh = mesh;
        chunk_entry->mesh_capacity=mesh_capacity;
    }
    chunk_entry->mesh_count=new_mesh_count;

    return &chunk_entry->mesh[mesh_count];
}

#ifndef NO_GLEW
static void LAB_ViewUploadVBO(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    LAB_Triangle* mesh = chunk_entry->mesh;

    if(!chunk_entry->vbo)
    {
        glGenBuffers(1, &chunk_entry->vbo); LAB_GL_DEBUG_ALLOC(1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, chunk_entry->vbo);
    glBufferData(GL_ARRAY_BUFFER, chunk_entry->mesh_count*sizeof *mesh, mesh, GL_DYNAMIC_DRAW);
}
#endif

static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    // TODO enshure light update after at most 1 sec
    if(view->rest_update && ( (chunk_entry->dirty&2) || ((chunk_entry->dirty&1) && (rand()&0x1f)==0) ))
    {
        bool chunk_available = LAB_ViewBuildMesh(view, chunk_entry, view->world);
        if(!chunk_available) return;
        chunk_entry->dirty = 0;
        chunk_entry->exist = 1;
        view->rest_update--;

        #ifndef NO_GLEW
        if(view->flags & LAB_VIEW_USE_VBO)
            LAB_ViewUploadVBO(view, chunk_entry);
        #endif
    }
    else
    {
        #ifndef NO_GLEW
        if(view->flags & LAB_VIEW_USE_VBO)
        {
            if(!chunk_entry->vbo) return;
            glBindBuffer(GL_ARRAY_BUFFER, chunk_entry->vbo);
        }
        #endif
    }

    if(chunk_entry->mesh_count == 0) return;

    glPushMatrix();
    glTranslatef(LAB_CHUNK_SIZE*chunk_entry->x, LAB_CHUNK_SIZE*chunk_entry->y, LAB_CHUNK_SIZE*chunk_entry->z);
    glScalef(1.00001, 1.00001, 1.00001); // Reduces gaps/lines between chunks
    //glScalef(0.9990, 0.9990, 0.9990);

    LAB_Triangle* mesh;

    mesh = view->flags & LAB_VIEW_USE_VBO ? 0 /* Origin of vbo is at 0 */ : chunk_entry->mesh;

    glVertexPointer(3, LAB_GL_TYPEOF(mesh->v[0].x), sizeof *mesh->v, &mesh->v[0].x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof *mesh->v, &mesh->v[0].color);
    glTexCoordPointer(2, LAB_GL_TYPEOF(mesh->v[0].u), sizeof *mesh->v, &mesh->v[0].u);

    glDrawArrays(GL_TRIANGLES, 0, 3*chunk_entry->mesh_count);
    glPopMatrix();


    #ifndef NO_GLEW
    if(view->flags & LAB_VIEW_USE_VBO)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    #endif
}

#if !LAB_VIEW_QUERY_IMMEDIATELY
static void LAB_View_FetchQueryChunks(LAB_View* view)
{
    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        LAB_ViewChunkEntry* entry = &view->chunks[i];

        if(entry->query_id)
            LAB_View_FetchQueryChunk(view, entry);
    }
}
#endif

static void LAB_View_FetchQueryChunk(LAB_View* view, LAB_ViewChunkEntry* entry)
{
    unsigned visible = 1;
    glGetQueryObjectuiv(entry->query_id, GL_QUERY_RESULT, &visible);
    entry->visible = visible;

    glDeleteQueries(1, &entry->query_id); LAB_GL_DEBUG_FREE(1);
    entry->query_id = 0;
}



static void LAB_View_OrderQueryChunks(LAB_View* view)
{
    int px = LAB_Sar((int)floorf(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar((int)floorf(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar((int)floorf(view->z), LAB_CHUNK_SHIFT);


    int dist_sq = view->render_dist*view->render_dist + 3;
    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        LAB_ViewChunkEntry* entry = &view->chunks[i];
        int cx, cy, cz;
        cx = entry->x;
        cy = entry->y;
        cz = entry->z;
        int c_dist_sq = (cx-px)*(cx-px) + (cy-py)*(cy-py) + (cz-pz)*(cz-pz);


        if(c_dist_sq <= 1*1+3)
            entry->visible = 1;
        else if(c_dist_sq <= dist_sq)
        {
            #if 0
            if(entry->do_query || LAB_View_HasChunkVisibleNeighbors(view, cx, cy, cz))
            {
                entry->do_query = 0;
                LAB_View_QueryChunk(view, entry);
            }
            #else
            if(!LAB_View_HasChunkVisibleNeighbors(view, cx, cy, cz))
            {
                entry->do_query = 0;
                entry->visible = 0;
            }
            else
            {
                int probability_update = 255/c_dist_sq;
                //int probability_update = 200/c_dist_sq+55;
                //int probability_update = 150/c_dist_sq+105;
                //int probability_update = 16;
                //int probability_update = 255;

                int r = rand()&0xff;
                if((entry->do_query && r <= 255) || r <= probability_update)
                {
                    entry->do_query = 0;
                    LAB_View_OrderQueryChunk(view, entry);
                }
            }
            #endif
        }
    }
}

static void LAB_View_OrderQueryChunk(LAB_View* view, LAB_ViewChunkEntry* entry)
{
    // TODO fixed sized buffer of queries, glGetQueryObject called
    //      in the next frame, in LAB_View_Tick
    glPushMatrix();
    glColorMask(0, 0, 0, 0);
    glDepthMask(0);

    #if LAB_VIEW_QUERY_IMMEDIATELY
    static unsigned query_id = 0;
    if(query_id == 0)
    {
        glGenQueries(1, &query_id); LAB_GL_DEBUG_ALLOC(1);
    }
    #else
    glGenQueries(1, &entry->query_id); LAB_GL_DEBUG_ALLOC(1);
    unsigned query_id = entry->query_id;
    #endif
    glBeginQuery(GL_ANY_SAMPLES_PASSED, query_id);

    glTranslatef(16*entry->x, 16*entry->y, 16*entry->z);
    glScalef(16, 16, 16);
    static float box[6*2*3][3] = {
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
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, box);
    glDrawArrays(GL_TRIANGLES, 0, 6*2*3);

    glEndQuery(GL_ANY_SAMPLES_PASSED);

    glDepthMask(1);
    glColorMask(1, 1, 1, 1);
    glPopMatrix();

    #if LAB_VIEW_QUERY_IMMEDIATELY
    unsigned visible = 1;
    glGetQueryObjectuiv(query_id, GL_QUERY_RESULT, &visible);
    entry->visible = visible;
    #endif
}

















static void LAB_RenderBlockSelection(int x, int y, int z)
{
#define O (-0.001f)
#define I ( 1.001f)
//#define O 0
//#define I 1
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

    glPushMatrix();
    glTranslatef(x, y, z);
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


static void LAB_View_RenderBlockSelection(LAB_View* view)
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
        if(memcmp(target, prev, sizeof target) != 0)
            LAB_RenderBlockSelection(target[0], target[1], target[2]);
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
        TTF_Font* font = LAB_GuiMonospaceFont();
        if(!font) return;

        int rerender = 0;
        int px, py, pz;
        px = (int)floor(view->x);
        py = (int)floor(view->y);
        pz = (int)floor(view->z);
        if(view->info.surf == NULL)
        {
            //view->info.surf = SDL_CreateRGBSurface(0, INFO_WIDTH, INFO_HEIGHT, 32, 0, 0, 0, 0);
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
            if(view->info.surf != NULL) SDL_FreeSurface(view->info.surf);

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

            view->info.surf = TTF_RenderUTF8_Shaded(font, buf, fg, bg);
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

void LAB_ViewRenderProc(void* user, LAB_Window* window)
{
    LAB_View* view = (LAB_View*)user;

    // Block rendering settings
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // Sky color
    if(view->y <= -16*4)
        glClearColor(0.03, 0.03, 0.03, 1);
    else
        glClearColor(0.5, 0.7, 0.9, 1);

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
    glFrustum(-fov*nearp*ratio, fov*nearp*ratio, -fov*nearp, fov*nearp, nearp, 1000);

    // Setup world matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(view->ax, 1, 0, 0);
    glRotatef(view->ay, 0, 1, 0);
    glRotatef(view->az, 0, 0, 1);
    glTranslatef(-view->x, -view->y, -view->z);

    // Block rendering settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, LAB_block_terrain_gl_id);


    glEnableClientState(GL_VERTEX_ARRAY); // TODO once
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    LAB_ViewRenderChunks(view);
    LAB_View_OrderQueryChunks(view);

    // TODO: remove this
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
        LAB_FpsGraph_Render(&view->fps_graph);
        glPopMatrix();
    }

    LAB_GL_CHECK();
}


void LAB_ViewRemoveDistantChunks(LAB_View* view)
{
    int px = LAB_Sar((int)floorf(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar((int)floorf(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar((int)floorf(view->z), LAB_CHUNK_SHIFT);

    int dist_sq = view->keep_dist*view->keep_dist + 3;
    size_t a = 0;
    for(size_t i = 0; i < view->chunk_count; ++i)
    {

        int cx, cy, cz;
        cx = view->chunks[i].x;
        cy = view->chunks[i].y;
        cz = view->chunks[i].z;
        if((cx-px)*(cx-px) + (cy-py)*(cy-py) + (cz-pz)*(cz-pz) <= dist_sq)
        {
            if(a != i)
            {
                memcpy(&view->chunks[a], &view->chunks[i], sizeof *view->chunks);
            }
            ++a;
        }
        else
        {
            LAB_ViewDestructChunk(view, &view->chunks[i]);
        }
    }
    view->chunk_count = a;
}



void LAB_ViewRenderChunks(LAB_View* view)
{
    view->rest_update = view->max_update;

    int px = LAB_Sar((int)floorf(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar((int)floorf(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar((int)floorf(view->z), LAB_CHUNK_SHIFT);


    int dist_sq = view->render_dist*view->render_dist + 3;
    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        int cx, cy, cz;
        cx = view->chunks[i].x;
        cy = view->chunks[i].y;
        cz = view->chunks[i].z;
        if((cx-px)*(cx-px) + (cy-py)*(cy-py) + (cz-pz)*(cz-pz) <= dist_sq
           && view->chunks[i].visible)
            LAB_ViewRenderChunk(view, &view->chunks[i]);
    }
}





LAB_ViewChunkEntry* LAB_ViewFindChunkEntry(LAB_View* view, int x, int y, int z)
{
    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        if(view->chunks[i].x == x && view->chunks[i].y == y && view->chunks[i].z == z)
            return &view->chunks[i];
    }
    return NULL;
}

/**
 *  Assume: x,y,z is not contained in view->chunks
 */
LAB_ViewChunkEntry* LAB_ViewNewChunkEntry(LAB_View* view, int x, int y, int z)
{
    LAB_ViewChunkEntry* chunks;
    size_t chunk_count, chunk_capacity;

    chunks = view->chunks;
    chunk_count = view->chunk_count;
    chunk_capacity = view->chunk_capacity;

    if(LAB_UNLIKELY(view->chunk_count >= chunk_capacity))
    {
        if(chunk_capacity == 0)
            chunk_capacity = 16;
        else
            chunk_capacity *= 2;

        chunks = LAB_ReallocN(chunks, chunk_capacity, sizeof *chunks);
        if(!chunks) return NULL;
        view->chunk_capacity = chunk_capacity;
        view->chunks = chunks;
    }

    memset(&chunks[chunk_count], 0, sizeof *chunks);
    chunks[chunk_count].x = x;
    chunks[chunk_count].y = y;
    chunks[chunk_count].z = z;
    //chunks[chunk_count].visible = 1; // <-- TODO: remove from here
    chunks[chunk_count].do_query = 1;
    view->chunk_count++;

    return &chunks[chunk_count];
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
    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        view->chunks[i].dirty = ~0;
    }
    if(free_buffers)
    {
        for(size_t i = 0; i < view->chunk_count; ++i)
        {
            if(view->chunks[i].vbo)
            {
                glDeleteBuffers(1, &view->chunks[i].vbo); LAB_GL_DEBUG_FREE(1);
                view->chunks[i].vbo = 0;
            }
        }
    }
}



void LAB_ViewGetDirection(LAB_View* view, LAB_OUT float dir[3])
{
    float ax, ay;
    float sax, cax, say, cay;

    ax = view->ax*LAB_PI/180.f;
    ay = view->ay*LAB_PI/180.f;

    sax = sin(ax);
    cax = cos(ax);
    say = sin(ay);
    cay = cos(ay);

    dir[0] = cax*say;
    dir[1] = -sax;
    dir[2] = -cax*cay;
}




void LAB_ViewTick(LAB_View* view, uint32_t delta_ms)
{
    LAB_ViewRemoveDistantChunks(view);
    LAB_ViewLoadNearChunks(view);
    #if !LAB_VIEW_QUERY_IMMEDIATELY
    LAB_View_FetchQueryChunks(view);
    #endif
    LAB_FpsGraph_AddSample(&view->fps_graph, delta_ms);
    LAB_GuiManager_Tick(&view->gui_mgr);
}


static bool LAB_View_HasChunkVisibleNeighbors(LAB_View* view, int x, int y, int z)
{
    for(int i = 0; i < 6; ++i)
    {
        const int* off = LAB_offset[i];
        int xx = x + off[0];
        int yy = y + off[1];
        int zz = z + off[2];
        LAB_ViewChunkEntry* entry = LAB_ViewFindChunkEntry(view, xx, yy, zz);
        if(entry && entry->visible) return 1;
    }
    return 0;
}

void LAB_ViewLoadNearChunks(LAB_View* view)
{
    // TODO: check if gen-queue is full: quit

    int px = LAB_Sar((int)floorf(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar((int)floorf(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar((int)floorf(view->z), LAB_CHUNK_SHIFT);

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
                    }
                    for(int q = 0; q < 8; ++q)
                    {
                        // ~x == -x-1
                        int xx, yy, zz;
                        if(q&1 && x==0) continue; else xx = px+(q&1 ? -x : x);
                        if(q&2 && y==0) continue; else yy = py+(q&2 ? -y : y);
                        if(q&4 && z==0) continue; else zz = pz+(q&4 ? -z : z);

                        LAB_ViewChunkEntry* entry = LAB_ViewFindChunkEntry(view, xx, yy, zz);
                        if(entry == NULL)
                        {
                            if(r > 1 && !LAB_View_HasChunkVisibleNeighbors(view, xx, yy, zz))
                                continue;
                            entry = LAB_ViewNewChunkEntry(view, xx, yy, zz);
                            if(entry == NULL) return; // NO MEMORY
                            entry->dirty = ~0;
                        /*}
                        if(!entry->exist)
                        {*/
                            (void)LAB_GetChunk(view->world, xx, yy, zz, LAB_CHUNK_GENERATE_LATER);
                            --load_amount;
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
    if(chunk_entry->mesh)
        LAB_Free(chunk_entry->mesh);
    #ifndef NO_GLEW
    if(chunk_entry->vbo)
    {
        glDeleteBuffers(1, &chunk_entry->vbo); LAB_GL_DEBUG_FREE(1);
    }
    #endif
    #if !LAB_VIEW_QUERY_IMMEDIATELY
    if(chunk_entry->query_id)
    {
        glDeleteQueries(1, &chunk_entry->query_id); LAB_GL_DEBUG_FREE(1);
    }
    #endif
}
