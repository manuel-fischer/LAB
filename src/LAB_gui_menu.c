#include "LAB_gui_menu.h"

#include "LAB_world.h"
#include "LAB_gui_world_selection.h"
#include "LAB_memory.h"

static void LAB_GuiMenu_Resume(void* user, LAB_GuiManager* mgr);
static void LAB_GuiMenu_Save(void* user, LAB_GuiManager* mgr);
static void LAB_GuiMenu_SaveAs(void* user, LAB_GuiManager* mgr);
static void LAB_GuiMenu_Load(void* user, LAB_GuiManager* mgr);
static void LAB_GuiMenu_Quit(void* user, LAB_GuiManager* mgr);

void LAB_GuiMenu_Create(LAB_GuiMenu* menu, LAB_World* world)
{
    menu->x = 0;
    menu->y = 0;
    menu->w = 120;
    menu->h = 185;

    menu->on_event = &LAB_GuiContainer_OnEvent;
    menu->render = &LAB_GuiContainer_Render_Framed;
    menu->current = NULL; // TODO call LAB_GuiContainer_Create

    menu->components = menu->components_arr;
    menu->components_arr[0] = (LAB_GuiComponent*)&menu->lblTitle;
    menu->components_arr[1] = (LAB_GuiComponent*)&menu->cmdResume;
    menu->components_arr[2] = (LAB_GuiComponent*)&menu->cmdSave;
    menu->components_arr[3] = (LAB_GuiComponent*)&menu->cmdSaveAs;
    menu->components_arr[4] = (LAB_GuiComponent*)&menu->cmdLoad;
    menu->components_arr[5] = (LAB_GuiComponent*)&menu->cmdQuit;
    menu->components_arr[6] = NULL;


    LAB_GuiLabel_Create(&menu->lblTitle,
                        10, 5, 100, 25,
                        "Menu");

    LAB_GuiButton_Create(&menu->cmdResume,
                         10, 30, 100, 25,
                         "Resume", &LAB_GuiMenu_Resume, NULL);

    LAB_GuiButton_Create(&menu->cmdSave,
                         10, 60, 100, 25,
                         "Save", &LAB_GuiMenu_Save, world);

    LAB_GuiButton_Create(&menu->cmdSaveAs,
                         10, 90, 100, 25,
                         "Save As\u2026", &LAB_GuiMenu_SaveAs, world);

    LAB_GuiButton_Create(&menu->cmdLoad,
                         10, 120, 100, 25,
                         "Load\u2026", &LAB_GuiMenu_Load, world);

    LAB_GuiButton_Create(&menu->cmdQuit,
                         10, 150, 100, 25,
                         "Quit", &LAB_GuiMenu_Quit, NULL);
}

#include <stdio.h>
static void LAB_GuiMenu_Resume(void* user, LAB_GuiManager* mgr)
{
    LAB_GuiManager_Dismiss(mgr);
}

static void LAB_GuiMenu_Save(void* user, LAB_GuiManager* mgr)
{
    LAB_World* world = (LAB_World*)user;
    printf("Saving world here: %i chunks\n", (int)world->chunks.size);
    //LAB_GuiManager_Dismiss(mgr);
}

static void LAB_GuiMenu_SaveAs(void* user, LAB_GuiManager* mgr)
{
   // LAB_World* world = (LAB_World*)user;
    printf("Saving dialog here\n");
    //LAB_GuiManager_Dismiss(mgr);
}

static void LAB_GuiMenu_Load(void* user, LAB_GuiManager* mgr)
{
    LAB_GuiManager_Dismiss(mgr);
    //printf("Loading dialog here\n");

    LAB_GuiWorldSelection* gui = LAB_Malloc(sizeof *gui);
    if(!gui) return;
    LAB_GuiWorldSelection_Create(gui, "Load World", NULL);
    LAB_GuiManager_ShowDialog(mgr, (LAB_GuiComponent*)gui);
}

static void LAB_GuiMenu_Quit(void* user, LAB_GuiManager* mgr)
{
    SDL_Event quit_event = { .type = SDL_QUIT };
    SDL_PushEvent(&quit_event);
}
