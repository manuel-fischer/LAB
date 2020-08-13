#include "LAB_world_view.h"

#include "LAB_memory.h"
#include "LAB_error.h"
#include "LAB_math.h"

#include "LAB_world.h"
#include "LAB_block.h"
#include "LAB_window.h"
#include "LAB_gl.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#include "LAB_perf.h"
#include "LAB_bits.h"
#include "LAB_opt.h"
#include "LAB_util.h"


/*#define HTL_PARAM LAB_VIEW_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM*/

///############################

static int  LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world);
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_IN LAB_Chunk* chunk_neighborhood[27]);
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_IN LAB_Chunk* chunk_neighborhood[27], int x, int y, int z);
static void LAB_ViewRenderChunks(LAB_View* view);
static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
static void LAB_ViewRemoveDistantChunks(LAB_View* view);
static void LAB_ViewDestructChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);

static LAB_ViewTriangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size, size_t extra_size);

GLuint LAB_gltextureid = 0;
void LAB_View_StaticInit(void)
{
    static char init = 0;
    if(init) return;
    init = 1;

    SDL_Surface* img = IMG_Load("assets/terrain.png");
    if(LAB_UNLIKELY(img == NULL)) return;

    if(img->format->format != SDL_PIXELFORMAT_RGBA32)
    {
        SDL_Surface* nImg;
        nImg = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(img);
        img = nImg;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &LAB_gltextureid);
    glBindTexture(GL_TEXTURE_2D, LAB_gltextureid);

    #ifndef NO_GLEW
    glEnable(GL_MIPMAP);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    #else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    #endif

    glMatrixMode(GL_TEXTURE);
    glScalef(32.f / (float)img->w, 32.f / (float)img->h, 1);

    SDL_FreeSurface(img);
}

int  LAB_ConstructView(LAB_View* view, LAB_World* world)
{
    LAB_View_StaticInit();

    memset(view, 0, sizeof *view);
    view->world = world;

    view->ax = 22;
    view->y = 1.5;

    return 1;
}

void LAB_DestructView(LAB_View* view)
{
    for(int i = 0; i < view->chunk_count; ++i)
    {
        LAB_ViewDestructChunk(view, &view->chunks[i]);
    }
    LAB_Free(view->chunks);
}


void LAB_ViewChunkProc(void* user, LAB_World* world, int x, int y, int z)
{
    LAB_View* view = (LAB_View*)user;

    for(int iz = -1; iz <= 1; ++iz)
    for(int iy = -1; iy <= 1; ++iy)
    for(int ix = -1; ix <= 1; ++ix)
    {
        LAB_ViewChunkEntry* entry = LAB_ViewGetChunkEntry(view, x+ix, y+iy, z+iz);
        entry->dirty = 1;
    }

}



/**
 *  Return 1 if the chunk was available
 */
