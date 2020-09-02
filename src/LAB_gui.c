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

    const int S = 16; // Size of cell
    const int B = 6;  // Size of border

    for(int b = 0; b < 3; ++b)
    {
        src.y = j*16 + (b==0?0:b==1?B:S-B);
        src.h = b==1?S-2*B:B;

        dst.y = y + (b==0?0:b==1?B:h-B);
        dst.h = b==1?h-2*B:B;

        for(int a = 0; a < 3; ++a)
        {
            src.x = i*16 + (a==0?0:a==1?B:S-B);
            src.w = a==1?S-2*B:B;

            dst.x = y + (a==0?0:a==1?B:w-B);
            dst.w = a==1?w-2*B:B;

            SDL_BlitScaled(gui, &src, surf, &dst);
        }
    }
}
