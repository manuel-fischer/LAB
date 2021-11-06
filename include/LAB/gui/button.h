#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_GuiButton
{
    LAB_GUI_COMPONENT_INHERIT;
    const char* title;
    SDL_Surface* text_surf;

    void(*on_click)(void* user, LAB_GuiManager* mgr);
    void* user;

    int state;
} LAB_GuiButton;


void LAB_GuiButton_Create(LAB_GuiButton* button,
                          int x, int y, int w, int h,
                          const char* title,
                          void(*on_click)(void* user, LAB_GuiManager* mgr),
                          void* user);
