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
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>


#define HTL_PARAM LAB_VIEW_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM


static void LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world);
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* chunk_neighborhood[27]);
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Block* block_neighborhood[27], int x, int y, int z);
static void LAB_GetBlockNeighborhood(LAB_Chunk* chunk_neighborhood[27], LAB_Block* block_neighborhood[27], int x, int y, int z);
static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);

static LAB_ViewTriangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size);

GLuint LAB_gltextureid = 0;
void LAB_View_StaticInit()
{
    static char init = 0;
    if(init) return;
    init = 1;

    SDL_Surface* img = IMG_Load("assets/terrain.png");
    if(img == NULL) return;

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    //glGenerateMipmap(GL_TEXTURE_2D);

    glMatrixMode(GL_TEXTURE);
    glScalef(32.f / (float)img->w, 32.f / (float)img->h, 1);

    SDL_FreeSurface(img);
    printf("SUCCESS\n");
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

    LAB_ViewBuildMeshNeighbored(view, chunk_entry, chunk_neighborhood);
}
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Chunk* cnk3x3x3[27])
{
    const int X = 1;
    const int Y = 3*1;
    const int Z = 3*3*1;

    LAB_Block* blk3x3x3[27];

    chunk_entry->mesh_count = 0;

    for(size_t z = 0; z < LAB_CHUNK_SIZE; ++z)
    {
        for(size_t y = 0; y < LAB_CHUNK_SIZE; ++y)
        {
            for(size_t x = 0; x < LAB_CHUNK_SIZE; ++x)
            {
                LAB_GetBlockNeighborhood(cnk3x3x3, blk3x3x3, x, y, z);
                if(blk3x3x3[X+Y+Z]->flags & LAB_BLOCK_SOLID)
                    LAB_ViewBuildMeshBlock(view, chunk_entry, blk3x3x3, x, y, z);
                //return;
            }
        }
    }
}



