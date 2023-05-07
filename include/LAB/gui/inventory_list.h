#pragma once

#include "LAB/gui/component.h"
#include "LAB_i_inventory.h"
#include "LAB_render_item.h"
#include "LAB_vec2.h"

#define LAB_SLOT_BORDER 4
#define LAB_SLOT_SIZE (2*LAB_SLOT_BORDER+LAB_ITEM_SIZE)

typedef struct LAB_Block LAB_Block;

typedef struct LAB_GuiInventoryList
{
    LAB_GUI_COMPONENT_INHERIT;
    size_t selected_slot;
    size_t topleft_slot;
    LAB_IInventory const* inventory;
    void* inventory_user;

} LAB_GuiInventoryList;

typedef struct LAB_GuiInventoryList_Spec
{
    LAB_Vec2I rect;

    LAB_IInventory const* inventory;
    void* inventory_user;

} LAB_GuiInventoryList_Spec;

void LAB_GuiInventoryList_Create(LAB_GuiInventoryList* lst,
                                 int x, int y, int w, int h,
                                 LAB_IInventory const* inventory,
                                 void* inventory_user);

void LAB_GuiInventoryList_Create_Cells(LAB_GuiInventoryList* lst,
                                 int x, int y, int columns, int rows,
                                 LAB_IInventory const* inventory,
                                 void* inventory_user);

void LAB_GuiInventoryList_Create_Columns(LAB_GuiInventoryList* lst,
                                 int x, int y, int columns,
                                 LAB_IInventory const* inventory,
                                 void* inventory_user);
