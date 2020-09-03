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

static int  LAB_ViewBuildMesh(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_World* world);
static void LAB_ViewBuildMeshNeighbored(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_IN LAB_Chunk* chunk_neighborhood[27]);
static void LAB_ViewBuildMeshBlock(LAB_View* view, LAB_ViewChunkEntry* chunk_entry, LAB_IN LAB_Chunk* chunk_neighborhood[27], int x, int y, int z);
static void LAB_ViewRenderChunks(LAB_View* view);
static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);
static void LAB_ViewRemoveDistantChunks(LAB_View* view);
static void LAB_ViewDestructChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry);

static LAB_Triangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size, size_t extra_size);

GLuint LAB_gltextureid = 0;
static void LAB_View_StaticInit(void)
{
    // TODO
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
    // Mipmaps
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
    LAB_GL_CHECK();
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
    for(size_t i = 0; i < view->chunk_count; ++i)
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


LAB_HOT
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


    LAB_Color light_sides[6];
    for(int face_itr=faces; face_itr; face_itr &= face_itr-1)
    {
        int face = LAB_Ctz(face_itr);
        const int* o = LAB_offset[face];
        light_sides[face] = LAB_GetNeighborhoodLight(cnk3x3x3, x+o[0], y+o[1], z+o[2], LAB_RGB(255, 255, 255));
    }


    const LAB_Model* model = block->model;
    if(!model) return;
    LAB_Triangle* tri;
    tri = LAB_ViewMeshAlloc(chunk_entry, model->size, 0);
    if(LAB_UNLIKELY(tri == NULL)) return;
    int count = LAB_PutModelShadedAt(tri, model, x, y, z, faces, light_sides);
    chunk_entry->mesh_count -= model->size-count;

#undef GET_BLOCK_FLAGS
#undef GET_BLOCK

}


static LAB_Triangle* LAB_ViewMeshAlloc(LAB_ViewChunkEntry* chunk_entry, size_t add_size, size_t extra_size)
{
    size_t mesh_count, new_mesh_count, mesh_capacity;

    mesh_count = chunk_entry->mesh_count;
    new_mesh_count = mesh_count+add_size;
    mesh_capacity = chunk_entry->mesh_capacity;

    if(LAB_UNLIKELY(new_mesh_count+extra_size > mesh_capacity))
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

    if(!chunk_entry->vbo) glGenBuffers(1, &chunk_entry->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, chunk_entry->vbo);
    glBufferData(GL_ARRAY_BUFFER, chunk_entry->mesh_count*sizeof *mesh, mesh, GL_DYNAMIC_DRAW);
}
#endif

