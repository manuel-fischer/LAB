#include "LAB/gui/button.h"

#include "LAB/gui.h"
#include "LAB_sdl.h"
#include "LAB_attr.h"


enum LAB_GuiButtonState
{
    LAB_GUI_BUTTON_NORMAL = 0,
    LAB_GUI_BUTTON_FOCUSED,
    LAB_GUI_BUTTON_PRESSED,
};


LAB_STATIC
void LAB_GuiButton_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                          int x, int y);

LAB_STATIC
bool LAB_GuiButton_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);

LAB_STATIC
void LAB_GuiButton_Destroy(LAB_GuiComponent* self);


void LAB_GuiButton_Create(LAB_GuiButton* button, LAB_GuiButton_Spec spec)
{
    LAB_GuiComponent_SetRect(button, spec.rect);

    button->on_event = &LAB_GuiButton_OnEvent;
    button->render = &LAB_GuiButton_Render;
    button->destroy = &LAB_GuiButton_Destroy;

    button->title = spec.title;
    button->text_surf = NULL;

    button->on_click = spec.on_click;
    button->ctx = spec.ctx;

    button->state = LAB_GUI_BUTTON_NORMAL;
}


void LAB_GuiButton_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                          int x, int y)
{
    int s = mgr->scale;

    LAB_GuiButton* cself = (LAB_GuiButton*)self;

    int cx = cself->state;
    LAB_RenderRect(surf, s, s*x, s*y, s*self->w, s*self->h, cx, 1);

    if(!cself->text_surf)
    {
        SDL_Color fg = { 255, 255, 255, 255 };
        LAB_SDL_ALLOC(TTF_RenderUTF8_Blended, &cself->text_surf, mgr->button_font, cself->title, fg);
        if(!cself->text_surf) return;
    }
    SDL_Rect dst;

    dst.x = s*x + (s*self->w-cself->text_surf->w)/2;
    dst.y = s*y + (s*self->h-cself->text_surf->h)/2;
    dst.w = cself->text_surf->w;
    dst.h = cself->text_surf->h;
    if(cself->state == LAB_GUI_BUTTON_PRESSED)
        dst.y+=s;

    SDL_BlitSurface(cself->text_surf, NULL, surf, &dst);
}



bool LAB_GuiButton_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiButton* cself = (LAB_GuiButton*)self;
    switch(event->type)
    {
        case SDL_MOUSEBUTTONDOWN:
        {
            cself->state = LAB_GUI_BUTTON_PRESSED;
            return 1;
        } break;
        case SDL_MOUSEBUTTONUP:
        {
            if(cself->state == LAB_GUI_BUTTON_PRESSED)
            {
                cself->on_click(cself->ctx, mgr);
                cself->state = LAB_GUI_BUTTON_FOCUSED;
                return 1;
            }
        } break;

        case SDL_KEYDOWN:
        {
            if(event->key.keysym.sym == SDLK_RETURN)
            {
                cself->state = LAB_GUI_BUTTON_PRESSED;
                return 1;
            }
        } break;
        case SDL_KEYUP:
        {
            if(event->key.keysym.sym == SDLK_RETURN)
            {
                if(cself->state == LAB_GUI_BUTTON_PRESSED)
                {
                    cself->on_click(cself->ctx, mgr);
                    cself->state = LAB_GUI_BUTTON_FOCUSED;
                    return 1;
                }
            }
        } break;
    }
    if(LAB_IS_GUI_EVENT(event->type))
    {
        switch(LAB_GUI_EVENT(event->type))
        {
            case LAB_GUI_EVENT_FOCUS:
            {
                if(cself->state != LAB_GUI_BUTTON_FOCUSED)
                {
                    cself->state = LAB_GUI_BUTTON_FOCUSED;
                    return 1;
                }
            } break;
            case LAB_GUI_EVENT_UNFOCUS:
            {
                if(cself->state != LAB_GUI_BUTTON_NORMAL)
                {
                    cself->state = LAB_GUI_BUTTON_NORMAL;
                    return 1;
                }
            } break;
        }
    }
    return 0;
}

void LAB_GuiButton_Destroy(LAB_GuiComponent* self)
{
    LAB_GuiButton* cself = (LAB_GuiButton*)self;
    LAB_SDL_FREE(SDL_FreeSurface, &cself->text_surf);
    //LAB_Free(cself->content);
}
