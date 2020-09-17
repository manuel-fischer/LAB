#pragma once

#include "LAB_gui_label.h"
#include "LAB_gui_button.h"

typedef struct LAB_GuiInventory
{
    LAB_GUI_CONTAINER_INHERIT;
    LAB_GuiComponent* components_arr[1+1];

    LAB_GuiLabel lblTitle;
} LAB_GuiInventory;


void LAB_GuiInventory_Create(LAB_GuiInventory* gui);
