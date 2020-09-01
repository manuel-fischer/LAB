#include "LAB_gui.h"

#include <SDL2/SDL_image.h>

static SDL_Surface* gui;

int LAB_GuiInit()
{
    gui = IMG_Load("assets/gui.png");
    if(!gui) return 0;

    return 1;
}

void LAB_GuiQuit()
{
    SDL_FreeSurface(gui);
}

void LAB_RenderRect(SDL_Surface* surf, int x, int y, int w, int h, int i, int j)
{
    SDL_Rect src, dst;

    src.x = i*16;
    src.y = j*16;
    src.w = 16;
    src.h = 16;

    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;

    SDL_BlitScaled(gui, &src, surf, &dst);
}
