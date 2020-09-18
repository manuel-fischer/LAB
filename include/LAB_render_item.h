#pragma once

#include <SDL2/SDL_video.h>
#include "LAB_block.h"

#define LAB_ITEM_SIZE 32

SDL_Surface* LAB_RenderBlock2D(LAB_Block* block);
