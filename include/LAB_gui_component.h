#pragma once

#include "LAB_gui.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>

#define LAB_GUI_COMPONENT_INHERIT \
    /** \
     *  Return 1 if component got dirty and needs to be rerendered \
     */ \
    int  (*on_event)(LAB_GuiComponent* self, SDL_Event* event); \
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
int  LAB_GuiContainer_OnEvent(LAB_GuiComponent* self, SDL_Event* event);
void LAB_GuiContainer_Render(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y);

void LAB_GuiContainer_Render_Framed(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y);

int LAB_GetMouseCoordPtr(SDL_Event* event, int** x, int** y);

static inline int LAB_GuiHitTest(LAB_GuiComponent* component, int x, int y)
{
    x -= component->x;
    y -= component->y;
    return 0 <= x && x < component->w
        && 0 <= y && y < component->h;
}


