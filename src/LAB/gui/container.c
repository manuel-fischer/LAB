#include "LAB/gui/container.h"
#include "LAB/gui/util.h"

#include "LAB_debug.h"
#include <stdio.h>

LAB_STATIC
bool LAB_GuiContainer_OnEvent_Mouse(LAB_GuiContainer* cself, LAB_GuiManager* mgr, SDL_Event* event, int *x, int *y)
{
    bool rerender = false;

    SDL_Event tmp_event;

    bool is_drag = event->type == SDL_MOUSEMOTION && event->motion.state;

    if(is_drag && cself->current)
    {
        *x -= cself->current->x;
        *y -= cself->current->y;
        LAB_ASSUME(cself->current->on_event);
        return rerender | cself->current->on_event(cself->current, mgr, event);
    }
    else
    {
        LAB_GuiComponent** c;
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
    }
    if(cself->current)
    {
        tmp_event.type = LAB_GUI_EVENT2SDL(LAB_GUI_EVENT_UNFOCUS);
        rerender |= (cself->current)->on_event(cself->current, mgr, &tmp_event);
        cself->current = NULL;
    }
    return rerender;
}

bool LAB_GuiContainer_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiContainer* cself = (LAB_GuiContainer*)self;

    bool rerender = false;

    int *x, *y;
    if(LAB_GetMouseCoordPtr(event, &x, &y))
    {
        rerender |= LAB_GuiContainer_OnEvent_Mouse(cself, mgr, event, x, y);
    }
    else if(event->type == LAB_GUI_EVENT2SDL(LAB_GUI_EVENT_UNFOCUS))
    {
        if(cself->current)
        {
            rerender |= (cself->current)->on_event(cself->current, mgr, event);
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
void LAB_GuiContainer_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf, int x, int y)
{
    LAB_GuiContainer* cself = (LAB_GuiContainer*)self;

    LAB_GuiComponent** c;
    for(c = cself->components; *c; ++c)
    {
        (*c)->render(*c, mgr, surf, x+(*c)->x, y+(*c)->y);
    }
}

void LAB_GuiContainer_Render_Framed(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf, int x, int y)
{
    //LAB_GuiContainer* cself = (LAB_GuiContainer*)self;

    int s = mgr->scale;

    //surf.paintpicture x, y, cself->w, cself->h, sp.x, sp.y, sp.w, sp.h;
    LAB_RenderRect(surf, s, s*x, s*y, s*self->w, s*self->h, 1, 3);

    LAB_GuiContainer_Render(self, mgr, surf, x, y);
}

void LAB_GuiContainer_Destroy(LAB_GuiComponent* self)
{
    LAB_GuiContainer* cself = (LAB_GuiContainer*)self;

    LAB_GuiComponent** c;
    for(c = cself->components; *c; ++c)
    {
        (*c)->destroy(*c);
    }
}
