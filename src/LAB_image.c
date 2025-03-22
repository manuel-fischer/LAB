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

void LAB_MemBlendColor(LAB_Color* dst, LAB_Color const* src, size_t size)
{
    for(size_t i = 0; i < size; ++i)
        dst[i] = LAB_BlendColor(dst[i], src[i]);
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

void LAB_MemTintColor2(LAB_Color* dst, LAB_Color const* src, LAB_Color black_tint, LAB_Color white_tint, size_t size)
{
    black_tint &= ~LAB_ALP_MASK; // TODO?
    for(size_t i = 0; i < size; ++i)
        dst[i] = LAB_LerpColor(black_tint, white_tint, src[i]);
}

void LAB_MemNoColor(LAB_Color* dst, size_t size)
{
    LAB_MemSetColor(dst, LAB_RGBX(ff0000), size);
    //return;
    //for(size_t i = 0; i < size; ++i)
    //    dst[i] = LAB_PopCnt(i)&1 ? LAB_RGBX(ff0000) : LAB_RGBX(ff4020);
}

void LAB_MemMulColor_Fast(LAB_Color* dst, LAB_Color color, size_t size)
{
    for(size_t i = 0; i < size; ++i)
        dst[i] = LAB_MulColor_Fast(dst[i], color);
}


#include <stdio.h> // TODO remove
SDL_Surface* LAB_ImageLoad(const char* filename)
{
    SDL_Surface* img;
    //LAB_DBG_PRINTF("Loading image %s\n", filename);
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
    LAB_SDL_ALLOC(SDL_CreateRGBSurfaceFrom, &surf, data, w, h, 32, 4*w,
                                            LAB_RED_MASK,
                                            LAB_GRN_MASK,
                                            LAB_BLU_MASK,
                                            LAB_ALP_MASK);
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


void LAB_Fix0Alpha(size_t w, size_t h, LAB_Color* data, size_t stride)
{
    LAB_ASSERT(LAB_IsPow2(stride));
    LAB_ASSERT((w&1) == 0);
    LAB_ASSERT((h&1) == 0);

    // Fix fully transparent pixels
    for(size_t x = 0;         x < w; ++x)
    for(size_t y = 0, yi = 0; y < h; ++y, yi+=stride)
    {
        if(LAB_ALP(data[x+yi]) == 0)
        {
            LAB_Color b = data[(x^1) +  yi        ];
            LAB_Color c = data[ x    + (yi^stride)];
            LAB_Color d = data[(x^1) + (yi^stride)];

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


void LAB_Image_FillColor(LAB_ImageView vdst, LAB_Color color)
{
    if(vdst.stride == vdst.w)
    {
        LAB_MemSetColor(vdst.data, color, vdst.w*vdst.h);
        return;
    }

    LAB_Color* dst = vdst.data;
    for(size_t y = 0; y < vdst.h; ++y, dst+=vdst.stride)
        LAB_MemSetColor(dst, color, vdst.w);
}

void LAB_Image_Copy(LAB_ImageView vdst, LAB_ImageCView vsrc)
{
    LAB_ASSERT(vdst.w == vsrc.w);
    LAB_ASSERT(vdst.h == vsrc.h);

    LAB_Color* dst = vdst.data;
    const LAB_Color* src = vsrc.data;
    for(size_t y = 0; y < vdst.h; ++y, dst+=vdst.stride, src+=vsrc.stride)
        LAB_MemCpyColor(dst, src, vdst.w);
}

void LAB_Image_Blend(LAB_ImageView vdst, LAB_ImageCView vsrc)
{
    LAB_ASSERT(vdst.w == vsrc.w);
    LAB_ASSERT(vdst.h == vsrc.h);

    LAB_Color* dst = vdst.data;
    const LAB_Color* src = vsrc.data;
    for(size_t y = 0; y < vdst.h; ++y, dst+=vdst.stride, src+=vsrc.stride)
        LAB_MemBlendColor(dst, src, vdst.w);
}

void LAB_Image_Blit(LAB_ImageView vdst, LAB_ImageCView vsrc, LAB_Color tint)
{
    LAB_ASSERT(vdst.w == vsrc.w);
    LAB_ASSERT(vdst.h == vsrc.h);

    LAB_Color* dst = vdst.data;
    const LAB_Color* src = vsrc.data;
    for(size_t y = 0; y < vdst.h; ++y, dst+=vdst.stride, src+=vsrc.stride)
        LAB_MemBltColor(dst, src, tint, vdst.w);
}

void LAB_Image_Blit2(LAB_ImageView vdst, LAB_ImageCView vsrc, LAB_Color black_tint, LAB_Color white_tint)
{
    if((black_tint & LAB_COL_MASK) == 0 && white_tint == LAB_RGBI(0xffffff))
    {
        LAB_Image_Blend(vdst, vsrc);
        return;
    }

    LAB_ASSERT(vdst.w == vsrc.w);
    LAB_ASSERT(vdst.h == vsrc.h);

    LAB_Color* dst = vdst.data;
    const LAB_Color* src = vsrc.data;
    for(size_t y = 0; y < vdst.h; ++y, dst+=vdst.stride, src+=vsrc.stride)
        LAB_MemBltColor2(dst, src, black_tint, white_tint, vdst.w);
}

void LAB_Image_Tint2(LAB_ImageView vdst, LAB_ImageCView vsrc, LAB_Color black_tint, LAB_Color white_tint)
{
    if((black_tint & LAB_COL_MASK) == 0 && white_tint == LAB_RGBI(0xffffff))
    {
        LAB_Image_Copy(vdst, vsrc);
        return;
    }

    LAB_ASSERT(vdst.w == vsrc.w);
    LAB_ASSERT(vdst.h == vsrc.h);

    LAB_Color* dst = vdst.data;
    const LAB_Color* src = vsrc.data;
    for(size_t y = 0; y < vdst.h; ++y, dst+=vdst.stride, src+=vsrc.stride)
        LAB_MemTintColor2(dst, src, black_tint, white_tint, vdst.w);
}


void LAB_Image_MarkNoColor(LAB_ImageView vdst)
{
    if(vdst.stride == vdst.w)
    {
        LAB_MemNoColor(vdst.data, vdst.w*vdst.h);
        return;
    }

    LAB_Color* dst = vdst.data;
    for(size_t y = 0; y < vdst.h; ++y, dst+=vdst.stride)
        LAB_MemNoColor(dst, vdst.w);
}

void LAB_Image_MulColor_Fast(LAB_ImageView vdst, LAB_Color color)
{
    if(vdst.stride == vdst.w)
    {
        LAB_MemMulColor_Fast(vdst.data, color, vdst.w*vdst.h);
        return;
    }

    LAB_Color* dst = vdst.data;
    for(size_t y = 0; y < vdst.h; ++y, dst+=vdst.stride)
        LAB_MemMulColor_Fast(dst, color, vdst.w);
}
