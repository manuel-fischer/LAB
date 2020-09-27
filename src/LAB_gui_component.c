#include "LAB_gui_component.h"
#include "LAB_debug.h"

bool LAB_GetMouseCoordPtr(SDL_Event* event, int** x, int** y)
{
    switch(event->type)
    {
        case SDL_MOUSEMOTION:
            *x = &event->motion.x;
            *y = &event->motion.y;
            return 1;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            *x = &event->button.x;
            *y = &event->button.y;
            return 1;

        default:
            return 0;
    }
}

bool LAB_Gui_OnEvent_Ignore(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    return 0;
}


void LAB_GuiComponent_Destroy_Ignore(LAB_GuiComponent* self)
{
}
