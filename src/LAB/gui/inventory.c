#include "LAB/gui/inventory.h"

#define LAB_INVENTORY_ROWS 6
#define LAB_INVENTORY_COLUMNS 10

LAB_STATIC
bool LAB_GuiInventory_OnScroll(void* ctx, int scroll_value)
{
    LAB_GuiInventory* gui = ctx;

    gui->lstInventory.topleft_slot = scroll_value*LAB_INVENTORY_COLUMNS;

    return true;
}

LAB_STATIC
bool LAB_GuiInventory_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiInventory* cself = (LAB_GuiInventory*)self;

    if(event->type == SDL_MOUSEWHEEL)
    {
        int new_scroll = cself->scrInventory.scroll_value - event->wheel.y;

        return LAB_GuiVScroll_SetScrollValue(&cself->scrInventory, new_scroll);
    }

    return LAB_GuiContainer_OnEvent(self, mgr, event);
}

void LAB_GuiInventory_Create(LAB_GuiInventory* gui,
                             LAB_IInventory const* inventory,
                             void* inventory_user)
{
    int list_width = LAB_INVENTORY_COLUMNS*LAB_SLOT_SIZE;
    int list_height = LAB_INVENTORY_ROWS*LAB_SLOT_SIZE;
    int scroll_width = 14;

    gui->x = 0;
    gui->y = 0;
    gui->w = 20+list_width+4+scroll_width;
    gui->h = 40+list_height;

    gui->on_event = &LAB_GuiInventory_OnEvent;
    gui->render = &LAB_GuiContainer_Render_Framed;
    gui->destroy = &LAB_GuiContainer_Destroy;
    gui->current = NULL; // TODO call LAB_GuiContainer_Create

    gui->components = gui->components_arr;
    gui->components_arr[0] = (LAB_GuiComponent*)&gui->lblTitle;
    gui->components_arr[1] = (LAB_GuiComponent*)&gui->lstInventory;
    gui->components_arr[2] = (LAB_GuiComponent*)&gui->scrInventory;
    gui->components_arr[3] = NULL;

    LAB_GuiLabel_Create(&gui->lblTitle, (LAB_GuiLabel_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 5, LAB_INVENTORY_COLUMNS*LAB_SLOT_SIZE, 25),
        .title = "Inventory",
    });

    LAB_GuiInventoryList_Create_Cells(&gui->lstInventory,
                        10, 30, LAB_INVENTORY_COLUMNS, LAB_INVENTORY_ROWS,
                        inventory, inventory_user);

    LAB_GuiVScroll_Create(&gui->scrInventory, (LAB_GuiScroll_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10+gui->lstInventory.w+4, 30, scroll_width, gui->lstInventory.h),
        .total = LAB_CeilDiv(inventory->get_size(inventory_user), LAB_INVENTORY_COLUMNS),
        .viewport = LAB_INVENTORY_ROWS,
        .on_scroll = LAB_GuiInventory_OnScroll,
        .on_scroll_ctx = gui,
    });

}
