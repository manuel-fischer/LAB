#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/**
 *  Return nonzero on success
 */
int LAB_GuiInit(void);
void LAB_GuiQuit(void);

void LAB_RenderRect(SDL_Surface* surf, int x, int y, int w, int h, int i, int j);


TTF_Font* LAB_GuiFont(void);
TTF_Font* LAB_GuiTitleFont(void);