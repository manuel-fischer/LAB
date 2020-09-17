#include "LAB_gui_inventory.h"

void LAB_GuiInventory_Create(LAB_GuiInventory* gui)
{
    gui->x = 0;
    gui->y = 0;
    gui->w = 200;
    gui->h = 155;

    gui->on_event = &LAB_GuiContainer_OnEvent;
    gui->render = &LAB_GuiContainer_Render_Framed;
    gui->current = NULL; // TODO call LAB_GuiContainer_Create

    gui->components = gui->components_arr;
    gui->components_arr[0] = (LAB_GuiComponent*)&gui->lblTitle;
    gui->components_arr[1] = NULL;

    LAB_GuiLabel_Create(&gui->lblTitle,
                        10, 5, 180, 25,
                        "Inventory");
}
