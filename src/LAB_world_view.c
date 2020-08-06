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


#define HTL_PARAM LAB_VIEW_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM

///############################

#define BLOCK_NEIGHBORHOOD 0

static void LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world);
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* chunk_neighborhood[27]);
#if BLOCK_NEIGHBORHOOD
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Block* block_neighborhood[27], int x, int y, int z);
static void LAB_GetBlockNeighborhood(LAB_Chunk* chunk_neighborhood[27], LAB_Block* block_neighborhood[27], int x, int y, int z);
#else
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* chunk_neighborhood[27], int x, int y, int z);
#endif
static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);

static LAB_ViewTriangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size, size_t extra_size);

GLuint LAB_gltextureid = 0;
void LAB_View_StaticInit()
{
    static char init = 0;
    if(init) return;
    init = 1;

    SDL_Surface* img = IMG_Load("assets/terrain.png");
    if(LAB_UNLIKELY(img == NULL)) return;

    //if(img->format-> != SDL_PIXELFORMAT_RGBA32)
    {
        SDL_Surface* nImg;
        nImg = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(img);
        img = nImg;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &LAB_gltextureid);
    glBindTexture(GL_TEXTURE_2D, LAB_gltextureid);

    glEnable(GL_MIPMAP);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glMatrixMode(GL_TEXTURE);
    glScalef(32.f / (float)img->w, 32.f / (float)img->h, 1);

    SDL_FreeSurface(img);
}

LAB_View* LAB_CreateView(LAB_World* world)
{
    LAB_View_StaticInit();

    LAB_View* world_view = LAB_Calloc(1, sizeof *world_view);
    world_view->world = world;

    world_view->ax = 22;
    world_view->y = 1.5;

    return world_view;
}

void LAB_DestroyView(LAB_View* world_view)
{
    LAB_Free(world_view);
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



static void LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world)
{
    LAB_Chunk* chunk_neighborhood[27];

    LAB_GetChunkNeighborhood(world, chunk_neighborhood,
                             chunk_entry->x, chunk_entry->y, chunk_entry->z,
                             LAB_CHUNK_EXISTING);

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
    if(chunk_neighborhood[1+3+9] != NULL)
        LAB_ViewBuildMeshNeighbored(view, chunk_entry, chunk_neighborhood);
#endif
}
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27])
{
    const int X = 1;
    const int Y = 3*1;
    const int Z = 3*3*1;

    #if BLOCK_NEIGHBORHOOD
    LAB_Block* blk3x3x3[27];
    #endif

    chunk_entry->mesh_count = 0;

    for(size_t z = 0; z < LAB_CHUNK_SIZE; ++z)
    {
        for(size_t y = 0; y < LAB_CHUNK_SIZE; ++y)
        {
            for(size_t x = 0; x < LAB_CHUNK_SIZE; ++x)
            {

                if(cnk3x3x3[X+Y+Z]->blocks[LAB_CHUNK_OFFSET(x, y, z)]->flags & LAB_BLOCK_SOLID)
                {
                    #if BLOCK_NEIGHBORHOOD
                    LAB_GetBlockNeighborhood(cnk3x3x3, blk3x3x3, x, y, z);
                    LAB_ViewBuildMeshBlock(view, chunk_entry, blk3x3x3, x, y, z);
                    #else
                    LAB_ViewBuildMeshBlock(view, chunk_entry, cnk3x3x3, x, y, z);
                    #endif
                }
            }
        }
    }
}


//static void* void_mem[16] = {};

LAB_HOT LAB_INLINE
LAB_BlockFlags LAB_GetNeighborhoodBlockFlags(LAB_Chunk* neighborhood[27], int x, int y, int z)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
#if 0 // branchless
    LAB_BlockFlags flags = 0;

    LAB_Block** blockaddr = (void*)void_mem;
    LAB_Block* block = (void*)void_mem;

    (void)LAB_UNLIKELY(chunk == NULL);

    LAB_CMOV(chunk, blockaddr, &chunk->blocks[block_index]);
    LAB_CMOV_FROM(chunk, block, blockaddr);
    LAB_CMOV_FROM(chunk, flags, &block->flags);

    return flags;
