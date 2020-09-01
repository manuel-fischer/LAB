#pragma once

#include <SDL2/SDL.h>

/**
 *  Return nonzero on success
 */
int LAB_GuiInit();
void LAB_GuiQuit();

void LAB_RenderRect(SDL_Surface* surf, int x, int y, int w, int h, int i, int j);