__attribute__((hot))
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_Block* blk3x3x3[27], int x, int y, int z)
{
    const int X = 1;
    const int Y = 3*1;
    const int Z = 3*3*1;

    LAB_Block* block = blk3x3x3[X+Y+Z];

    int cr = block->r;
    int cg = block->g;
    int cb = block->b;

    int tx = block->tx;
    int ty = block->ty;

    int faces = 0;
    faces |=  1*(!(blk3x3x3[0*X+Y+Z]->flags&LAB_BLOCK_SOLID));
    faces |=  2*(!(blk3x3x3[2*X+Y+Z]->flags&LAB_BLOCK_SOLID));
    faces |=  4*(!(blk3x3x3[X+0*Y+Z]->flags&LAB_BLOCK_SOLID));
    faces |=  8*(!(blk3x3x3[X+2*Y+Z]->flags&LAB_BLOCK_SOLID));
    faces |= 16*(!(blk3x3x3[X+Y+0*Z]->flags&LAB_BLOCK_SOLID));
    faces |= 32*(!(blk3x3x3[X+Y+2*Z]->flags&LAB_BLOCK_SOLID));

    // TODO: use popcount instead
    int face_count = !!(faces &  1)
                   + !!(faces &  2)
                   + !!(faces &  4)
                   + !!(faces &  8)
                   + !!(faces & 16)
                   + !!(faces & 32);

    LAB_ViewTriangle* tri;
    tri = LAB_ViewMeshAlloc(chunk_entry, 2*face_count);
    if(tri == NULL) return;


    float x0 = x,    y0 = y,    z0 = z;
    float x1 = x0+1, y1 = y0+1, z1 = z0+1;

    /*#define LAB_SetVertex(vert, xx, yy, zz, rr, gg, bb, aa) \
        (void)(((vert).x = xx), ((vert).y = yy), ((vert).z = zz), ((vert).r = rr), ((vert).g = gg), ((vert).b = bb), ((vert).a = aa))*/

    /*#define LAB_SetVertex(vert, xx, yy, zz, rr, gg, bb, aa) do \
    {                                                                \
        (vert).x = xx; (vert).y = yy; (vert).z = zz;                 \
        (vert).r = rr; (vert).g = gg; (vert).b = bb; (vert).a = aa; \
        (vert).r = ((int)xx&0xf)*17*rr/255;                               \
        (vert).g = ((int)yy&0xf)*17*gg/255;                               \
        (vert).b = ((int)zz&0xf)*17*aa/255;                               \
        (vert).a = aa;                                               \
    } while(0)*/

    #define LAB_SetVertex(vert,  xx, yy, zz,  rr, gg, bb, aa,  uu, vv) do \
    {                                                                     \
        (vert).x = xx; (vert).y = yy; (vert).z = zz;                      \
        /*(vert).r = rr; (vert).g = gg; (vert).b = bb; (vert).a = aa;*/   \
        (vert).r = ((int)cr)*rr/255;                                      \
        (vert).g = ((int)cg)*gg/255;                                      \
        (vert).b = ((int)cb)*bb/255;                                      \
        (vert).a = aa;                                                    \
        (vert).u = uu + tx;                                                    \
        (vert).v = vv + ty;                                                    \
    } while(0)

    #define LAB_SetQuad(x0, y0, z0,  r0, g0, b0, a0,  u0, v0, \
                        x1, y1, z1,  r1, g1, b1, a1,  u1, v1, \
                        x2, y2, z2,  r2, g2, b2, a2,  u2, v2, \
                        x3, y3, z3,  r3, g3, b3, a3,  u3, v3) do \
    { \
        LAB_ViewVertex* t0 = tri[0].v;                    \
        LAB_ViewVertex* t1 = tri[1].v;                    \
        LAB_SetVertex(t0[1],  x0, y0, z0,  r0, g0, b0, a0,  u0, v0); \
        LAB_SetVertex(t0[2],  x1, y1, z1,  r1, g1, b1, a1,  u1, v1); \
        LAB_SetVertex(t1[2],  x1, y1, z1,  r1, g1, b1, a1,  u1, v1); \
        LAB_SetVertex(t0[0],  x2, y2, z2,  r2, g2, b2, a2,  u2, v2); \
        LAB_SetVertex(t1[1],  x2, y2, z2,  r2, g2, b2, a2,  u2, v2); \
        LAB_SetVertex(t1[0],  x3, y3, z3,  r3, g3, b3, a3,  u3, v3); \
        /*int r, g, b;                                      \
        r = x*17*r0/255;                                  \
        g = y*17*g0/255;                                  \
        b = z*17*b0/255;                                  \
        LAB_SetVertex(v0[0], x0, y0, z0, r, g, b, a0); \
        LAB_SetVertex(v0[1], x1, y1, z1, r, g, b, a1); \
        LAB_SetVertex(v1[1], x1, y1, z1, r, g, b, a1); \
        LAB_SetVertex(v0[2], x2, y2, z2, r, g, b, a2); \
        LAB_SetVertex(v1[0], x2, y2, z2, r, g, b, a2); \
        LAB_SetVertex(v1[2], x3, y3, z3, r, g, b, a3);*/ \
        tri+=2;                                           \
    } while(0)

    // WEST
    if(faces&1)   LAB_SetQuad(x0, y0, z0,  235, 235, 235, 255,  0, 1,
                              x0, y1, z0,  235, 235, 235, 255,  0, 0,
                              x0, y0, z1,  235, 235, 235, 255,  1, 1,
                              x0, y1, z1,  235, 235, 235, 255,  1, 0);

    // EAST
    if(faces&2)   LAB_SetQuad(x1, y1, z0,  235, 235, 235, 255,  1, 0,
                              x1, y0, z0,  235, 235, 235, 255,  1, 1,
                              x1, y1, z1,  235, 235, 235, 255,  0, 0,
                              x1, y0, z1,  235, 235, 235, 255,  0, 1);


    // BOTTOM
    if(faces&4)   LAB_SetQuad(x0, y0, z0,  192, 192, 192, 255,  1, 0,
                              x0, y0, z1,  192, 192, 192, 255,  1, 1,
                              x1, y0, z0,  192, 192, 192, 255,  0, 0,
                              x1, y0, z1,  192, 192, 192, 255,  0, 1);

    // TOP
    if(faces&8)   LAB_SetQuad(x0, y1, z1,  255, 255, 255, 255,  1, 0,
                              x0, y1, z0,  255, 255, 255, 255,  1, 1,
                              x1, y1, z1,  255, 255, 255, 255,  0, 0,
                              x1, y1, z0,  255, 255, 255, 255,  0, 1);


    // NORTH
    if(faces&16)  LAB_SetQuad(x0, y0, z0,  215, 215, 215, 255,  1, 1,
                              x1, y0, z0,  215, 215, 215, 255,  0, 1,
                              x0, y1, z0,  215, 215, 215, 255,  1, 0,
                              x1, y1, z0,  215, 215, 215, 255,  0, 0);

    // SOUTH
    if(faces&32)  LAB_SetQuad(x1, y0, z1,  215, 215, 215, 255,  1, 1,
                              x0, y0, z1,  215, 215, 215, 255,  0, 1,
                              x1, y1, z1,  215, 215, 215, 255,  1, 0,
                              x0, y1, z1,  215, 215, 215, 255,  0, 0);

}


