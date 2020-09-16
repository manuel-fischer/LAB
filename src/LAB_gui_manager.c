#include "LAB_gui_manager.h"
#include "LAB_gui_component.h"
#include "LAB_memory.h"
#include "LAB_gl.h"

#define GUI_SCALE 2

void LAB_GuiManager_Create(LAB_GuiManager* manager)
{
    manager->component = NULL;
}

void LAB_GuiManager_Destroy(LAB_GuiManager* manager)
{
    LAB_Free(manager->component);
}


void LAB_GuiManager_ShowDialog(LAB_GuiManager* manager, LAB_GuiComponent* component)
{
    if(manager->component) LAB_Free(manager->component);
    manager->component = component;
}

void LAB_GuiManager_Dismiss(LAB_GuiManager* manager)
{
    LAB_Free(manager->component);
    manager->component = NULL;
    //memset(manager, 0, sizeof*manager);
}



void LAB_GuiManager_Render(LAB_GuiManager* manager, int sw, int sh)
{
    if(manager->component)
    {
        LAB_GuiComponent* c = manager->component;

        c->x = (sw/GUI_SCALE-c->w)/2;
        c->y = (sh/GUI_SCALE-c->h)/2;

        SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, c->w, c->h, 32, SDL_PIXELFORMAT_RGBA32);
        if(!surf) return;

        (*c->render)(c, surf, 0, 0);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        LAB_GL_ActivateTexture(&manager->gl_id);
        LAB_GL_UploadSurf(manager->gl_id, surf);
        LAB_GL_DrawSurf(manager->gl_id, c->x*GUI_SCALE, c->y*GUI_SCALE, c->w*GUI_SCALE, c->h*GUI_SCALE, sw, sh);

        SDL_FreeSurface(surf);
    }
}

int LAB_GuiManager_HandleEvent(LAB_GuiManager* manager, SDL_Event* event)
{
    if(manager->component)
    {
        if(event->type == SDL_KEYUP)
        {
            SDL_Keycode key = ((SDL_KeyboardEvent*)event)->keysym.sym;
            switch(key)
            {
                case SDLK_ESCAPE:
                    LAB_GuiManager_Dismiss(manager);
                    return 1;
            }
        }

        int* x,* y;
        if(LAB_GetMouseCoordPtr(event, &x, &y))
        {
            *x /= GUI_SCALE;
            *y /= GUI_SCALE;
            if(!LAB_GuiHitTest(manager->component, *x, *y))
            {
                if(event->type == SDL_MOUSEBUTTONUP)
                    LAB_GuiManager_Dismiss(manager);

                return 1;
            }

            *x-=manager->component->x;
            *y-=manager->component->y;
        }


        (*manager->component->on_event)(manager->component, event);
        return 1;
    }
    return 0;
}
