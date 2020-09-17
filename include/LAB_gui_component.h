#pragma once

#include "LAB_gui.h"
#include "LAB_gui_manager.h"
#include "LAB_stdinc.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>

#define LAB_GUI_COMPONENT_INHERIT \
    /** \
     *  Return 1 if component got dirty and needs to be rerendered \
     */ \
    bool (*on_event)(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event); \
    void (*render)(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y); \
    int x, y, w, h
struct LAB_GuiComponent
{
    LAB_GUI_COMPONENT_INHERIT;
};



#define LAB_GUI_CONTAINER_INHERIT \
    LAB_GUI_COMPONENT_INHERIT; \
    LAB_GuiComponent** components; \
    LAB_GuiComponent*  current
typedef struct LAB_GuiContainer
{
    LAB_GUI_CONTAINER_INHERIT;
} LAB_GuiContainer;

bool LAB_Gui_OnEvent_Ignore(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);

bool LAB_GuiContainer_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);
void LAB_GuiContainer_Render(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y);

void LAB_GuiContainer_Render_Framed(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y);

bool LAB_GetMouseCoordPtr(SDL_Event* event, int** x, int** y);

static inline bool LAB_GuiHitTest(LAB_GuiComponent* component, int x, int y)
{
    x -= component->x;
    y -= component->y;
    return 0 <= x && x < component->w
        && 0 <= y && y < component->h;
}


