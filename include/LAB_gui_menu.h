#pragma once

#include "LAB_gui_component.h"

typedef struct LAB_GuiMenu
{
    LAB_GUI_CONTAINER_INHERIT;
    //LAB_GuiButton cmdSave;
} LAB_GuiMenu;


void LAB_GuiMenu_Create(LAB_GuiMenu* menu);
