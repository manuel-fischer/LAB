#include "LAB/gui/util.h"

bool LAB_Event_GetMouseCoordPtr(SDL_Event* event, int** x, int** y)
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

bool LAB_Event_GetMouseCoord(SDL_Event* event, int* x, int* y)
{
    int* px,* py;
    if(!LAB_Event_GetMouseCoordPtr(event, &px, &py)) return false;

    *x = *px;
    *y = *py;

    return true;
}