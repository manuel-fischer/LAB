#pragma once

#include "LAB_attr.h"

#include "LAB/gui.h"
#include "LAB/gui/manager.h"
#include "LAB_stdinc.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>

#define LAB_GUI_COMPONENT_INHERIT \
    bool (*on_event)(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event); \
    void (*render)(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf, int x, int y); \
    void (*destroy)(LAB_GuiComponent* self); \
    int x, y, w, h


struct LAB_GuiComponent
{
    LAB_GUI_COMPONENT_INHERIT;
};

bool LAB_Gui_OnEvent_Ignore(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);
void LAB_GuiComponent_Destroy_Ignore(LAB_GuiComponent* self);


bool LAB_GetMouseCoordPtr(SDL_Event* event, int** x, int** y);

LAB_INLINE bool LAB_GuiHitTest(LAB_GuiComponent* component, int x, int y)
{
    x -= component->x;
    y -= component->y;
    return 0 <= x && x < component->w
        && 0 <= y && y < component->h;
}


