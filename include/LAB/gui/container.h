#pragma once

#include "LAB/gui/component.h"

#define LAB_GUI_CONTAINER_INHERIT \
    LAB_GUI_COMPONENT_INHERIT; \
    LAB_GuiComponent** components; \
    LAB_GuiComponent*  current
typedef struct LAB_GuiContainer
{
    LAB_GUI_CONTAINER_INHERIT;
} LAB_GuiContainer;

bool LAB_GuiContainer_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);

void LAB_GuiContainer_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf, int x, int y);
void LAB_GuiContainer_Render_Framed(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf, int x, int y);

void LAB_GuiContainer_Destroy(LAB_GuiComponent* self);