#else
    if(LAB_UNLIKELY(chunk == NULL)) return 0;
    return chunk->blocks[block_index]->flags;
#endif
}


#if 1
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
#else
LAB_NOINLINE LAB_HOT
static void LAB_SetVertex(LAB_ViewVertex* vert,  int xx, int yy, int zz,  int rr, int gg, int bb, int aa,  int uu, int vv,   int cr, int cg, int cb,  int tx, int ty)
{
    vert->x = xx; vert->y = yy; vert->z = zz;
    vert->r = ((int)cr)*(rr*256/255)/256;
    vert->g = ((int)cg)*(gg*256/255)/256;
    vert->b = ((int)cb)*(bb*256/255)/256;
    vert->a = aa;
    vert->u = uu + tx;
    vert->v = vv + ty;
}
#endif

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
#if BLOCK_NEIGHBORHOOD
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Block* blk3x3x3[27], int x, int y, int z)
#else
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27], int x, int y, int z)
#endif
{

#define BRANCHLESS  0
#define BITFIELD    1
#define PRECOMP_PTR 0

#if BLOCK_NEIGHBORHOOD
    const int X = 1;
    const int Y = 3*1;
    const int Z = 3*3*1;
#define GET_BLOCK(bx, by, bz) blk3x3x3[X*((bx)+1) + Y*((by)+1) + Z*((bz)+1)]
#else
#define GET_BLOCK(bx, by, bz) LAB_GetNeighborhoodBlock(cnk3x3x3, x+(bx), y+(by), z+(bz))
//#define GET_BLOCK_FLAGS(bx, by, bz) LAB_GetNeighborhoodBlockFlags(cnk3x3x3, x+(bx), y+(by), z+(bz))
#endif
#ifndef GET_BLOCK_FLAGS
#define GET_BLOCK_FLAGS(bx, by, bz) (GET_BLOCK(bx, by, bz)->flags)
#endif

#if BLOCK_NEIGHBORHOOD
    LAB_Block* block = GET_BLOCK( 0, 0, 0);
#else
    LAB_Block* block = cnk3x3x3[1+3+9]->blocks[LAB_CHUNK_OFFSET(x, y, z)];
#endif

    int cr = block->r;
    int cg = block->g;
    int cb = block->b;

    int tx = block->tx;
    int ty = block->ty;

    #if BITFIELD
    int faces = 0;
    #if 0
    faces |=  1*(!(GET_BLOCK_FLAGS(-1, 0, 0)&LAB_BLOCK_SOLID));
    faces |=  2*(!(GET_BLOCK_FLAGS( 1, 0, 0)&LAB_BLOCK_SOLID));
    faces |=  4*(!(GET_BLOCK_FLAGS( 0,-1, 0)&LAB_BLOCK_SOLID));
    faces |=  8*(!(GET_BLOCK_FLAGS( 0, 1, 0)&LAB_BLOCK_SOLID));
    faces |= 16*(!(GET_BLOCK_FLAGS( 0, 0,-1)&LAB_BLOCK_SOLID));
    faces |= 32*(!(GET_BLOCK_FLAGS( 0, 0, 1)&LAB_BLOCK_SOLID));
    #else
    faces = (
                1*(!(GET_BLOCK_FLAGS(-1, 0, 0)&LAB_BLOCK_SOLID))
              | 2*(!(GET_BLOCK_FLAGS( 1, 0, 0)&LAB_BLOCK_SOLID))
            )
          | (
                4*(!(GET_BLOCK_FLAGS( 0,-1, 0)&LAB_BLOCK_SOLID))
              | 8*(!(GET_BLOCK_FLAGS( 0, 1, 0)&LAB_BLOCK_SOLID))
            )
          |
            (
               16*(!(GET_BLOCK_FLAGS( 0, 0,-1)&LAB_BLOCK_SOLID))
             | 32*(!(GET_BLOCK_FLAGS( 0, 0, 1)&LAB_BLOCK_SOLID))
            );
    #endif

    if(faces == 0) return;

    int face_count = LAB_PopCnt(faces);

    #define f0 (!!(faces&1))
    #define f1 (!!(faces&2))
    #define f2 (!!(faces&4))
    #define f3 (!!(faces&8))
    #define f4 (!!(faces&16))
    #define f5 (!!(faces&32))
    #else

    int f0, f1, f2, f3, f4, f5;
    f0 = !(GET_BLOCK_FLAGS(-1, 0, 0)&LAB_BLOCK_SOLID);
    f1 = !(GET_BLOCK_FLAGS( 1, 0, 0)&LAB_BLOCK_SOLID);
    f2 = !(GET_BLOCK_FLAGS( 0,-1, 0)&LAB_BLOCK_SOLID);
    f3 = !(GET_BLOCK_FLAGS( 0, 1, 0)&LAB_BLOCK_SOLID);
    f4 = !(GET_BLOCK_FLAGS( 0, 0,-1)&LAB_BLOCK_SOLID);
    f5 = !(GET_BLOCK_FLAGS( 0, 0, 1)&LAB_BLOCK_SOLID);

    // Shorter dependency chain by adding up boolean values
    // in parallel
    int face_count = ((f0+f1) + (f2+f3)) + (f4+f5);

    if(face_count == 0) return;
    #endif

    LAB_ViewTriangle* tri;
    #if 0
    typedef LAB_ViewTriangle*(*funcptrT)(LAB_ViewChunkEntry*, size_t, size_t);
    static volatile funcptrT ptr = LAB_ViewMeshAlloc;
    tri = ptr(chunk_entry, 2*face_count);
    #else
    tri = LAB_ViewMeshAlloc(chunk_entry, 2*face_count, !!BRANCHLESS);
    #endif
    if(LAB_UNLIKELY(tri == NULL)) return;


    // shading
    #define SHx 235u // west east
    #define SHy 192u // bottom
    #define SHz 215u // north south

#if 1
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
        { {0, 1, 0,  255},
          {1, 1, 0,  255},
          {0, 1, 1,  255},
          {1, 1, 1,  255} },


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

        #if 0
        uint8_t l = ds[0].l;
        #else
        typedef long long ll;
        uint8_t l = (SHx | SHx << 8 | SHy << 16 | 255u << 24 | (ll)SHz << 32 | (ll)SHz << 40)>>(i<<3);
        #endif

        const int* o = &LAB_offset[i];

        int lum = LAB_GetNeighborhoodLight(cnk3x3x3, x+o[0], y+o[1], z+o[2]);


        l = (l*lum)>>8;


        LAB_SetQuad(tri, x+ds[0].x, y+ds[0].y, z+ds[0].z,  l, l, l, 255,  0, 0,
                         x+ds[1].x, y+ds[1].y, z+ds[1].z,  l, l, l, 255,  1, 0,
                         x+ds[2].x, y+ds[2].y, z+ds[2].z,  l, l, l, 255,  0, 1,
                         x+ds[3].x, y+ds[3].y, z+ds[3].z,  l, l, l, 255,  1, 1);

        tri+=2;
    } while(face_itr);
