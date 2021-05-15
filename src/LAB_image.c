#include "LAB_image.h"

#include <SDL2/SDL_image.h>
#include "LAB_sdl.h" // LAB_SDL_ALLOC -- TODO


#include "LAB_bits.h"


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

void LAB_MemBltColor(LAB_Color* dst, LAB_Color const* src, LAB_Color tint, size_t size)
{
    for(size_t i = 0; i < size; ++i)
        dst[i] = LAB_BlendColor(dst[i], LAB_MulColor_Fast(src[i], tint));
}

void LAB_MemBltColor2(LAB_Color* dst, LAB_Color const* src, LAB_Color black_tint, LAB_Color white_tint, size_t size)
{
    black_tint &= ~LAB_ALP_MASK; // TODO?
    for(size_t i = 0; i < size; ++i)
        dst[i] = LAB_BlendColor(dst[i], LAB_LerpColor(black_tint, white_tint, src[i]));
}

void LAB_MemNoColor(LAB_Color* dst, size_t size)
{
    LAB_MemSetColor(dst, LAB_RGBX(ff0000), size);
    //return;
    //for(size_t i = 0; i < size; ++i)
    //    dst[i] = LAB_PopCnt(i)&1 ? LAB_RGBX(ff0000) : LAB_RGBX(ff4020);
}


#include <stdio.h> // TODO remove
SDL_Surface* LAB_ImageLoad(const char* filename)
{
    SDL_Surface* img;
    LAB_DBG_PRINTF("Loading image %s\n", filename);
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

SDL_Surface* LAB_ImageLoad_Fmt(const char* fname_fmt, ...)
{
    char fname_buf[300];
    va_list lst;
    va_start(lst, fname_fmt);
    vsnprintf(fname_buf, sizeof(fname_buf), fname_fmt, lst);
    va_end(lst);

    return LAB_ImageLoad(fname_buf);
}


SDL_Surface* LAB_ImageData2SDL(size_t w, size_t h, LAB_Color* data)
{
    SDL_Surface* surf;
    LAB_SDL_ALLOC(SDL_CreateRGBSurfaceWithFormat, &surf, 0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if(!surf) return false;
    memcpy(surf->pixels, data, w*h*sizeof*data);
    return surf;
}


bool LAB_ImageSave(size_t w, size_t h, LAB_Color* data, const char* filename)
{
    SDL_Surface* surf = LAB_ImageData2SDL(w, h, data);
    if(!surf) return false;
    LAB_DBG_PRINTF("LAB_ImageSave \"%s\"\n", filename);
    IMG_SavePNG(surf, filename);
    LAB_SDL_FREE(SDL_FreeSurface, &surf);
    return true;
}

void LAB_ImageSave_Fmt(size_t w, size_t h, LAB_Color* data, const char* fname_fmt, ...)
{
    char fname_buf[300];
    va_list lst;
    va_start(lst, fname_fmt);
    vsnprintf(fname_buf, sizeof(fname_buf), fname_fmt, lst);
    va_end(lst);


    LAB_ImageSave(w, h, data, fname_buf);
}






// w, h the size of the new texture
void LAB_MakeMipmap2D(size_t w, size_t h, const LAB_Color* in_data, LAB_OUT LAB_Color* out_data)
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


void LAB_Fix0Alpha(size_t w, size_t h, LAB_Color* data)
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
