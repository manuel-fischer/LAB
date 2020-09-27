#pragma once

#include "LAB_gui_component.h"
#include <SDL2/SDL_surface.h>

typedef struct LAB_GuiTextBox
{
    LAB_GUI_COMPONENT_INHERIT;
    size_t content_capacity;
    char*  content;
    SDL_Surface* text_surf;

    int state;
} LAB_GuiTextBox;

enum LAB_GuiTextBoxState
{
    LAB_GUI_TEXT_BOX_NORMAL = 0,
    LAB_GUI_TEXT_BOX_FOCUSED,
};

void LAB_GuiTextBox_Create(LAB_GuiTextBox* cself,
                          int x, int y, int w, int h);

void LAB_GuiTextBox_Render(LAB_GuiComponent* self, SDL_Surface* surf,
                          int x, int y);

bool LAB_GuiTextBox_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);


void LAB_GuiTextBox_Destroy(LAB_GuiComponent* self);