#elif 0
    typedef struct {
        uint8_t x, y, z;
        uint8_t r, g, b;
        uint8_t u, v;
    } DataSelect;

    static const DataSelect data_select[6][4] = {
        // WEST
        { {0, 0, 0,  SHx, SHx, SHx,  0, 1 },
          {0, 1, 0,  SHx, SHx, SHx,  0, 0 },
          {0, 0, 1,  SHx, SHx, SHx,  1, 1 },
          {0, 1, 1,  SHx, SHx, SHx,  1, 0 } },

        // EAST
        { {1, 1, 0,  SHx, SHx, SHx,  1, 0 },
          {1, 0, 0,  SHx, SHx, SHx,  1, 1 },
          {1, 1, 1,  SHx, SHx, SHx,  0, 0 },
          {1, 0, 1,  SHx, SHx, SHx,  0, 1 } },


        // BOTTOM
        { {0, 0, 0,  SHy, SHy, SHy,  1, 0 },
          {0, 0, 1,  SHy, SHy, SHy,  1, 1 },
          {1, 0, 0,  SHy, SHy, SHy,  0, 0 },
          {1, 0, 1,  SHy, SHy, SHy,  0, 1 } },

        // TOP
        { {0, 1, 1,  255, 255, 255,  1, 1 },
          {0, 1, 0,  255, 255, 255,  1, 0 },
          {1, 1, 1,  255, 255, 255,  0, 1 },
          {1, 1, 0,  255, 255, 255,  0, 0 } },


        // NORTH
        { {0, 0, 0,  SHz, SHz, SHz,  1, 1 },
          {1, 0, 0,  SHz, SHz, SHz,  0, 1 },
          {0, 1, 0,  SHz, SHz, SHz,  1, 0 },
          {1, 1, 0,  SHz, SHz, SHz,  0, 0 } },

        // SOUTH
        { {1, 0, 1,  SHz, SHz, SHz,  1, 1 },
          {0, 0, 1,  SHz, SHz, SHz,  0, 1 },
          {1, 1, 1,  SHz, SHz, SHz,  1, 0 },
          {0, 1, 1,  SHz, SHz, SHz,  0, 0 } },
    };

    for(int face_itr=faces, i = 0; face_itr; face_itr>>=1, i++)
    {
        if(face_itr&1)
        {
            DataSelect* ds = data_select[i];

            /*LAB_SetQuad(tri, x+ds[0].x, y+ds[0].y, z+ds[0].z,  ds[0].r, ds[0].g, ds[0].b, 255,  ds[0].u, ds[0].v,
                             x+ds[1].x, y+ds[1].y, z+ds[1].z,  ds[1].r, ds[1].g, ds[1].b, 255,  ds[1].u, ds[1].v,
                             x+ds[2].x, y+ds[2].y, z+ds[2].z,  ds[2].r, ds[2].g, ds[2].b, 255,  ds[2].u, ds[2].v,
                             x+ds[3].x, y+ds[3].y, z+ds[3].z,  ds[3].r, ds[3].g, ds[3].b, 255,  ds[3].u, ds[3].v);*/

            typedef long long ll;
            uint8_t l = (SHx | SHx << 8 | SHy << 16 | 255u << 24 | (ll)SHz << 32 | (ll)SHz << 40)>>(i<<3);

            LAB_SetQuad(tri, x+ds[0].x, y+ds[0].y, z+ds[0].z,  l, l, l, 255,  ds[0].u, ds[0].v,
                             x+ds[1].x, y+ds[1].y, z+ds[1].z,  l, l, l, 255,  ds[1].u, ds[1].v,
                             x+ds[2].x, y+ds[2].y, z+ds[2].z,  l, l, l, 255,  ds[2].u, ds[2].v,
                             x+ds[3].x, y+ds[3].y, z+ds[3].z,  l, l, l, 255,  ds[3].u, ds[3].v);

            tri+=2;
        }
    }

