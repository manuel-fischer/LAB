#pragma once

#include "LAB/gui/component.h"
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

void LAB_GuiLabel_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                         int x, int y);

void LAB_GuiLabel_Destroy(LAB_GuiComponent* self);
