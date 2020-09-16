#include "LAB_gui_menu.h"

static void LAB_GuiMenu_Dummy(void* user);
static void LAB_GuiMenu_Quit(void* user);

void LAB_GuiMenu_Create(LAB_GuiMenu* menu)
{
    menu->x = 0;
    menu->y = 0;
    menu->w = 100;
    menu->h = 125;
    menu->components = menu->components_arr;
    menu->components_arr[0] = (LAB_GuiComponent*)&menu->lblMenu;
    menu->components_arr[1] = (LAB_GuiComponent*)&menu->cmdSave;
    menu->components_arr[2] = (LAB_GuiComponent*)&menu->cmdLoad;
    menu->components_arr[3] = (LAB_GuiComponent*)&menu->cmdQuit;
    menu->components_arr[4] = NULL;

    menu->on_event = &LAB_GuiContainer_OnEvent;
    menu->render = &LAB_GuiContainer_Render_Framed;

    LAB_GuiLabel_Create(&menu->lblMenu,
                        10, 5, 80, 25,
                        "Menu");

    LAB_GuiButton_Create(&menu->cmdSave,
                         10, 30, 80, 25,
                         "Save", &LAB_GuiMenu_Dummy, NULL);

    LAB_GuiButton_Create(&menu->cmdLoad,
                         10, 60, 80, 25,
                         "Load", &LAB_GuiMenu_Dummy, NULL);

    LAB_GuiButton_Create(&menu->cmdQuit,
                         10, 90, 80, 25,
                         "Quit", &LAB_GuiMenu_Quit, NULL);
}

#include <stdio.h>
void LAB_GuiMenu_Dummy(void* user)
{
    printf("CLICKED\n");
}


static void LAB_GuiMenu_Quit(void* user)
{
    SDL_Event quit_event = { .type = SDL_QUIT };
    SDL_PushEvent(&quit_event);
}