static void LAB_ViewRenderChunk(LAB_View* view, LAB_ViewChunkEntry* chunk_entry)
{
    if(view->rest_update && chunk_entry->dirty)
    {
        int chunk_available;
        chunk_available = LAB_ViewBuildMesh(view, chunk_entry, view->world);
        chunk_entry->dirty = !chunk_available;
        view->rest_update -= chunk_available;

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















void LAB_GL_ActivateTexture(unsigned* gl_id)
{
    if(*gl_id == 0)
    {
        glGenTextures(1, gl_id);
        glBindTexture(GL_TEXTURE_2D, *gl_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, INFO_WIDTH, INFO_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, *gl_id);
    }
}

void LAB_UploadSurf(unsigned gl_id, SDL_Surface* surf)
{
    int info_width  = LAB_CeilPow2(surf->w);
    int info_height = LAB_CeilPow2(surf->h);

    int free_surf = 0;
    if(surf->format->format != SDL_PIXELFORMAT_RGBA32)
    {
        SDL_Surface* nImg;
        nImg = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
        if(nImg == NULL) return;
        surf = nImg;
        free_surf = 1;
        //printf("Conv\n");
    }

    /*for(int x = 0; x < 16; ++x)
    {
        printf("%08x\n", ((uint32_t*)surf->pixels)[x]);
    }*/

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info_width, info_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surf->w, surf->h, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    if(free_surf) SDL_FreeSurface(surf);
}

void LAB_DrawSurf(LAB_View* view, unsigned gl_id, int x, int y, int w, int h)
{
    int rw, rh;
    rw = LAB_CeilPow2(w);
    rh = LAB_CeilPow2(h);


    // partly const
    static float info[5*3*2] = {
          0, (0), -1,   0,   0,
        (0),   0, -1, (1), (1),
          0,   0, -1,   0, (1),
        //
          0, (0), -1,   0,   0,
        (0), (0), -1, (1),   0,
        (0),   0, -1, (1), (1),
    };

    info[5*1] = info[5*4] = info[5*5]   = w;
    info[1] = info[5*3+1] = info[5*4+1] = h;

    info[5*1+3] = info[5*4+3] = info[5*5+3] = (float)w/rw;
    info[5*1+4] = info[5*2+4] = info[5*5+4] = (float)h/rh;


    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float f = 2.f/view->h;
    glScalef(f, f, 1);
    glTranslatef(-view->w/2+x, -view->h/2+y, 0);

    glVertexPointer(3, LAB_GL_TYPEOF(*info), sizeof *info * 5, info);
    glTexCoordPointer(2, LAB_GL_TYPEOF(*info), sizeof *info * 5, info+3);
    glDrawArrays(GL_TRIANGLES, 0, 3*2);

    LAB_GL_CHECK();

    //glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
}







void LAB_ViewRenderGui(LAB_View* view)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);

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
    glVertexPointer(3, LAB_GL_TYPEOF(crosshair[0]), 0, crosshair);
    glDrawArrays(GL_TRIANGLES, 0, 3*4);

    {
        if(view->info.font == NULL)
        {
            view->info.font = TTF_OpenFont("fonts/DejaVuSansMono.ttf", 13);
            if(!view->info.font) return;
        }

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

            SDL_Color fg = { 255, 255, 255, 255 };
            SDL_Color bg = {   0,   0,   0, 255 };

            view->info.surf = TTF_RenderUTF8_Shaded(view->info.font, buf, fg, bg);
            if(!view->info.surf) return;


            LAB_UploadSurf(view->info.gl_texture, view->info.surf);
        }

        LAB_DrawSurf(view, view->info.gl_texture, 0, view->h-view->info.surf->h, view->info.surf->w, view->info.surf->h);
        /*glLoadIdentity();
        glEnable(GL_TEXTURE_2D);

        // partly const
        static float info[5*3*2] = {
                0, (0), -1,   0,   0,
              (0),   0, -1, (1), (1),
                0,   0, -1,   0, (1),
              //
                0, (0), -1,   0,   0,
              (0), (0), -1, (1),   0,
              (0),   0, -1, (1), (1),
        };
        int w, h;
        w = view->info.surf->w;
        h = view->info.surf->h;

        info[5*1] = info[5*4] = info[5*5]   = w;
        info[1] = info[5*3+1] = info[5*4+1] = h;

        info[5*1+3] = info[5*4+3] = info[5*5+3] = (float)w/info_width;
        info[5*1+4] = info[5*2+4] = info[5*5+4] = (float)h/info_height;


        glEnableClientState(GL_TEXTURE_COORD_ARRAY);


        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        float f = 2.f/view->h;
        glScalef(f, f, 1);
        glTranslatef(-view->w/2, +view->h/2-h, 0);

        glVertexPointer(3, LAB_GL_TYPEOF(*info), sizeof *info * 5, info);
        glTexCoordPointer(2, LAB_GL_TYPEOF(*info), sizeof *info * 5, info+3);
        glDrawArrays(GL_TRIANGLES, 0, 3*2);

        LAB_GL_CHECK();
        glPopMatrix();
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();*/
    }


    if(0)
    {
         LAB_GuiMenu menu;
         LAB_GuiMenu_Create(&menu);

         int scale = 2;

         menu.x = (view->w/scale-menu.w)/2;
         menu.y = (view->h/scale-menu.h)/2;

         SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, menu.w, menu.h, 32, SDL_PIXELFORMAT_RGBA32);
         if(!surf) return;

         LAB_GuiContainer_Render_Framed((LAB_GuiComponent*)&menu, surf, 0, 0);
         //LAB_RenderRect(surf, 10, 10, 80, 20, 2, 1);

         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

         static unsigned gl_id = 0;
         LAB_GL_ActivateTexture(&gl_id);
         LAB_UploadSurf(gl_id, surf);
         LAB_DrawSurf(view, gl_id, menu.x*scale, menu.y*scale, menu.w*scale, menu.h*scale);

         SDL_FreeSurface(surf);
     }
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
    view->w = w; view->h = h;
    float ratio = h?(float)w/(float)h:1;
    float nearp = 0.075f;
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

    int load_amount = 3; // should be configurable

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
                            entry = LAB_ViewNewChunkEntry(view, xx, yy, zz);
                            if(entry == NULL) return; // NO MEMORY
                            entry->dirty = 1;
                        }
                        if(entry->dirty)
                        {
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
        glDeleteBuffers(1, &chunk_entry->vbo);
    #endif
}
