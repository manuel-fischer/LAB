#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_GuiTextBox
{
    LAB_GUI_COMPONENT_INHERIT;
    size_t content_capacity;
    char*  content;
    SDL_Surface* text_surf;

    int state;
} LAB_GuiTextBox;

void LAB_GuiTextBox_Create(LAB_GuiTextBox* cself,
                           int x, int y, int w, int h);

bool LAB_GuiTextBox_SetContent(LAB_GuiTextBox* cself, const char* txt);
