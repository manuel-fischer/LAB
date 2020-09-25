#pragma once

#include <SDL2/SDL_surface.h>

#define LAB_TILE_SIZE 32

SDL_Surface* LAB_block_terrain;
unsigned     LAB_block_terrain_gl_id;

void LAB_InitAssets(void);
void LAB_QuitAssets(void);
