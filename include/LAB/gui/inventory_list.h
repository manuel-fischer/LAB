#pragma once

#include "LAB/gui/component.h"
#include "LAB_i_inventory.h"
#include "LAB_render_item.h"

#define LAB_SLOT_BORDER 4
#define LAB_SLOT_SIZE (2*LAB_SLOT_BORDER+LAB_ITEM_SIZE)

typedef struct LAB_Block LAB_Block;

typedef struct LAB_GuiInventoryList
{
    LAB_GUI_COMPONENT_INHERIT;
    size_t selected_slot;
    LAB_IInventory const* inventory;
    void* inventory_user;

} LAB_GuiInventoryList;

void LAB_GuiInventoryList_Create(LAB_GuiInventoryList* lst,
                                 int x, int y, int w, int h,
                                 LAB_IInventory const* inventory,
                                 void* inventory_user);

void LAB_GuiInventoryList_Create_Columns(LAB_GuiInventoryList* lst,
                                 int x, int y, int columns,
                                 LAB_IInventory const* inventory,
                                 void* inventory_user);

void LAB_GuiInventoryList_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf, int x, int y);
bool LAB_GuiInventoryList_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);