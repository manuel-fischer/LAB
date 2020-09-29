#pragma once

#include "LAB_stdinc.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct LAB_GuiComponent LAB_GuiComponent;

enum LAB_GuiEvent
{
    LAB_GUI_EVENT_FOCUS,
    LAB_GUI_EVENT_UNFOCUS,
    LAB_GUI_EVENT_DROPITEM,
    LAB_GUI_EVENT_COUNT
};
extern Uint32 LAB_GuiEvent_base;
// unsigned substraction -> values less than LAB_GuiEvent_base map to large values
#define LAB_GUI_EVENT(sdl_event) ((Uint32)(sdl_event)-LAB_GuiEvent_base)
#define LAB_IS_GUI_EVENT(sdl_event) (LAB_GUI_EVENT(sdl_event)-LAB_GUI_EVENT_COUNT)
#define LAB_GUI_EVENT2SDL(lab_event) ((Uint32)(lab_event)+LAB_GuiEvent_base)

/**
 *  Return 1 on success
 */
bool LAB_GuiInit(void);
void LAB_GuiQuit(void);

void LAB_RenderRect(SDL_Surface* surf, int scale, int x, int y, int w, int h, int i, int j);


/*TTF_Font* LAB_GuiFont(void);
TTF_Font* LAB_GuiTitleFont(void);
TTF_Font* LAB_GuiMonospaceFont(void);*/
