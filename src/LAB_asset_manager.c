#include "LAB_asset_manager.h"

#include "LAB_opt.h"
#include "LAB_stdinc.h"
#include "LAB_gl.h"
#include "LAB_color.h"
#include "LAB_memory.h"
#include "LAB_debug.h"
#include <SDL2/SDL_image.h>

SDL_Surface* LAB_block_terrain = NULL;
unsigned     LAB_block_terrain_gl_id = 0;

// alpha corrected mip maps
LAB_STATIC void LAB_GL_GenerateMipmap2D(size_t w, size_t h, LAB_Color* data, int num_mipmaps);
LAB_STATIC void LAB_Fix0Alpha(size_t w, size_t h, LAB_Color* data);

void LAB_InitAssets(void)
{
    // TODO
    static bool init = 0;
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
    glGenTextures(1, &LAB_block_terrain_gl_id); LAB_GL_DEBUG_ALLOC(1);
    glBindTexture(GL_TEXTURE_2D, LAB_block_terrain_gl_id);

    #if LAB_MIPMAPS
    // Mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    const int num_mipmaps = 5;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_mipmaps);

    LAB_Fix0Alpha(img->w, img->h, (LAB_Color*)img->pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    //glGenerateMipmap(GL_TEXTURE_2D);
    LAB_GL_GenerateMipmap2D(img->w, img->h, img->pixels, num_mipmaps);
    #else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
    #endif

    glMatrixMode(GL_TEXTURE);
    glScalef((float)LAB_TILE_SIZE / (float)img->w, (float)LAB_TILE_SIZE / (float)img->h, 1);

    LAB_block_terrain = img;
    LAB_GL_CHECK();
}

void LAB_QuitAssets(void)
{
    glDeleteTextures(1, &LAB_block_terrain_gl_id); LAB_GL_DEBUG_FREE(1);
    SDL_FreeSurface(LAB_block_terrain);

    LAB_block_terrain_gl_id = 0;
    LAB_block_terrain = NULL;
}




LAB_STATIC void LAB_Fix0Alpha(size_t w, size_t h, LAB_Color* data)
{
    // Fix fully transparent pixels
    for(size_t x = 0;         x < w; ++x)
    for(size_t y = 0, yi = 0; y < h; ++y, yi+=w)
    {
        if(LAB_ALP(data[x+yi]) == 0)
        {
            LAB_Color b = data[(x^1) +  yi   ];
            LAB_Color c = data[ x    + (yi^w)];
            LAB_Color d = data[(x^1) + (yi^w)];

            int sum_alp = LAB_ALP(b)+LAB_ALP(c)+LAB_ALP(d);
            if(sum_alp != 0)
            {
                int sum_red =                              (int)LAB_RED(b)*LAB_ALP(b)
                            + (int)LAB_RED(c)*LAB_ALP(c) + (int)LAB_RED(d)*LAB_ALP(d);
                int sum_grn =                              (int)LAB_GRN(b)*LAB_ALP(b)
                            + (int)LAB_GRN(c)*LAB_ALP(c) + (int)LAB_GRN(d)*LAB_ALP(d);
                int sum_blu =                              (int)LAB_BLU(b)*LAB_ALP(b)
                            + (int)LAB_BLU(c)*LAB_ALP(c) + (int)LAB_BLU(d)*LAB_ALP(d);
                data[x+yi] = LAB_RGBA(sum_red/sum_alp, sum_grn/sum_alp, sum_blu/sum_alp, 0);
            }
        }
    }
}


// w, h the size of the new texture
LAB_STATIC void LAB_CalculateMipmap2D(size_t w, size_t h, LAB_Color* in_data, LAB_Color* out_data)
{
    for(size_t x = 0;         x < w; ++x)
    for(size_t y = 0, yi = 0; y < h; ++y, yi+=w)
    {
        LAB_Color a = in_data[2*x   + 4*yi    ];
        LAB_Color b = in_data[2*x+1 + 4*yi    ];
        LAB_Color c = in_data[2*x   + 4*yi+2*w];
        LAB_Color d = in_data[2*x+1 + 4*yi+2*w];

        int sum_alp = (int)LAB_ALP(a)+LAB_ALP(b)+LAB_ALP(c)+LAB_ALP(d);
        if(sum_alp == 0)
        {
            out_data[x+yi] = 0;
        }
        else
        {
            int sum_red = (int)LAB_RED(a)*LAB_ALP(a) + (int)LAB_RED(b)*LAB_ALP(b)
                        + (int)LAB_RED(c)*LAB_ALP(c) + (int)LAB_RED(d)*LAB_ALP(d);
            int sum_grn = (int)LAB_GRN(a)*LAB_ALP(a) + (int)LAB_GRN(b)*LAB_ALP(b)
                        + (int)LAB_GRN(c)*LAB_ALP(c) + (int)LAB_GRN(d)*LAB_ALP(d);
            int sum_blu = (int)LAB_BLU(a)*LAB_ALP(a) + (int)LAB_BLU(b)*LAB_ALP(b)
                        + (int)LAB_BLU(c)*LAB_ALP(c) + (int)LAB_BLU(d)*LAB_ALP(d);
            out_data[x+yi] = LAB_RGBA(sum_red/sum_alp, sum_grn/sum_alp, sum_blu/sum_alp, sum_alp/4);
        }
    }
}

LAB_STATIC void LAB_DebugSaveImage(size_t w, size_t h, LAB_Color* data, const char* fname_fmt, ...);

LAB_STATIC void LAB_GL_GenerateMipmap2D(size_t w, size_t h, LAB_Color* data, int num_mipmaps)
{
    //LAB_Color* tmp_buf = LAB_Malloc(w*h*sizeof*tmp_buf);
    LAB_Color* tmp_buf = LAB_Malloc((w/2*h/2+w/4*w/4)*sizeof*tmp_buf);
    LAB_Color* tmp_buf_2 = tmp_buf+(w/2*h/2);

    LAB_Color* new_data = tmp_buf;

    for(int i = 1; i <= num_mipmaps; ++i)
    {
        w/=2; h/=2;
        LAB_CalculateMipmap2D(w, h, data, new_data);
        if(i != num_mipmaps) LAB_Fix0Alpha(w, h, new_data);
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, new_data);
        LAB_DebugSaveImage(w, h, new_data, "dbg_terrain_%i.png", i);
        //data = new_data;
        if(new_data == tmp_buf)
            new_data = tmp_buf_2, data = tmp_buf;
        else
            data = tmp_buf_2, new_data = tmp_buf;
    }

    LAB_Free(tmp_buf);
}








LAB_STATIC void LAB_DebugSaveImage(size_t w, size_t h, LAB_Color* data, const char* fname_fmt, ...)
{
    char fname_buf[300];
    va_list lst;
    va_start(lst, fname_fmt);
    vsnprintf(fname_buf, sizeof(fname_buf), fname_fmt, lst);
    va_end(lst);

    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    LAB_ASSUME(surf);
    memcpy(surf->pixels, data, w*h*sizeof*data);
    printf("LAB_DebugSaveImage \"%s\"\n", fname_buf);
    IMG_SavePNG(surf, fname_buf);
    SDL_FreeSurface(surf);
}




































