#include "LAB/gui/world_selection.h"

#include "LAB_memory.h"
#include <limits.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#define LAB_WORLD_DIRECTORY "./worlds"

LAB_STATIC
void LAB_GuiWorldSelection_Destroy(LAB_GuiComponent* self);

LAB_STATIC bool LAB_GuiWorldSelection_InitList(LAB_GuiWorldSelection* gui)
{
    DIR* d = opendir(LAB_WORLD_DIRECTORY);
    if(d == NULL) return false;

    bool success = true;
    struct dirent * dir;
    while((dir = readdir(d)) != NULL)
    {
        if(dir->d_name[0] != '.')
        {
            char** entry = LAB_ARRAY_APPEND_SOME(LAB_GuiWorldSelection_worlds(gui), 1);
            if(entry == NULL) { success = false; goto cleanup; }
            *entry = LAB_StrDup(dir->d_name);
            if(*entry == NULL) {
                gui->worlds_count--;
                success = false;
                goto cleanup;
            }
        }
    }
cleanup:
    closedir(d);

    return success;

}

LAB_STATIC void LAB_GuiWorldSelection_lstWorlds_OnSelect(void* user, size_t entry);
LAB_STATIC void LAB_GuiWorldSelection_OK(void* user, LAB_GuiManager* mgr);

LAB_STATIC bool LAB_GuiWorldSelection_OnWorldsScroll(void* vgui, int scroll_value)
{
    LAB_GuiWorldSelection* gui = (LAB_GuiWorldSelection*)vgui;

    gui->lstWorlds.top_element = scroll_value;

    return true;
}

LAB_STATIC
bool LAB_GuiWorldSelection_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiWorldSelection* cself = (LAB_GuiWorldSelection*)self;

    if(event->type == SDL_MOUSEWHEEL)
    {
        int new_scroll = cself->scrWorlds.scroll_value - event->wheel.y;

        return LAB_GuiVScroll_SetScrollValue(&cself->scrWorlds, new_scroll);
    }

    return LAB_GuiContainer_OnEvent(self, mgr, event);
}

void LAB_GuiWorldSelection_Create(LAB_GuiWorldSelection* gui, const char* title, LAB_WorldSelectProc on_select)
{
    int scroll_width = 14;

    gui->x = 0;
    gui->y = 0;
    gui->w = 220;
    gui->h = 188;

    gui->on_event = &LAB_GuiWorldSelection_OnEvent;
    gui->render = &LAB_GuiContainer_Render_Framed;
    gui->destroy = &LAB_GuiWorldSelection_Destroy;
    gui->current = NULL; // TODO call LAB_GuiContainer_Create

    gui->components = gui->components_arr;
    gui->components_arr[0] = (LAB_GuiComponent*)&gui->lblTitle;
    gui->components_arr[1] = (LAB_GuiComponent*)&gui->lstWorlds;
    gui->components_arr[2] = (LAB_GuiComponent*)&gui->scrWorlds;
    gui->components_arr[3] = (LAB_GuiComponent*)&gui->txtWorld;
    gui->components_arr[4] = (LAB_GuiComponent*)&gui->cmdOK;
    gui->components_arr[5] = NULL;


    LAB_GuiLabel_Create(&gui->lblTitle, (LAB_GuiLabel_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 5, 200, 25),
        .title = title,
    });


    gui->worlds_count = 0;
    gui->worlds = NULL;
    LAB_GuiWorldSelection_InitList(gui); // ERR

    LAB_GuiListBox_Create(&gui->lstWorlds, (LAB_GuiListBox_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 30, 200-scroll_width-4, 118),

        .element_count = gui->worlds_count,
        .elements = /*char** -> */(const char*const*)gui->worlds,

        .on_selection = LAB_GuiWorldSelection_lstWorlds_OnSelect,
        .ctx = gui,
    });

    LAB_GuiVScroll_Create(&gui->scrWorlds, (LAB_GuiScroll_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10+200-scroll_width, 30, scroll_width, 118),
        .total = gui->worlds_count,
        .viewport = LAB_GuiListBox_Viewport(&gui->lstWorlds),
        .on_scroll = LAB_GuiWorldSelection_OnWorldsScroll,
        .on_scroll_ctx = gui,
    });

    LAB_GuiTextBox_Create(&gui->txtWorld, (LAB_GuiTextBox_Spec)
    {
        .rect = LAB_Box2I_New_Sized(10, 153, 140, 25),
    });

    LAB_GuiButton_Create(&gui->cmdOK, (LAB_GuiButton_Spec)
    {
        .rect = LAB_Box2I_New_Sized(155, 153, 55, 25),
        .title = "OK",
        .on_click = &LAB_GuiWorldSelection_OK,
        .ctx = gui,
    });

}

LAB_STATIC
void LAB_GuiWorldSelection_Destroy(LAB_GuiComponent* self)
{
    LAB_GuiContainer_Destroy(self);
    LAB_GuiWorldSelection* cself = (LAB_GuiWorldSelection*)self;
    for(size_t i = 0; i < cself->worlds_count; ++i)
    {
        LAB_Free(cself->worlds[i]);
    }
    LAB_Free(cself->worlds);
}

LAB_STATIC void LAB_GuiWorldSelection_lstWorlds_OnSelect(void* user, size_t entry)
{
    LAB_GuiWorldSelection* gui = (LAB_GuiWorldSelection*)user;
    if(entry < gui->worlds_count)
    {
        LAB_GuiVScroll_ScrollTowards(&gui->scrWorlds, entry);
        LAB_GuiTextBox_SetContent(&gui->txtWorld, gui->worlds[entry]);
    }
}

LAB_STATIC void LAB_GuiWorldSelection_OK(void* user, LAB_GuiManager* mgr)
{
    LAB_GuiWorldSelection* gui = (LAB_GuiWorldSelection*)user;
    const char* filename = gui->txtWorld.content;
    if(filename == NULL) filename = "";
    if(filename[0] == '\0')
    {
        printf("Not loading world\n");
    }
    else
    {
        printf("Loading world here %s...\n", filename);
        printf("NOT IMPLEMENTED\n");
    }
    LAB_GuiManager_Dismiss(mgr);
}
