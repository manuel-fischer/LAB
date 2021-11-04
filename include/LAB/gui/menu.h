#pragma once

#include "LAB/gui/container.h"
#include "LAB/gui/label.h"
#include "LAB/gui/button.h"

typedef struct LAB_World LAB_World; // Forward

typedef struct LAB_GuiMenu
{
    LAB_GUI_CONTAINER_INHERIT;
    LAB_GuiComponent* components_arr[6+1];

    LAB_GuiLabel  lblTitle;
    LAB_GuiButton cmdResume;
    LAB_GuiButton cmdSave;
    LAB_GuiButton cmdSaveAs;
    LAB_GuiButton cmdLoad;
    LAB_GuiButton cmdQuit;
} LAB_GuiMenu;


void LAB_GuiMenu_Create(LAB_GuiMenu* menu, LAB_World* world);