static LAB_ViewTriangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size)
{
    size_t mesh_count, new_mesh_count, mesh_capacity;

    mesh_count = chunk_entry->mesh_count;
    new_mesh_count = mesh_count+add_size;
    mesh_capacity = chunk_entry->mesh_capacity;

    if(new_mesh_count > mesh_capacity)
    {
        if(mesh_capacity == 0) mesh_capacity = 1;
        while(new_mesh_count > mesh_capacity) mesh_capacity *= 2;
        LAB_ViewTriangle* mesh = LAB_ReallocN(chunk_entry->mesh, new_mesh_count, sizeof *mesh);
        if(!mesh) {
            return NULL;
        }
        chunk_entry->mesh = mesh;
    }
    chunk_entry->mesh_count=new_mesh_count;

    return &chunk_entry->mesh[mesh_count];
}



static void LAB_GetBlockNeighborhood(LAB_Chunk* cnk3x3x3[27], LAB_Block* blk3x3x3[27], int x, int y, int z)
{
    int i = 0;
    for(int zz = z+15; zz <= z+17; ++zz)
    {
        int cz = 3*3*(zz >> LAB_CHUNK_SHIFT);
        int iz = LAB_CHUNK_Z(zz & LAB_CHUNK_MASK);
        for(int yy = y+15; yy <= y+17; ++yy)
        {
            int cy = 3*(yy >> LAB_CHUNK_SHIFT);
            int iy = LAB_CHUNK_Y(yy & LAB_CHUNK_MASK);
            for(int xx = x+15; xx <= x+17; ++xx)
            {
                int cx = xx >> LAB_CHUNK_SHIFT;
                int ix = LAB_CHUNK_X(xx & LAB_CHUNK_MASK);
                blk3x3x3[i] = cnk3x3x3[cx+cy+cz]!=NULL ? cnk3x3x3[cx+cy+cz]->blocks[ix+iy+iz] : &LAB_BLOCK_OUTSIDE;
                ++i;
            }
        }
    }
}

static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    if(chunk_entry->dirty)
    {
        LAB_ViewBuildMesh(view, chunk_entry, view->world);
        chunk_entry->dirty = 0;
    }


    if(chunk_entry->mesh_count == 0) return;
    glPushMatrix();
    glTranslatef(LAB_CHUNK_SIZE*chunk_entry->x, LAB_CHUNK_SIZE*chunk_entry->y, LAB_CHUNK_SIZE*chunk_entry->z);
    LAB_ViewTriangle* mesh = chunk_entry->mesh;
    glVertexPointer(3, GL_FLOAT, sizeof *mesh->v, &mesh->v[0].x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof *mesh->v, &mesh->v[0].r);
    glTexCoordPointer(2, GL_FLOAT, sizeof *mesh->v, &mesh->v[0].u);
    glDrawArrays(GL_TRIANGLES, 0, 3*chunk_entry->mesh_count);
    glPopMatrix();
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

    if(view->chunk_count >= chunk_capacity)
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
