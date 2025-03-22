#pragma once

#include <SDL2/SDL_surface.h>
#include "LAB_color.h"
#include "LAB_stdinc.h"
#include "LAB_vec2.h"

// export from LAB_color.h
// export LAB_Mem*Color*
// export LAB_ImageData2SDL
// export LAB_MakeMipmap2D
// export LAB_Fix0Alpha
// export LAB_Image*

void LAB_MemSetColor(LAB_Color* dst, LAB_Color color, size_t size);
void LAB_MemCpyColor(LAB_Color* dst, LAB_Color const* src, size_t size);
void LAB_MemBlendColor(LAB_Color* dst, LAB_Color const* src, size_t size);
void LAB_MemBltColor(LAB_Color* dst, LAB_Color const* src, LAB_Color tint, size_t size);
void LAB_MemBltColor2(LAB_Color* dst, LAB_Color const* src, LAB_Color black_tint, LAB_Color white_tint, size_t size);
void LAB_MemTintColor2(LAB_Color* dst, LAB_Color const* src, LAB_Color black_tint, LAB_Color white_tint, size_t size);
void LAB_MemNoColor(LAB_Color* dst, size_t size);
void LAB_MemMulColor_Fast(LAB_Color* dst, LAB_Color color, size_t size);

// working with the correct format: 32 Bit RGBA
SDL_Surface* LAB_ImageLoad(const char* filename);
SDL_Surface* LAB_ImageLoad_Fmt(const char* fname_fmt, ...);


bool LAB_ImageSave(size_t w, size_t h, LAB_Color* data, const char* filename);
void LAB_ImageSave_Fmt(size_t w, size_t h, LAB_Color* data, const char* fname_fmt, ...);

SDL_Surface* LAB_ImageData2SDL(size_t w, size_t h, LAB_Color* data);

// alpha corrected mip maps
void LAB_MakeMipmap2D(size_t w, size_t h, const LAB_Color* in_data, LAB_OUT LAB_Color* out_data);
void LAB_Fix0Alpha(size_t w, size_t h, LAB_Color* data, size_t stride);



typedef struct LAB_ImageView
{
    size_t w, h;
    LAB_Color* data;
    size_t stride;
} LAB_ImageView, LAB_ImageCView;

#define LAB_ImageView_IsOk(v) (!!(v).w)

LAB_INLINE LAB_ImageView LAB_ImageView_Create(size_t w, size_t h, LAB_Color* data)
{
    return (LAB_ImageView) { w, h, data, w };
}

LAB_INLINE LAB_ImageView LAB_ImageView_CreateSDL(SDL_Surface* surf)
{
    LAB_ASSERT(surf->format->format == SDL_PIXELFORMAT_RGBA32);
    LAB_ASSERT((surf->pitch & 3) == 0);
    return (LAB_ImageView) { surf->w, surf->h, (LAB_Color*)surf->pixels, surf->pitch/4 };
}


LAB_INLINE LAB_ImageView LAB_ImageView_Clip(LAB_ImageView v, LAB_Box2Z clip)
{
    LAB_ASSERT(clip.a.x <  v.w);
    LAB_ASSERT(clip.b.x <= v.w);
    LAB_ASSERT(clip.a.y <  v.h);
    LAB_ASSERT(clip.b.y <= v.h);
    LAB_ASSERT(clip.a.x < clip.b.x);
    LAB_ASSERT(clip.a.y < clip.b.y);

    size_t offset = v.stride * clip.a.y + clip.a.x;
    size_t w = LAB_Box2Z_DX(clip);
    size_t h = LAB_Box2Z_DY(clip);
    return (LAB_ImageView) { w, h, v.data+offset, v.stride };
}



void LAB_Image_FillColor(LAB_ImageView vdst, LAB_Color color);
void LAB_Image_Copy(LAB_ImageView vdst, LAB_ImageCView vsrc);
void LAB_Image_Blend(LAB_ImageView vdst, LAB_ImageCView vsrc);
void LAB_Image_Blit(LAB_ImageView vdst, LAB_ImageCView vsrc, LAB_Color tint);
void LAB_Image_Blit2(LAB_ImageView vdst, LAB_ImageCView vsrc, LAB_Color black_tint, LAB_Color white_tint);
void LAB_Image_Tint2(LAB_ImageView vdst, LAB_ImageCView vsrc, LAB_Color black_tint, LAB_Color white_tint);
void LAB_Image_MarkNoColor(LAB_ImageView vdst);
void LAB_Image_MulColor_Fast(LAB_ImageView vdst, LAB_Color color);

LAB_INLINE
void LAB_Image_SetPixel(LAB_ImageView vdst, size_t x, size_t y, LAB_Color color)
{
    LAB_ASSERT(x < vdst.w);
    LAB_ASSERT(y < vdst.h);
    vdst.data[vdst.stride*y + x] = color;
}