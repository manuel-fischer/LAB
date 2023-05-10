#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_GuiLabel
{
    LAB_GUI_COMPONENT_INHERIT;
    const char* title;
    SDL_Surface* text_surf;
} LAB_GuiLabel;


typedef struct LAB_GuiLabel_Spec
{
    LAB_Box2I rect;
    const char* title;
} LAB_GuiLabel_Spec;


void LAB_GuiLabel_Create(LAB_GuiLabel* label, LAB_GuiLabel_Spec spec);
