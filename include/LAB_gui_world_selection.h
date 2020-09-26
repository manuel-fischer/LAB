#pragma once

#include "LAB_gui_container.h"
#include "LAB_gui_label.h"
#include "LAB_gui_button.h"

typedef struct LAB_World LAB_World; // Forward

typedef void (*LAB_WorldSelectProc)(void* user, const char* world_filename);

typedef LAB_GuiButton LAB_GuiList, LAB_GuiTextBox;

typedef struct LAB_GuiWorldSelection
{
    LAB_GUI_CONTAINER_INHERIT;
    LAB_GuiComponent* components_arr[4+1];

    LAB_GuiLabel   lblTitle;
    LAB_GuiList    lstWorlds;
    LAB_GuiTextBox txtWorld;
    LAB_GuiButton  cmdOK;

    LAB_WorldSelectProc on_select;
    void* on_select_user;
} LAB_GuiWorldSelection;


void LAB_GuiWorldSelection_Create(LAB_GuiWorldSelection* gui, const char* title, LAB_WorldSelectProc on_select);
