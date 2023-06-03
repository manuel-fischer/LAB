#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>
#include "LAB_vec2.h"

// export LAB_GuiListBox* LAB_GUI_LIST_BOX*

#define LAB_GUI_LIST_BOX_BORDER_INSET 3
#define LAB_GUI_LIST_BOX_ELEMENT_HEIGHT 16

typedef struct LAB_GuiListBox
{
    LAB_GUI_COMPONENT_INHERIT;
    size_t element_count;
    const char* const* elements;
    SDL_Surface** text_surfs;

    size_t top_element;

    size_t selected_element;

    void (*on_selection)(void* ctx, size_t element);
    void* ctx;

    int state;
} LAB_GuiListBox;


typedef struct LAB_GuiListBox_Spec
{
    LAB_Box2I rect;
    size_t element_count;
    const char* const* elements;

    void (*on_selection)(void* ctx, size_t element);
    void (*on_scroll)(void* ctx, size_t new_top);
    void* ctx;
} LAB_GuiListBox_Spec;


// memory of elements not taken ownership over, caller needs to clean it up
void LAB_GuiListBox_Create(LAB_GuiListBox* cself, LAB_GuiListBox_Spec spec);

// number of simultaneously displayed items
size_t LAB_GuiListBox_Viewport(LAB_GuiListBox* cself);