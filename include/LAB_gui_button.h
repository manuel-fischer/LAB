#pragma once

#include "LAB_gui_component.h"
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

enum LAB_GuiButtonState
{
    LAB_GUI_BUTTON_NORMAL = 0,
    LAB_GUI_BUTTON_FOCUSED,
    LAB_GUI_BUTTON_PRESSED,
};

void LAB_GuiButton_Create(LAB_GuiButton* button,
                          int x, int y, int w, int h,
                          const char* title,
                          void(*on_click)(void* user, LAB_GuiManager* mgr),
                          void* user);

void LAB_GuiButton_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                          int x, int y);


bool LAB_GuiButton_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);

void LAB_GuiButton_Destroy(LAB_GuiComponent* self);
