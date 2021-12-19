#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_GuiLabel
{
    LAB_GUI_COMPONENT_INHERIT;
    const char* title;
    SDL_Surface* text_surf;
} LAB_GuiLabel;

void LAB_GuiLabel_Create(LAB_GuiLabel* label,
                         int x, int y, int w, int h,
                         const char* title);