#include "LAB/gui.h"

#include <SDL2/SDL_image.h>
#include "LAB_sdl.h"

// TODO: move global state into struct
static SDL_Surface* gui = NULL;

Uint32 LAB_GuiEvent_base;

bool LAB_GuiInit(void)
{
    LAB_SDL_ALLOC(IMG_Load, &gui, "assets/gui.png");
    if(!gui) return 0;

    LAB_GuiEvent_base = SDL_RegisterEvents(LAB_GUI_EVENT_COUNT);
    if(LAB_GuiEvent_base == (Uint32)-1) return 0;

    return 1;
}

void LAB_GuiQuit(void)
{
    LAB_SDL_FREE(SDL_FreeSurface, &gui);
}

void LAB_RenderRect(SDL_Surface* surf, int scale, int x, int y, int w, int h, int i, int j)
{
    SDL_Rect src, dst;

    const int S = 16; // Size of cell
    const int B = 6;  // Size of border
    const int dB = scale*B;

    for(int b = 0; b < 3; ++b)
    {
        src.y = j*16 + (b==0?0:b==1?B:S-B);
        src.h = b==1?S-2*B:B;

        dst.y = y + (b==0?0:b==1?dB:h-dB);
        dst.h = b==1?h-2*dB:dB;

        for(int a = 0; a < 3; ++a)
        {
            src.x = i*16 + (a==0?0:a==1?B:S-B);
            src.w = a==1?S-2*B:B;

            dst.x = x + (a==0?0:a==1?dB:w-dB);
            dst.w = a==1?w-2*dB:dB;

            SDL_BlitScaled(gui, &src, surf, &dst);
        }
    }
}