#else

    float x0 = x,    y0 = y,    z0 = z;
    float x1 = x0+1, y1 = y0+1, z1 = z0+1;





#if BRANCHLESS
    #define IF_DO(cond)
#else
    #define IF_DO(cond) if(cond)
#endif


#if PRECOMP_PTR
    LAB_ViewTriangle* tri0,* tri1,* tri2,* tri3,* tri4,* tri5;
    tri0 = tri;
    tri1 = tri+2*(f0);
    tri2 = tri+2*(f0+f1);
    tri3 = tri+2*(f0+f1+f2);
    tri4 = tri+2*(f0+f1+f2+f3);
    tri5 = tri+2*(f0+f1+f2+f3+f4);


    IF_DO(f0)
    {
        LAB_SetQuad(tri0, x0, y0, z0,  SHx, SHx, SHx, 255,  0, 1,
                          x0, y1, z0,  SHx, SHx, SHx, 255,  0, 0,
                          x0, y0, z1,  SHx, SHx, SHx, 255,  1, 1,
                          x0, y1, z1,  SHx, SHx, SHx, 255,  1, 0);
    }

    // EAST
    IF_DO(f1)
    {
        LAB_SetQuad(tri1, x1, y1, z0,  SHx, SHx, SHx, 255,  1, 0,
                          x1, y0, z0,  SHx, SHx, SHx, 255,  1, 1,
                          x1, y1, z1,  SHx, SHx, SHx, 255,  0, 0,
                          x1, y0, z1,  SHx, SHx, SHx, 255,  0, 1);
    }


    // BOTTOM
    IF_DO(f2)
    {
        LAB_SetQuad(tri2, x0, y0, z0,  SHy, SHy, SHy, 255,  1, 0,
                          x0, y0, z1,  SHy, SHy, SHy, 255,  1, 1,
                          x1, y0, z0,  SHy, SHy, SHy, 255,  0, 0,
                          x1, y0, z1,  SHy, SHy, SHy, 255,  0, 1);
    }

    // TOP
    IF_DO(f3)
    {
        LAB_SetQuad(tri3, x0, y1, z1,  255, 255, 255, 255,  1, 1,
                          x0, y1, z0,  255, 255, 255, 255,  1, 0,
                          x1, y1, z1,  255, 255, 255, 255,  0, 1,
                          x1, y1, z0,  255, 255, 255, 255,  0, 0);
    }


    // NORTH
    IF_DO(f4)
    {
        LAB_SetQuad(tri4, x0, y0, z0,  SHz, SHz, SHz, 255,  1, 1,
                          x1, y0, z0,  SHz, SHz, SHz, 255,  0, 1,
                          x0, y1, z0,  SHz, SHz, SHz, 255,  1, 0,
                          x1, y1, z0,  SHz, SHz, SHz, 255,  0, 0);
    }

    // SOUTH
    IF_DO(f5)
    {
        LAB_SetQuad(tri5, x1, y0, z1,  SHz, SHz, SHz, 255,  1, 1,
                          x0, y0, z1,  SHz, SHz, SHz, 255,  0, 1,
                          x1, y1, z1,  SHz, SHz, SHz, 255,  1, 0,
                          x0, y1, z1,  SHz, SHz, SHz, 255,  0, 0);
    }
