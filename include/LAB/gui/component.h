#pragma once

#include "LAB_attr.h"

#include "LAB/gui.h"
#include "LAB/gui/manager.h"
#include "LAB_stdinc.h"
#include "LAB_vec2.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>

// export from LAB/gui/manager.h

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

LAB_INLINE
void LAB_GuiComponent_SetRect(void* component, LAB_Box2I rect)
{
    LAB_GuiComponent* c = (LAB_GuiComponent*)component;
    LAB_Box2I_Unpack_Sized(&c->x, &c->y, &c->w, &c->h, rect);
}