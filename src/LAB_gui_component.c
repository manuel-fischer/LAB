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

bool LAB_GuiContainer_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiContainer* cself = (LAB_GuiContainer*)self;
    LAB_GuiComponent** c;

    int rerender = 0;

    SDL_Event tmp_event;

    int *x, *y;
    if(LAB_GetMouseCoordPtr(event, &x, &y))
    {
        for(c = cself->components; *c; ++c)
        {
            if(LAB_GuiHitTest(*c, *x, *y))
            {
                if(cself->current != *c)
                {
                    if(cself->current)
                    {
                        tmp_event.type = LAB_GUI_EVENT2SDL(LAB_GUI_EVENT_UNFOCUS);
                        rerender |= (cself->current)->on_event(cself->current, mgr, &tmp_event);
                    }
                    cself->current = *c;
                    tmp_event.type = LAB_GUI_EVENT2SDL(LAB_GUI_EVENT_FOCUS);
                    rerender |= (*c)->on_event(*c, mgr, &tmp_event);
                }
                *x -= (*c)->x;
                *y -= (*c)->y;
                LAB_ASSUME((*c)->on_event);
                return rerender | (*c)->on_event(*c, mgr, event);
            }
        }
        if(cself->current)
        {
            tmp_event.type = LAB_GUI_EVENT2SDL(LAB_GUI_EVENT_UNFOCUS);
            rerender |= (cself->current)->on_event(cself->current, mgr, &tmp_event);
            cself->current = NULL;
        }
    }
    else
    {
        if(cself->current)
        {
            rerender |= (cself->current)->on_event(cself->current, mgr, event);
        }
    }
    return rerender;
}

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
    //LAB_GuiContainer* cself = (LAB_GuiContainer*)self;

    //surf.paintpicture x, y, cself->w, cself->h, sp.x, sp.y, sp.w, sp.h;
    LAB_RenderRect(surf, 0, 0, self->w, self->h, 0, 3);

    LAB_GuiContainer_Render(self, surf, x, y);
}



