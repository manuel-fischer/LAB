#pragma once

#include "LAB/gui/container.h"
#include "LAB/gui/label.h"
#include "LAB/gui/button.h"
#include "LAB/gui/list_box.h"
#include "LAB/gui/text_box.h"

typedef struct LAB_World LAB_World; // Forward

typedef void (*LAB_WorldSelectProc)(void* user, const char* world_filename);

typedef LAB_GuiButton LAB_GuiListBox_, LAB_GuiTextBox_;

typedef struct LAB_GuiWorldSelection
{
    LAB_GUI_CONTAINER_INHERIT;
    LAB_GuiComponent* components_arr[4+1];

    LAB_GuiLabel   lblTitle;
    LAB_GuiListBox lstWorlds;
    LAB_GuiTextBox txtWorld;
    LAB_GuiButton  cmdOK;

    size_t world_count;
    char** worlds;

    LAB_WorldSelectProc on_select;
    void* on_select_user;
} LAB_GuiWorldSelection;


void LAB_GuiWorldSelection_Create(LAB_GuiWorldSelection* gui, const char* title, LAB_WorldSelectProc on_select);
void LAB_GuiWorldSelection_Destroy(LAB_GuiComponent* self);
