#pragma once

#include "LAB_attr.h"
#include "LAB/gui/component.h"

// export LAB_Event_*
// export LAB_GuiHitTest

bool LAB_Event_GetMouseCoordPtr(SDL_Event* event, int** x, int** y);
bool LAB_Event_GetMouseCoord(SDL_Event* event, int* x, int* y);

LAB_INLINE bool LAB_GuiHitTest(LAB_GuiComponent* component, int x, int y)
{
    x -= component->x;
    y -= component->y;
    return 0 <= x && x < component->w
        && 0 <= y && y < component->h;
}
