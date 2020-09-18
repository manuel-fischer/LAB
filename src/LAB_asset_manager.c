#include "LAB_asset_manager.h"

#include "LAB_opt.h"
#include "LAB_stdinc.h"
#include "LAB_gl.h"
#include <SDL2/SDL_image.h>

SDL_Surface* LAB_block_terrain = NULL;
unsigned     LAB_block_terrain_gl_id = 0;

void LAB_InitAssets()
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
    glGenTextures(1, &LAB_block_terrain_gl_id);
    glBindTexture(GL_TEXTURE_2D, LAB_block_terrain_gl_id);

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
    glScalef((float)LAB_TILE_SIZE / (float)img->w, (float)LAB_TILE_SIZE / (float)img->h, 1);

    LAB_block_terrain = img;
    LAB_GL_CHECK();
}
