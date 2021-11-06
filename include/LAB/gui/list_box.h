#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>

#define LAB_GUI_LIST_BOX_ELEMENT_HEIGHT 16

typedef struct LAB_GuiListBox
{
    LAB_GUI_COMPONENT_INHERIT;
    size_t element_count;
    const char* const* elements;
    SDL_Surface** text_surfs;

    size_t selected_element;

    void (*on_selection)(void* user, size_t element);
    void*  on_selection_user;

    int state;
} LAB_GuiListBox;

// memory of elements not taken ownership over, caller needs to clean it up
void LAB_GuiListBox_Create(LAB_GuiListBox* cself,
                          int x, int y, int w, int h,
                          size_t element_count, const char* const* elements,
                          void (*on_selection)(void* user, size_t element),
                          void*  on_selection_user);
