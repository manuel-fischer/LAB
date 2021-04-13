#include "LAB_image.h"

#include <SDL2/SDL_image.h>
#include "LAB_sdl.h" // LAB_SDL_ALLOC -- TODO




void LAB_MemSetColor(LAB_Color* dst, LAB_Color color, size_t size)
{
    for(size_t i = 0; i < size; ++i)
        dst[i] = color;
}

void LAB_MemCpyColor(LAB_Color* dst, LAB_Color const* src, size_t size)
{
//    for(size_t i = 0; i < size; ++i)
//        dst[i] = src[i];
    memcpy(dst, src, size*sizeof(LAB_Color));
}



#include <stdio.h> // TODO remove
SDL_Surface* LAB_ImageLoad(const char* filename)
{
    SDL_Surface* img;
    fprintf(stderr, "Loading image %s\n", filename);
    LAB_SDL_ALLOC(IMG_Load, &img, filename);
    if(LAB_UNLIKELY(img == NULL)) return NULL;

    if(img->format->format != SDL_PIXELFORMAT_RGBA32)
    {
        SDL_Surface* nImg;
        LAB_SDL_ALLOC(SDL_ConvertSurfaceFormat, &nImg, img, SDL_PIXELFORMAT_RGBA32, 0);
        LAB_SDL_FREE(SDL_FreeSurface, &img);
        return nImg;
    }
    return img;
}


bool LAB_ImageSaveData(const char* filename, size_t w, size_t h, LAB_Color* data)
{
    SDL_Surface* surf;
    LAB_SDL_ALLOC(SDL_CreateRGBSurfaceWithFormat, &surf, 0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if(!surf) return false;
    memcpy(surf->pixels, data, w*h*sizeof*data);
    IMG_SavePNG(surf, filename);
    LAB_SDL_FREE(SDL_FreeSurface, &surf);
    return true;
}
