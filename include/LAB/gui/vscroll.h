#pragma once

#include "LAB/gui/component.h"
#include <SDL2/SDL_surface.h>
#include "LAB_vec2.h"

typedef struct LAB_GuiVScroll
{
    LAB_GUI_COMPONENT_INHERIT;

    int state;

    int total_height;
    int viewport_height;
    int scroll_value;

    int grip_pos;

    bool (*on_scroll)(void* ctx, int scroll_value);
    void* on_scroll_ctx;
} LAB_GuiVScroll;

typedef struct LAB_GuiScroll_Spec
{
    LAB_Box2I rect;
    int total, viewport;

    bool (*on_scroll)(void* ctx, int scroll_value);
    void* on_scroll_ctx;
} LAB_GuiScroll_Spec;

void LAB_GuiVScroll_Create(LAB_GuiVScroll* cself, LAB_GuiScroll_Spec spec);
bool LAB_GuiVScroll_SetScrollValue(LAB_GuiVScroll* cself, int new_scroll);