static int LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world)
{
    LAB_Chunk* chunk_neighborhood[27];

    LAB_GetChunkNeighborhood(world, chunk_neighborhood,
                             chunk_entry->x, chunk_entry->y, chunk_entry->z,
                             LAB_CHUNK_EXISTING);

    if(chunk_neighborhood[1+3+9] == NULL) return 0;
#if 0
    if(chunk_entry->x == 0 && chunk_entry->y == -1 && chunk_entry->z == 0)
    {
        LAB_PERF_BEGIN("build mesh", 333);
            LAB_ViewBuildMeshNeighbored(view, chunk_entry, chunk_neighborhood);
        LAB_PERF_END("build mesh");
    }
    else
    {
        //LAB_ViewBuildMeshNeighbored(view, chunk_entry, chunk_neighborhood);
    }
#else
    LAB_ViewBuildMeshNeighbored(view, chunk_entry, chunk_neighborhood);
#endif
    return 1;
}
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27])
{
    const int X = 1;
    const int Y = 3*1;
    const int Z = 3*3*1;

    chunk_entry->mesh_count = 0;

    for(size_t z = 0; z < LAB_CHUNK_SIZE; ++z)
    {
        for(size_t y = 0; y < LAB_CHUNK_SIZE; ++y)
        {
            for(size_t x = 0; x < LAB_CHUNK_SIZE; ++x)
            {
                if(cnk3x3x3[X+Y+Z]->blocks[LAB_CHUNK_OFFSET(x, y, z)]->flags & LAB_BLOCK_SOLID)
                {
                    LAB_ViewBuildMeshBlock(view, chunk_entry, cnk3x3x3, x, y, z);
                }
            }
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


#define LAB_SetVertex(vert,  xx, yy, zz,  rr, gg, bb, aa,  uu, vv,   cr, cg, cb,  tx, ty) do \
{                                                                      \
    (vert)->x = xx; (vert)->y = yy; (vert)->z = zz;                    \
    (vert)->r = ((int)cr)*(rr*256/255)/256;                            \
    (vert)->g = ((int)cg)*(gg*256/255)/256;                            \
    (vert)->b = ((int)cb)*(bb*256/255)/256;                            \
    (vert)->a = aa;                                                    \
    (vert)->u = uu + tx;                                               \
    (vert)->v = vv + ty;                                               \
} while(0)

#define LAB_SetQuad(tri, x0, y0, z0,  r0, g0, b0, a0,  u0, v0, \
                         x1, y1, z1,  r1, g1, b1, a1,  u1, v1, \
                         x2, y2, z2,  r2, g2, b2, a2,  u2, v2, \
                         x3, y3, z3,  r3, g3, b3, a3,  u3, v3) do \
{ \
    LAB_ViewVertex* restrict t0 = tri[0].v;                    \
    LAB_ViewVertex* restrict t1 = tri[1].v;                    \
    LAB_SetVertex(&t0[1],  x0, y0, z0,  r0, g0, b0, a0,  u0, v0,  cr, cg, cb, tx, ty); \
    LAB_SetVertex(&t0[2],  x1, y1, z1,  r1, g1, b1, a1,  u1, v1,  cr, cg, cb, tx, ty); \
    LAB_SetVertex(&t1[2],  x1, y1, z1,  r1, g1, b1, a1,  u1, v1,  cr, cg, cb, tx, ty); \
    LAB_SetVertex(&t0[0],  x2, y2, z2,  r2, g2, b2, a2,  u2, v2,  cr, cg, cb, tx, ty); \
    LAB_SetVertex(&t1[1],  x2, y2, z2,  r2, g2, b2, a2,  u2, v2,  cr, cg, cb, tx, ty); \
    LAB_SetVertex(&t1[0],  x3, y3, z3,  r3, g3, b3, a3,  u3, v3,  cr, cg, cb, tx, ty); \
} while(0)



LAB_HOT
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27], int x, int y, int z)
{

#define BRANCHLESS  0
#define BITFIELD    1
#define PRECOMP_PTR 0

#define GET_BLOCK(bx, by, bz) LAB_GetNeighborhoodBlock(cnk3x3x3, x+(bx), y+(by), z+(bz))
#define GET_BLOCK_FLAGS(bx, by, bz) (GET_BLOCK(bx, by, bz)->flags)

    LAB_Block* block = cnk3x3x3[1+3+9]->blocks[LAB_CHUNK_OFFSET(x, y, z)];

    int cr = block->r;
    int cg = block->g;
    int cb = block->b;

    int tx = block->tx;
    int ty = block->ty;

    int faces = 0;
    faces |=  1*(!(GET_BLOCK_FLAGS(-1, 0, 0)&LAB_BLOCK_SOLID));
    faces |=  2*(!(GET_BLOCK_FLAGS( 1, 0, 0)&LAB_BLOCK_SOLID));
    faces |=  4*(!(GET_BLOCK_FLAGS( 0,-1, 0)&LAB_BLOCK_SOLID));
    faces |=  8*(!(GET_BLOCK_FLAGS( 0, 1, 0)&LAB_BLOCK_SOLID));
    faces |= 16*(!(GET_BLOCK_FLAGS( 0, 0,-1)&LAB_BLOCK_SOLID));
    faces |= 32*(!(GET_BLOCK_FLAGS( 0, 0, 1)&LAB_BLOCK_SOLID));

    if(faces == 0) return;

    int face_count = LAB_PopCnt(faces);

    LAB_ViewTriangle* tri;
    tri = LAB_ViewMeshAlloc(chunk_entry, 2*face_count, !!BRANCHLESS);
    if(LAB_UNLIKELY(tri == NULL)) return;


    // shading
    #define SHx 235u // west east
    #define SHy 192u // bottom
    #define SHz 215u // north south

    typedef struct {
        uint8_t x, y, z, l;
    } DataSelect;


    // Vertex order: Z
    static const DataSelect data_select[6][4] = {
        // WEST
        { {0, 1, 0,  SHx},
          {0, 1, 1,  SHx},
          {0, 0, 0,  SHx},
          {0, 0, 1,  SHx} },

        // EAST
        { {1, 1, 1,  SHx},
          {1, 1, 0,  SHx},
          {1, 0, 1,  SHx},
          {1, 0, 0,  SHx} },


        // BOTTOM
        { {1, 0, 0,  SHy},
          {0, 0, 0,  SHy},
          {1, 0, 1,  SHy},
          {0, 0, 1,  SHy} },

        // TOP
        { {0, 1, 0,  255u},
          {1, 1, 0,  255u},
          {0, 1, 1,  255u},
          {1, 1, 1,  255u} },


        // NORTH
        { {1, 1, 0,  SHz},
          {0, 1, 0,  SHz},
          {1, 0, 0,  SHz},
          {0, 0, 0,  SHz} },

        // SOUTH
        { {0, 1, 1,  SHz},
          {1, 1, 1,  SHz},
          {0, 0, 1,  SHz},
          {1, 0, 1,  SHz} },
    };



    int i, face_itr;
    face_itr = faces;
    do {
        i = LAB_Ctz(face_itr);

        // remove last bit
        face_itr &= face_itr-1;

        DataSelect const* ds = data_select[i];

        typedef long long ll;
        uint8_t l = (SHx | SHx << 8 | SHy << 16 | 255u << 24 | (ll)SHz << 32 | (ll)SHz << 40)>>(i<<3);

        const int* o = LAB_offset[i];

        LAB_Color lum = LAB_GetNeighborhoodLight(cnk3x3x3, x+o[0], y+o[1], z+o[2], LAB_RGB(255, 255, 255));

        int r, g, b;
        r = (l*LAB_RED(lum)) >> 8;
        g = (l*LAB_GRN(lum)) >> 8;
        b = (l*LAB_BLU(lum)) >> 8;


        LAB_SetQuad(tri, x+ds[0].x, y+ds[0].y, z+ds[0].z,  r, g, b, 255,  0, 0,
                         x+ds[1].x, y+ds[1].y, z+ds[1].z,  r, g, b, 255,  1, 0,
                         x+ds[2].x, y+ds[2].y, z+ds[2].z,  r, g, b, 255,  0, 1,
                         x+ds[3].x, y+ds[3].y, z+ds[3].z,  r, g, b, 255,  1, 1);

        tri+=2;
    } while(face_itr);
}


