#include "LAB_gui_inventory_list.h"

#include "LAB_render_item.h"

void LAB_GuiInventoryList_Render(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y);
bool LAB_GuiInventoryList_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);

void LAB_GuiInventoryList_Create(LAB_GuiInventoryList* lst,
                                 int x, int y, int w, int h,
                                 LAB_IInventory const* inventory,
                                 void* inventory_user)
{
    lst->x = x; lst->y = y;
    lst->w = w; lst->h = h;

    lst->on_event = &LAB_GuiInventoryList_OnEvent;
    lst->render = &LAB_GuiInventoryList_Render;

    lst->inventory = inventory;
    lst->inventory_user = inventory_user;
}

void LAB_GuiInventoryList_Create_Columns(LAB_GuiInventoryList* lst,
                                 int x, int y, int columns,
                                 LAB_IInventory const* inventory,
                                 void* inventory_user)
{
    int rows = (inventory->get_size(inventory_user)+columns-1)/columns;
    int w = columns*LAB_SLOT_SIZE;
    int h = rows*LAB_SLOT_SIZE;
    LAB_GuiInventoryList_Create(lst, x, y, w, h, inventory, inventory_user);
}

void LAB_GuiInventoryList_Render(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y)
{
    LAB_GuiInventoryList* cself = (LAB_GuiInventoryList*)self;

    size_t i = 0;
    size_t inv_size = cself->inventory->get_size(cself->inventory_user);
    int cols = cself->w/LAB_SLOT_SIZE;
    int rows = cself->h/LAB_SLOT_SIZE;
    for(int yi = 0; yi < rows; ++yi)
    for(int xi = 0; xi < cols; ++xi)
    {
        if(i >= inv_size) return;
        int cx, cy;
        cx = x+xi*LAB_SLOT_SIZE;
        cy = y+yi*LAB_SLOT_SIZE;
        LAB_RenderRect(surf, cx, cy, LAB_SLOT_SIZE, LAB_SLOT_SIZE, 0, 0);
        LAB_Block* b = cself->inventory->get_slot(cself->inventory_user, i);
        SDL_Surface* bsurf = LAB_RenderBlock2D(b);

        SDL_Rect dst_rect;
        dst_rect.x = cx+LAB_SLOT_BORDER;
        dst_rect.y = cy+LAB_SLOT_BORDER;
        dst_rect.w = dst_rect.h = LAB_ITEM_SIZE;

        //SDL_BlitSurface(bsurf, NULL, surf, &dst_rect);
        SDL_BlitScaled(bsurf, NULL, surf, &dst_rect);
        SDL_FreeSurface(bsurf);
        ++i;
    }
}

bool LAB_GuiInventoryList_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiInventoryList* cself = (LAB_GuiInventoryList*)self;
    int cols = cself->w/LAB_SLOT_SIZE;
    int rows = cself->h/LAB_SLOT_SIZE;
    size_t slot = -1;
    int* mx,* my;
    if(LAB_GetMouseCoordPtr(event, &mx, &my))
    {
        int col = *mx/LAB_SLOT_SIZE;
        int row = *my/LAB_SLOT_SIZE;
        if(col >= 0 && col < cols &&
           row >= 0 && row < rows)
            slot = col + row*cols;
    }
    switch(event->type)
    {
        case SDL_MOUSEBUTTONUP:
        {
            if(slot < cself->inventory->get_size(cself->inventory_user))
            {
                cself->inventory->take_slot(cself->inventory_user, slot);
                LAB_GuiManager_Dismiss(mgr);
            }
            return 1;
        } break;
    }
    return 0;
}
