#include "LAB_gui_world_selection.h"

#include "LAB_memory.h"
#include <limits.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

static bool LAB_GuiWorldSelection_ListDir(size_t* world_count, char*** world_names)
{
    size_t world_capacity = *world_count;

    DIR* d = opendir("./worlds");
    if(d == NULL) return 0;

    bool success = 1;
    struct dirent * dir;
    while((dir = readdir(d)) != NULL)
    {
        if(dir->d_name[0] != '.')
        {
            if(*world_count == world_capacity)
            {
                if(world_capacity == 0) world_capacity = 4;
                else                    world_capacity*= 2;
                char** mem = LAB_Realloc(*world_names, world_capacity*sizeof(*mem));
                if(!mem)
                {
                    success = 0;
                    goto cleanup;
                }
                *world_names = mem;
            }
            char* str = LAB_StrDup(dir->d_name);
            if(!str)
            {
                success = 0;
                goto cleanup;
            }
            (*world_names)[*world_count] = str;
            ++*world_count;
        }
    }
cleanup:
    closedir(d);

    return success;

}

static void LAB_GuiWorldSelection_lstWorlds_OnSelect(void* user, size_t entry);
static void LAB_GuiWorldSelection_OK(void* user, LAB_GuiManager* mgr);

void LAB_GuiWorldSelection_Create(LAB_GuiWorldSelection* gui, const char* title, LAB_WorldSelectProc on_select)
{
    gui->x = 0;
    gui->y = 0;
    gui->w = 220;
    gui->h = 185;

    gui->on_event = &LAB_GuiContainer_OnEvent;
    gui->render = &LAB_GuiContainer_Render_Framed;
    gui->destroy = &LAB_GuiWorldSelection_Destroy;
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


    gui->world_count = 0;
    gui->worlds = NULL;
    LAB_GuiWorldSelection_ListDir(&gui->world_count, &gui->worlds); // ERR

    LAB_GuiListBox_Create(&gui->lstWorlds,
                          10, 30, 200, 115,
                          gui->world_count, /*char** -> */(const char*const*)gui->worlds,
                          LAB_GuiWorldSelection_lstWorlds_OnSelect, gui);

    LAB_GuiTextBox_Create(&gui->txtWorld,
                          10, 150, 140, 25);

    LAB_GuiButton_Create(&gui->cmdOK,
                         155, 150, 55, 25,
                         "OK", &LAB_GuiWorldSelection_OK, gui);

}

void LAB_GuiWorldSelection_Destroy(LAB_GuiComponent* self)
{
    LAB_GuiContainer_Destroy(self);
    LAB_GuiWorldSelection* cself = (LAB_GuiWorldSelection*)self;
    for(size_t i = 0; i < cself->world_count; ++i)
    {
        LAB_Free(cself->worlds[i]);
    }
    LAB_Free(cself->worlds);
}

static void LAB_GuiWorldSelection_lstWorlds_OnSelect(void* user, size_t entry)
{
    LAB_GuiWorldSelection* gui = (LAB_GuiWorldSelection*)user;
    if(entry < gui->world_count)
    {
        LAB_GuiTextBox_SetContent(&gui->txtWorld, gui->worlds[entry]);
    }
}

static void LAB_GuiWorldSelection_OK(void* user, LAB_GuiManager* mgr)
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
