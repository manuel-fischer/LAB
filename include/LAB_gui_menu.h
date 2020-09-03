#pragma once

#include "LAB_gui_component.h"
#include "LAB_gui_label.h"
#include "LAB_gui_button.h"

typedef struct LAB_GuiMenu
{
    LAB_GUI_CONTAINER_INHERIT;
    LAB_GuiComponent* components_arr[4+1];

    LAB_GuiLabel  lblMenu;
    LAB_GuiButton cmdSave;
    LAB_GuiButton cmdLoad;
    LAB_GuiButton cmdQuit;
} LAB_GuiMenu;


void LAB_GuiMenu_Create(LAB_GuiMenu* menu);
