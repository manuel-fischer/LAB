#include "LAB_gui_component.h"

int LAB_GetMouseCoordPtr(SDL_Event* event, int** x, int** y)
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

int  LAB_GuiContainer_OnEvent(LAB_GuiComponent* self, SDL_Event* event)
{
    LAB_GuiContainer* cself = (LAB_GuiContainer*)self;
    LAB_GuiComponent** c;

    int *x, *y;
    if(LAB_GetMouseCoordPtr(event, &x, &y))
    {
        for(c = cself->components; *c; ++c)
        {
            if(LAB_GuiHitTest(*c, *x, *y))
            {
                cself->current = *c;
                *x -= (*c)->x;
                *y -= (*c)->y;
                return (*c)->on_event(*c, event);
            }
        }
    }
    else
    {
        for(c = cself->components; *c; ++c)
        {

        }
    }
    return 0;
}

int printf();

// Does not clip
void LAB_GuiContainer_Render(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y)
{
    LAB_GuiContainer* cself = (LAB_GuiContainer*)self;

    LAB_GuiComponent** c;
    for(c = cself->components; *c; ++c)
    {
        (*c)->render(*c, surf, x+(*c)->x, y+(*c)->y);
    }
}

void LAB_GuiContainer_Render_Framed(LAB_GuiComponent* self, SDL_Surface* surf, int x, int y)
{
    LAB_GuiContainer* cself = (LAB_GuiContainer*)self;

    //surf.paintpicture x, y, cself->w, cself->h, sp.x, sp.y, sp.w, sp.h;
    LAB_RenderRect(surf, 0, 0, self->w, self->h, 0, 3);

    LAB_GuiContainer_Render(self, surf, x, y);
}