#else
// Original
    // WEST
    IF_DO(f0)
    {
        LAB_SetQuad(tri, x0, y0, z0,  SHx, SHx, SHx, 255,  0, 1,
                         x0, y1, z0,  SHx, SHx, SHx, 255,  0, 0,
                         x0, y0, z1,  SHx, SHx, SHx, 255,  1, 1,
                         x0, y1, z1,  SHx, SHx, SHx, 255,  1, 0);
        tri+=2*f0;
    }

    // EAST
    IF_DO(f1)
    {
        LAB_SetQuad(tri, x1, y1, z0,  SHx, SHx, SHx, 255,  1, 0,
                         x1, y0, z0,  SHx, SHx, SHx, 255,  1, 1,
                         x1, y1, z1,  SHx, SHx, SHx, 255,  0, 0,
                         x1, y0, z1,  SHx, SHx, SHx, 255,  0, 1);
        tri+=2*f1;
    }


    // BOTTOM
    IF_DO(f2)
    {
        LAB_SetQuad(tri, x0, y0, z0,  SHy, SHy, SHy, 255,  1, 0,
                         x0, y0, z1,  SHy, SHy, SHy, 255,  1, 1,
                         x1, y0, z0,  SHy, SHy, SHy, 255,  0, 0,
                         x1, y0, z1,  SHy, SHy, SHy, 255,  0, 1);
        tri+=2*f2;
    }

    // TOP
    IF_DO(f3)
    {
        LAB_SetQuad(tri, x0, y1, z1,  255, 255, 255, 255,  1, 1,
                         x0, y1, z0,  255, 255, 255, 255,  1, 0,
                         x1, y1, z1,  255, 255, 255, 255,  0, 1,
                         x1, y1, z0,  255, 255, 255, 255,  0, 0);
        tri+=2*f3;
    }


    // NORTH
    IF_DO(f4)
    {
        LAB_SetQuad(tri, x0, y0, z0,  SHz, SHz, SHz, 255,  1, 1,
                         x1, y0, z0,  SHz, SHz, SHz, 255,  0, 1,
                         x0, y1, z0,  SHz, SHz, SHz, 255,  1, 0,
                         x1, y1, z0,  SHz, SHz, SHz, 255,  0, 0);
        tri+=2*f4;
    }

    // SOUTH
    IF_DO(f5)
    {
        LAB_SetQuad(tri, x1, y0, z1,  SHz, SHz, SHz, 255,  1, 1,
                         x0, y0, z1,  SHz, SHz, SHz, 255,  0, 1,
                         x1, y1, z1,  SHz, SHz, SHz, 255,  1, 0,
                         x0, y1, z1,  SHz, SHz, SHz, 255,  0, 0);
        //tri+=2;
    }
