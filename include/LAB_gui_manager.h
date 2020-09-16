#pragma once

#include "LAB_gui.h"

typedef struct LAB_GuiManager
{
    LAB_GuiComponent* component;
    unsigned gl_id;
} LAB_GuiManager;

void LAB_GuiManager_Create(LAB_GuiManager* manager);
void LAB_GuiManager_Destroy(LAB_GuiManager* manager);

void LAB_GuiManager_ShowDialog(LAB_GuiManager* manager, LAB_GuiComponent* component);
void LAB_GuiManager_Dismiss(LAB_GuiManager* manager);

void LAB_GuiManager_Render(LAB_GuiManager* manager, int w, int h);

int LAB_GuiManager_HandleEvent(LAB_GuiManager* manager, SDL_Event* event);