static LAB_ViewTriangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size, size_t extra_size)
{
    size_t mesh_count, new_mesh_count, mesh_capacity;

    mesh_count = chunk_entry->mesh_count;
    new_mesh_count = mesh_count+add_size;
    mesh_capacity = chunk_entry->mesh_capacity;

    if(LAB_UNLIKELY(new_mesh_count+extra_size > mesh_capacity))
    {
        if(mesh_capacity == 0) mesh_capacity = 1;
        while(new_mesh_count+extra_size > mesh_capacity) mesh_capacity *= 2;
        LAB_ViewTriangle* mesh = LAB_ReallocN(chunk_entry->mesh, mesh_capacity, sizeof *mesh);
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
    LAB_ViewTriangle* mesh = chunk_entry->mesh;

    if(!chunk_entry->vbo) glGenBuffers(1, &chunk_entry->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, chunk_entry->vbo);
    glBufferData(GL_ARRAY_BUFFER, chunk_entry->mesh_count*sizeof *mesh, mesh, GL_DYNAMIC_DRAW);
}
#endif

static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    if(chunk_entry->dirty)
    {
        int chunk_available;
        chunk_available = LAB_ViewBuildMesh(view, chunk_entry, view->world);
        chunk_entry->dirty = !chunk_available;

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
            glBindBuffer(GL_ARRAY_BUFFER, chunk_entry->vbo);
        }
        #endif
    }

    if(chunk_entry->mesh_count == 0) return;

    glPushMatrix();
    glTranslatef(LAB_CHUNK_SIZE*chunk_entry->x, LAB_CHUNK_SIZE*chunk_entry->y, LAB_CHUNK_SIZE*chunk_entry->z);


    LAB_ViewTriangle* mesh;

    mesh = view->flags & LAB_VIEW_USE_VBO ? 0 /* Origin of vbo is at 0 */ : chunk_entry->mesh;

    glVertexPointer(3, GL_FLOAT, sizeof *mesh->v, &mesh->v[0].x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof *mesh->v, &mesh->v[0].r);
    glTexCoordPointer(2, GL_FLOAT, sizeof *mesh->v, &mesh->v[0].u);

    glDrawArrays(GL_TRIANGLES, 0, 3*chunk_entry->mesh_count);
    glPopMatrix();


    #ifndef NO_GLEW
    if(view->flags & LAB_VIEW_USE_VBO)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    #endif
}


