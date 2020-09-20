#pragma once

#include "LAB_gui_container.h"
#include "LAB_gui_label.h"
#include "LAB_gui_button.h"
#include "LAB_gui_inventory_list.h"

#include "LAB_i_inventory.h"

typedef struct LAB_GuiInventory
{
    LAB_GUI_CONTAINER_INHERIT;
    LAB_GuiComponent* components_arr[2+1];

    LAB_GuiLabel lblTitle;
    LAB_GuiInventoryList lstInventory;
} LAB_GuiInventory;


void LAB_GuiInventory_Create(LAB_GuiInventory* gui,
                             LAB_IInventory const* inventory,
                             void* inventory_user);
