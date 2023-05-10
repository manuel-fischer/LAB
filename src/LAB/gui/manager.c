#include "LAB/gui/manager.h"
#include "LAB/gui/component.h"
#include "LAB_memory.h"
#include "LAB_gl.h"
#include "LAB_debug.h"
#include "LAB_sdl.h"
#include "LAB/gui/util.h"
#include "LAB_color_defs.h"

#include <SDL2/SDL_ttf.h>

bool LAB_GuiManager_Create(LAB_GuiManager* mgr)
{
    mgr->component = NULL;
    mgr->surf = NULL;
    mgr->dismiss_component = NULL;
    mgr->zoom = 1;
    mgr->scale = 2;

    LAB_SDL_ALLOC(TTF_OpenFont, &mgr->button_font, "fonts/DejaVuSans.ttf", 12*mgr->scale);
    if(!mgr->button_font) return 0;

    LAB_SDL_ALLOC(TTF_OpenFont, &mgr->title_font, "fonts/DejaVuSans.ttf", 15*mgr->scale);
    if(!mgr->title_font)  return 0;

    LAB_SDL_ALLOC(TTF_OpenFont, &mgr->mono_font, "fonts/DejaVuSansMono.ttf", 11*mgr->scale); // 13
    if(!mgr->mono_font)   return 0;

    LAB_SDL_ALLOC(TTF_OpenFont, &mgr->mono_font_small, "fonts/DejaVuSansMono.ttf", 7*mgr->scale); // 13
    if(!mgr->mono_font_small)   return 0;

    return 1;
}

void LAB_GuiManager_Destroy(LAB_GuiManager* mgr)
{
    LAB_SDL_FREE(SDL_FreeSurface, &mgr->surf);

    if(mgr->component)
    {
        (mgr->component)->destroy(mgr->component);
        LAB_Free(mgr->component);
    }
    if(mgr->dismiss_component)
    {
        (mgr->dismiss_component)->destroy(mgr->dismiss_component);
        LAB_Free(mgr->dismiss_component);
    }

    LAB_SDL_FREE(TTF_CloseFont, &mgr->button_font);
    LAB_SDL_FREE(TTF_CloseFont, &mgr->title_font);
    LAB_SDL_FREE(TTF_CloseFont, &mgr->mono_font);
    LAB_SDL_FREE(TTF_CloseFont, &mgr->mono_font_small);

    LAB_GL_FREE(glDeleteTextures, 1, &mgr->gl_tex.id);
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


void LAB_GuiManager_Render(LAB_GuiManager* mgr, LAB_SurfaceRenderer* r, int sw, int sh)
{
    if(mgr->component)
    {
        int z = mgr->zoom;
        int s = mgr->scale;

        LAB_GuiComponent* c = mgr->component;

        c->x = (sw/(s*z)-c->w)/2;
        c->y = (sh/(s*z)-c->h)/2;

        int rerender = 0;
        if(mgr->surf)
        {
            if(c->w*s != mgr->surf->w ||
               c->h*s != mgr->surf->h)
            {
                LAB_SDL_FREE(SDL_FreeSurface, &mgr->surf);
                mgr->surf = NULL;
            }
        }

        if(!mgr->surf)
        {
            LAB_SDL_ALLOC(SDL_CreateRGBSurfaceWithFormat, &mgr->surf,/**/ 0, c->w*s, c->h*s, 32, SDL_PIXELFORMAT_RGBA32);
            rerender = 1;
            if(!mgr->surf) return;
        }
        else if(mgr->rerender)
        {
            // clear buffer
            memset(mgr->surf->pixels, 0, 4*mgr->surf->w*mgr->surf->h);
            rerender = 1;
        }

        if(rerender)
        {
            (*c->render)(c, mgr, mgr->surf, 0, 0);
            mgr->rerender = 0;
            LAB_GL_Texture_ResizeUpload(&mgr->gl_tex, &mgr->gl_tex_size, mgr->surf);
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        LAB_Vec2I screen_size = { sw, sh };
        LAB_Vec2I pos = { c->x*z*s, sh-c->y*z*s-c->h*z*s };
        LAB_Vec2I tex_size = { c->w, c->h };
        LAB_RenderSurface_At(r, mgr->gl_tex, screen_size, pos, tex_size, z*s, LAB_COLOR_WHITE);
    }
}

bool LAB_GuiManager_HandleEvent(LAB_GuiManager* mgr, SDL_Event* event)
{
    if(mgr->component)
    {
        int z = mgr->zoom;
        int s = mgr->scale;

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
        if(LAB_Event_GetMouseCoordPtr(event, &x, &y))
        {
            *x /= z*s;
            *y /= z*s;

            bool is_drag = LAB_IsDragEvent(event);
            if(!is_drag && !LAB_GuiHitTest(mgr->component, *x, *y))
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
