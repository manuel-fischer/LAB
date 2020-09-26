#include "LAB_gui_world_selection.h"

static void LAB_GuiWorldSelection_OK(void* user, LAB_GuiManager* mgr);

void LAB_GuiWorldSelection_Create(LAB_GuiWorldSelection* gui, const char* title, LAB_WorldSelectProc on_select)
{
    gui->x = 0;
    gui->y = 0;
    gui->w = 220;
    gui->h = 185;

    gui->on_event = &LAB_GuiContainer_OnEvent;
    gui->render = &LAB_GuiContainer_Render_Framed;
    gui->current = NULL; // TODO call LAB_GuiContainer_Create

    gui->components = gui->components_arr;
    gui->components_arr[0] = (LAB_GuiComponent*)&gui->lblTitle;
    gui->components_arr[1] = (LAB_GuiComponent*)&gui->lstWorlds;
    gui->components_arr[2] = (LAB_GuiComponent*)&gui->txtWorld;
    gui->components_arr[3] = (LAB_GuiComponent*)&gui->cmdOK;
    gui->components_arr[4] = NULL;


    LAB_GuiLabel_Create(&gui->lblTitle,
                        10, 5, 200, 25,
                        title);

    LAB_GuiButton_Create(&gui->lstWorlds,
                         10, 30, 200, 115,
                         "lstWorlds", &LAB_GuiWorldSelection_OK, NULL);

    LAB_GuiButton_Create(&gui->txtWorld,
                         10, 150, 140, 25,
                         "txtWorld", &LAB_GuiWorldSelection_OK, NULL);

    LAB_GuiButton_Create(&gui->cmdOK,
                         155, 150, 55, 25,
                         "OK", &LAB_GuiWorldSelection_OK, NULL);
}

static void LAB_GuiWorldSelection_OK(void* user, LAB_GuiManager* mgr)
{
}
