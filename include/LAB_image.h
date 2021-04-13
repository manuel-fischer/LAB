#pragma once

#include <SDL2/SDL_surface.h>
#include "LAB_color.h"
#include "LAB_stdinc.h"

void LAB_MemSetColor(LAB_Color* dst, LAB_Color color, size_t size);
void LAB_MemCpyColor(LAB_Color* dst, LAB_Color const* src, size_t size);

// working with the correct format
SDL_Surface* LAB_ImageLoad(const char* filename);


bool LAB_ImageSaveData(const char* filename, size_t w, size_t h, LAB_Color* data);
