#include "LAB_gui_manager.h"
#include "LAB_gui_component.h"
#include "LAB_memory.h"
#include "LAB_gl.h"
#include "LAB_debug.h"

#define GUI_SCALE 2

void LAB_GuiManager_Create(LAB_GuiManager* manager)
{
    manager->component = NULL;
    manager->surf = NULL;
    manager->dismiss_component = NULL;
}

void LAB_GuiManager_Destroy(LAB_GuiManager* manager)
{
    SDL_FreeSurface(manager->surf);

    if(manager->component)
    {
        (manager->component)->destroy(manager->component);
        LAB_Free(manager->component);
    }
    if(manager->dismiss_component)
    {
        (manager->dismiss_component)->destroy(manager->dismiss_component);
        LAB_Free(manager->dismiss_component);
    }
}


void LAB_GuiManager_ShowDialog(LAB_GuiManager* manager, LAB_GuiComponent* component)
{
    //LAB_ASSUME(manager->component != component);
    LAB_ASSUME(manager->component == NULL);
    //if(manager->component) LAB_Free(manager->component);
    manager->component = component;
    manager->rerender = 1;
}

void LAB_GuiManager_Dismiss(LAB_GuiManager* manager)
{
    // Component cannot be freed directly, because this function might
    // have been called inside an event handler of that very component.
    // For this, the function LAB_GuiManager_Tick needs to be called,
    // It should be called when no events are handled.
    LAB_ASSUME(manager->component != NULL);
    LAB_ASSUME(manager->dismiss_component == NULL);
    manager->dismiss_component = manager->component;
    manager->component = NULL;
}


void LAB_GuiManager_Tick(LAB_GuiManager* manager)
{
    if(manager->dismiss_component)
    {
        (manager->dismiss_component)->destroy(manager->dismiss_component);
        LAB_Free(manager->dismiss_component);
        manager->dismiss_component = NULL;
    }
}


void LAB_GuiManager_Render(LAB_GuiManager* mgr, int sw, int sh)
{
    if(mgr->component)
    {
        LAB_GuiComponent* c = mgr->component;

        c->x = (sw/GUI_SCALE-c->w)/2;
        c->y = (sh/GUI_SCALE-c->h)/2;

        int rerender = 0;
        if(mgr->surf)
        {
            if(c->w != mgr->surf->w ||
               c->h != mgr->surf->h)
            {
                SDL_FreeSurface(mgr->surf);
                mgr->surf = NULL;
            }
        }

        if(!mgr->surf)
        {
            mgr->surf = SDL_CreateRGBSurfaceWithFormat(0, c->w, c->h, 32, SDL_PIXELFORMAT_RGBA32);
            rerender = 1;
            if(!mgr->surf) return;
        }
        else if(mgr->rerender)
        {
            // clear buffer
            memset(mgr->surf->pixels, 0, 4*c->w*c->h);
            rerender = 1;
        }

        if(rerender)
        {
            (*c->render)(c, mgr->surf, 0, 0);
            mgr->rerender = 0;
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        LAB_GL_ActivateTexture(&mgr->gl_id);
        LAB_GL_UploadSurf(mgr->gl_id, mgr->surf);
        LAB_GL_DrawSurf(mgr->gl_id, c->x*GUI_SCALE, c->y*GUI_SCALE, c->w*GUI_SCALE, c->h*GUI_SCALE, sw, sh);
    }
}

bool LAB_GuiManager_HandleEvent(LAB_GuiManager* mgr, SDL_Event* event)
{
    if(mgr->component)
    {
        if(event->type == SDL_KEYUP)
        {
            SDL_Keycode key = ((SDL_KeyboardEvent*)event)->keysym.sym;
            switch(key)
            {
                case SDLK_ESCAPE:
                    LAB_GuiManager_Dismiss(mgr);
                    return 1;
            }
        }

        int* x,* y;
        if(LAB_GetMouseCoordPtr(event, &x, &y))
        {
            *x /= GUI_SCALE;
            *y /= GUI_SCALE;
            if(!LAB_GuiHitTest(mgr->component, *x, *y))
            {
                if(event->type == SDL_MOUSEBUTTONDOWN)
                {
                    LAB_GuiManager_Dismiss(mgr);
                }
                else if(event->type == SDL_MOUSEMOTION)
                {
                    SDL_Event tmp_event;
                    tmp_event.type = LAB_GUI_EVENT2SDL(LAB_GUI_EVENT_UNFOCUS);
                    int rerender;
                    rerender = (*mgr->component->on_event)(mgr->component, mgr, &tmp_event);
                    if(rerender) mgr->rerender = 1;
                }

                return 1;
            }

            *x-=mgr->component->x;
            *y-=mgr->component->y;
        }

        int rerender;
        rerender = (*mgr->component->on_event)(mgr->component, mgr, event);
        if(rerender) mgr->rerender = 1;
        return 1;
    }
    return 0;
}
