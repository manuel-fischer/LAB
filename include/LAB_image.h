#pragma once

#include <SDL2/SDL_surface.h>
#include "LAB_color.h"
#include "LAB_stdinc.h"

void LAB_MemSetColor(LAB_Color* dst, LAB_Color color, size_t size);
void LAB_MemCpyColor(LAB_Color* dst, LAB_Color const* src, size_t size);
void LAB_MemBltColor(LAB_Color* dst, LAB_Color const* src, LAB_Color tint, size_t size);
void LAB_MemBltColor2(LAB_Color* dst, LAB_Color const* src, LAB_Color black_tint, LAB_Color white_tint, size_t size);
void LAB_MemNoColor(LAB_Color* dst, size_t size);

// working with the correct format: 32 Bit RGBA
SDL_Surface* LAB_ImageLoad(const char* filename);
SDL_Surface* LAB_ImageLoad_Fmt(const char* fname_fmt, ...);


bool LAB_ImageSave(size_t w, size_t h, LAB_Color* data, const char* filename);
void LAB_ImageSave_Fmt(size_t w, size_t h, LAB_Color* data, const char* fname_fmt, ...);

SDL_Surface* LAB_ImageData2SDL(size_t w, size_t h, LAB_Color* data);

// alpha corrected mip maps
void LAB_MakeMipmap2D(size_t w, size_t h, const LAB_Color* in_data, LAB_OUT LAB_Color* out_data);
void LAB_Fix0Alpha(size_t w, size_t h, LAB_Color* data);
