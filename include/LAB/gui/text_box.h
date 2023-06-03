#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>
#include "LAB_vec2.h"

typedef struct LAB_GuiTextBox
{
    LAB_GUI_COMPONENT_INHERIT;
    size_t content_capacity;
    char*  content;
    SDL_Surface* text_surf;

    int state;
} LAB_GuiTextBox;

typedef struct LAB_GuiTextBox_Spec
{
    LAB_Box2I rect;
} LAB_GuiTextBox_Spec;

void LAB_GuiTextBox_Create(LAB_GuiTextBox* cself, LAB_GuiTextBox_Spec spec);

bool LAB_GuiTextBox_SetContent(LAB_GuiTextBox* cself, const char* txt);
