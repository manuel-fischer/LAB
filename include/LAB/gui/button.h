#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_GuiButton
{
    LAB_GUI_COMPONENT_INHERIT;
    const char* title;
    SDL_Surface* text_surf;

    void(*on_click)(void* ctx, LAB_GuiManager* mgr);
    void* ctx;

    int state;
} LAB_GuiButton;


typedef struct LAB_GuiButton_Spec
{
    LAB_Box2I rect;
    const char* title;
    void(*on_click)(void* ctx, LAB_GuiManager* mgr);
    void* ctx;
} LAB_GuiButton_Spec;


void LAB_GuiButton_Create(LAB_GuiButton* button, LAB_GuiButton_Spec spec);
