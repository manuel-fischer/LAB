#include "LAB/gui/menu.h"

#include "LAB_world.h"
#include "LAB/gui/world_selection.h"
#include "LAB_memory.h"

LAB_STATIC void LAB_GuiMenu_Resume(void* user, LAB_GuiManager* mgr);
LAB_STATIC void LAB_GuiMenu_Save(void* user, LAB_GuiManager* mgr);
LAB_STATIC void LAB_GuiMenu_SaveAs(void* user, LAB_GuiManager* mgr);
LAB_STATIC void LAB_GuiMenu_Load(void* user, LAB_GuiManager* mgr);
LAB_STATIC void LAB_GuiMenu_Quit(void* user, LAB_GuiManager* mgr);

void LAB_GuiMenu_Create(LAB_GuiMenu* menu, LAB_World* world)
{
    menu->x = 0;
    menu->y = 0;
    menu->w = 120;
    menu->h = 185;

    menu->on_event = &LAB_GuiContainer_OnEvent;
    menu->render = &LAB_GuiContainer_Render_Framed;
    menu->destroy = &LAB_GuiContainer_Destroy;
    menu->current = NULL; // TODO call LAB_GuiContainer_Create

    menu->components = menu->components_arr;
    menu->components_arr[0] = (LAB_GuiComponent*)&menu->lblTitle;
    menu->components_arr[1] = (LAB_GuiComponent*)&menu->cmdResume;
    menu->components_arr[2] = (LAB_GuiComponent*)&menu->cmdSave;
    menu->components_arr[3] = (LAB_GuiComponent*)&menu->cmdSaveAs;
    menu->components_arr[4] = (LAB_GuiComponent*)&menu->cmdLoad;
    menu->components_arr[5] = (LAB_GuiComponent*)&menu->cmdQuit;
    menu->components_arr[6] = NULL;


    LAB_GuiLabel_Create(&menu->lblTitle, (LAB_GuiLabel_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 5, 100, 25),
        .title = "Menu",
    });

    LAB_GuiButton_Create(&menu->cmdResume, (LAB_GuiButton_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 30, 100, 25),
        .title = "Resume",
        .on_click = &LAB_GuiMenu_Resume,
        .ctx = NULL,
    });

    LAB_GuiButton_Create(&menu->cmdSave, (LAB_GuiButton_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 60, 100, 25),
        .title = "Save",
        .on_click = &LAB_GuiMenu_Save,
        .ctx = world,
    });

    LAB_GuiButton_Create(&menu->cmdSaveAs, (LAB_GuiButton_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 90, 100, 25),
        .title = "Save As\u2026",
        .on_click = &LAB_GuiMenu_SaveAs,
        .ctx = world,
    });

    LAB_GuiButton_Create(&menu->cmdLoad, (LAB_GuiButton_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 120, 100, 25),
        .title = "Load\u2026",
        .on_click = &LAB_GuiMenu_Load,
        .ctx = world,
    });

    LAB_GuiButton_Create(&menu->cmdQuit, (LAB_GuiButton_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 150, 100, 25),
        .title = "Quit",
        .on_click = &LAB_GuiMenu_Quit,
        .ctx = NULL,
    });
}

#include <stdio.h>
LAB_STATIC void LAB_GuiMenu_Resume(void* user, LAB_GuiManager* mgr)
{
    LAB_GuiManager_Dismiss(mgr);
}

LAB_STATIC void LAB_GuiMenu_Save(void* user, LAB_GuiManager* mgr)
{
    LAB_World* world = (LAB_World*)user;
    printf("Saving world here: %i chunks\n", (int)world->chunks.size);
    //LAB_GuiManager_Dismiss(mgr);
}

LAB_STATIC void LAB_GuiMenu_SaveAs(void* user, LAB_GuiManager* mgr)
{
   // LAB_World* world = (LAB_World*)user;
    printf("Saving dialog here\n");
    //LAB_GuiManager_Dismiss(mgr);
}

LAB_STATIC void LAB_GuiMenu_Load(void* user, LAB_GuiManager* mgr)
{
    LAB_GuiManager_Dismiss(mgr);
    //printf("Loading dialog here\n");

    LAB_GuiWorldSelection* gui = LAB_Malloc(sizeof *gui);
    if(!gui) return;
    LAB_GuiWorldSelection_Create(gui, "Load World", NULL);
    LAB_GuiManager_ShowDialog(mgr, (LAB_GuiComponent*)gui);
}

LAB_STATIC void LAB_GuiMenu_Quit(void* user, LAB_GuiManager* mgr)
{
    SDL_Event quit_event = { .type = SDL_QUIT };
    SDL_PushEvent(&quit_event);
}
