#pragma once

#include "LAB_gui_component.h"
#include <SDL2/SDL_surface.h>

#define LAB_GUI_LIST_BOX_ELEMENT_HEIGHT 16

typedef struct LAB_GuiListBox
{
    LAB_GUI_COMPONENT_INHERIT;
    size_t element_count;
    const char* const* elements;
    SDL_Surface** text_surfs;

    size_t selected_element; // every index above element_count means no element is selected

    void (*on_selection)(void* user, size_t element);
    void*  on_selection_user;

    int state;
} LAB_GuiListBox;

enum LAB_GuiListBoxState
{
    LAB_GUI_LIST_BOX_NORMAL = 0,
    LAB_GUI_LIST_BOX_FOCUSED,
};

// memory of elements not taken ownership over, caller needs to clean it up
void LAB_GuiListBox_Create(LAB_GuiListBox* cself,
                          int x, int y, int w, int h,
                          size_t element_count, const char* const* elements,
                          void (*on_selection)(void* user, size_t element),
                          void*  on_selection_user);

void LAB_GuiListBox_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                          int x, int y);

bool LAB_GuiListBox_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);


void LAB_GuiListBox_Destroy(LAB_GuiComponent* self);
