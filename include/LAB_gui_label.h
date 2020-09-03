#pragma once

#include "LAB_gui_component.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_GuiLabel
{
    LAB_GUI_COMPONENT_INHERIT;
    const char* title;
    SDL_Surface* text_surf;
} LAB_GuiLabel;

void LAB_GuiLabel_Create(LAB_GuiLabel* button,
                         int x, int y, int w, int h,
                         const char* title);

void LAB_GuiLabel_Render(LAB_GuiComponent* self, SDL_Surface* surf,
                         int x, int y);