void LAB_ViewRenderGui(LAB_View* view)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_DST_COLOR);

    static const float crosshair[3*3*4] = {
            0, -0.1, -5,
         0.05, -0.2, -5,
        -0.05, -0.2, -5,

         0.1,     0, -5,
         0.2,  0.05, -5,
         0.2, -0.05, -5,

            0,  0.1, -5,
        -0.05,  0.2, -5,
         0.05,  0.2, -5,

        -0.1,     0, -5,
        -0.2, -0.05, -5,
        -0.2,  0.05, -5,
    };

    glColor3f(1,1,1);
    glVertexPointer(3, GL_FLOAT, 0, crosshair);
    glDrawArrays(GL_TRIANGLES, 0, 3*4);
}

void LAB_ViewRenderProc(void* user, LAB_Window* window)
{
    LAB_View* view = (LAB_View*)user;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    if(view->y <= -16*4)
        glClearColor(0.03, 0.03, 0.03, 1);
    else
        glClearColor(0.5, 0.7, 0.9, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int w, h;
    SDL_GetWindowSize(window->window, &w, &h);
    float ratio = h?(float)w/(float)h:1;
    float nearp = 0.1f;
    float fov = 1;
    glFrustum(-fov*nearp*ratio, fov*nearp*ratio, -fov*nearp, fov*nearp, nearp, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(view->ax, 1, 0, 0);
    glRotatef(view->ay, 0, 1, 0);
    glRotatef(view->az, 0, 0, 1);
    glTranslatef(-view->x, -view->y, -view->z);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, LAB_gltextureid);


    glEnableClientState(GL_VERTEX_ARRAY); // TODO once
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    LAB_ViewRenderChunks(view);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);


    LAB_ViewRemoveDistantChunks(view);


    // Render Crosshair
    if(view->flags & LAB_VIEW_SHOW_GUI)
        LAB_ViewRenderGui(view);
}


void LAB_ViewRemoveDistantChunks(LAB_View* view)
{
    int px = LAB_Sar((int)floorf(view->x), LAB_CHUNK_SHIFT);
    int py = LAB_Sar((int)floorf(view->y), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar((int)floorf(view->z), LAB_CHUNK_SHIFT);

    int dist_sq = view->keep_dist*view->keep_dist + 3;
    int a = 0;
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
        if((cx-px)*(cx-px) + (cy-py)*(cy-py) + (cz-pz)*(cz-pz) <= dist_sq)
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
    view->chunk_count++;

    return &chunks[chunk_count];
}


LAB_ViewChunkEntry* LAB_ViewGetChunkEntry(LAB_View* view, int x, int y, int z)
{
    LAB_ViewChunkEntry* chunk_entry;
    chunk_entry = LAB_ViewFindChunkEntry(view, x, y, z);
    if(chunk_entry != NULL) return chunk_entry;
    return LAB_ViewNewChunkEntry(view, x, y, z);

}

void LAB_ViewInvalidateEverything(LAB_View* view)
{
    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        view->chunks[i].dirty = 1;
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




void LAB_ViewTick(LAB_View* view)
{
    LAB_ViewLoadNearChunks(view);
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

    for(int r = 0; r <= view->preload_dist; ++r)
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

                for(int i = 0; i < 3; ++i)
                {
                    int x, y, z;
                    switch(i)
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
                            entry = LAB_ViewNewChunkEntry(view, xx, yy, zz);
                            if(entry == NULL) return; // NO MEMORY
                            entry->dirty = 1;
                        }
                        if(entry->dirty)
                            (void)LAB_GetChunk(view->world, xx, yy, zz, LAB_CHUNK_GENERATE_LATER);

                        //(void)LAB_GetChunk(view->world, -xx-1, yy, zz, LAB_CHUNK_GENERATE_LATER);
                    }
                    if(a==r) break /*i*/;
                    if(b==r && i==1) break /*i*/;
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
        glDeleteBuffers(1, &chunk_entry->vbo);
    #endif
}