#endif
#endif
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


#if BLOCK_NEIGHBORHOOD
LAB_HOT
static void LAB_GetBlockNeighborhood(LAB_Chunk* cnk3x3x3[27], LAB_Block* blk3x3x3[27], int x, int y, int z)
{
    int i = 0;
    #define SWT(...) __VA_ARGS__
    //#define SWT(...)

    SWT(int dbx, dby, dbz;)

    SWT(dbz = -1;)
    for(int zz = z+15; zz <= z+17; ++zz)
    {
        SWT(dby = -1;)
        int cz = 3*3*(zz >> LAB_CHUNK_SHIFT);
        int iz = LAB_CHUNK_Z(zz & LAB_CHUNK_MASK);
        for(int yy = y+15; yy <= y+17; ++yy)
        {
            SWT(dbx = -1;)
            int cy = 3*(yy >> LAB_CHUNK_SHIFT);
            int iy = LAB_CHUNK_Y(yy & LAB_CHUNK_MASK);
            for(int xx = x+15; xx <= x+17; ++xx)
            {
                int cx = xx >> LAB_CHUNK_SHIFT;
                int ix = LAB_CHUNK_X(xx & LAB_CHUNK_MASK);
                SWT(if(!dbx + !dby + !dbz >= 2))
                    blk3x3x3[i] = cnk3x3x3[cx+cy+cz]!=NULL ? cnk3x3x3[cx+cy+cz]->blocks[ix+iy+iz] : &LAB_BLOCK_OUTSIDE;
                ++i;
                SWT(dbx++;)
            }
            SWT(dby++;)
        }
        SWT(dbz++;)
    }
}
#endif

static void LAB_ViewUploadVBO(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    LAB_ViewTriangle* mesh = chunk_entry->mesh;

    if(!chunk_entry->vbo) glGenBuffers(1, &chunk_entry->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, chunk_entry->vbo);
    glBufferData(GL_ARRAY_BUFFER, chunk_entry->mesh_count*sizeof *mesh, mesh, GL_DYNAMIC_DRAW);
}


static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    if(chunk_entry->dirty)
    {
        chunk_entry->dirty = 0;
        LAB_ViewBuildMesh(view, chunk_entry, view->world);

        if(view->flags & LAB_VIEW_USE_VBO)
            LAB_ViewUploadVBO(view, chunk_entry);
    }
    else
    {
        if(view->flags & LAB_VIEW_USE_VBO)
        {
            glBindBuffer(GL_ARRAY_BUFFER, chunk_entry->vbo);
        }
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


    if(view->flags & LAB_VIEW_USE_VBO)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LAB_ViewRenderProc(void* user, LAB_Window* window)
{
    LAB_View* view = (LAB_View*)user;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

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

    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        LAB_ViewRenderChunk(view, &view->chunks[i]);
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);


    // Render Crosshair
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






LAB_ViewChunkEntry* LAB_ViewGetChunkEntry(LAB_View* view, int x, int y, int z)
{
    /*LAB_ChunkPos chunkpos = { x, y, z };
    LAB_ViewChunkMap_Entry* entry = LAB_ViewChunkMap_Get(view->chunks, chunkpos);
    if(entry == NULL)
    {

    }
    return entry->value;*/

    #if 1
    LAB_ViewChunkEntry* chunks;
    size_t chunk_count, chunk_capacity;

    chunks = view->chunks;
    chunk_count = view->chunk_count;
    chunk_capacity = view->chunk_capacity;

    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        if(chunks[i].x == x && chunks[i].y == y && chunks[i].z == z)
            return &chunks[i];
    }

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
    #endif
}

void LAB_ViewInvalidateEverything(LAB_View* view)
{
    for(size_t i = 0; i < view->chunk_count; ++i)
    {
        view->chunks[i].dirty = 1;
    }
}



void LAB_ViewGetDirection(LAB_View* view, /*out*/ float dir[3])
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
