#include "LAB_gui_inventory.h"

#define LAB_INVENTORY_COLUMNS 10

void LAB_GuiInventory_Create(LAB_GuiInventory* gui,
                             LAB_IInventory const* inventory,
                             void* inventory_user)
{
    gui->x = 0;
    gui->y = 0;
    gui->w = 20+LAB_INVENTORY_COLUMNS*LAB_SLOT_SIZE;
    //gui->h = 155;

    gui->on_event = &LAB_GuiContainer_OnEvent;
    gui->render = &LAB_GuiContainer_Render_Framed;
    gui->destroy = &LAB_GuiContainer_Destroy;
    gui->current = NULL; // TODO call LAB_GuiContainer_Create

    gui->components = gui->components_arr;
    gui->components_arr[0] = (LAB_GuiComponent*)&gui->lblTitle;
    gui->components_arr[1] = (LAB_GuiComponent*)&gui->lstInventory;
    gui->components_arr[2] = NULL;

    LAB_GuiLabel_Create(&gui->lblTitle,
                        10, 5, LAB_INVENTORY_COLUMNS*LAB_SLOT_SIZE, 25,
                        "Inventory");
    LAB_GuiInventoryList_Create_Columns(&gui->lstInventory,
                        10, 30, LAB_INVENTORY_COLUMNS,
                        inventory, inventory_user);

    gui->h = 40+gui->lstInventory.h;
}
