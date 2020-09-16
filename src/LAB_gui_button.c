#include "LAB_gui_button.h"

#include "LAB_gui.h"

void LAB_GuiButton_Create(LAB_GuiButton* button,
                          int x, int y, int w, int h,
                          const char* title,
                          void(*on_click)(void* user),
                          void* user)
{
    button->x = x; button->y = y;
    button->w = w; button->h = h;

    button->on_event = &LAB_GuiButton_OnEvent;
    button->render = &LAB_GuiButton_Render;

    button->title = title;
    button->text_surf = NULL;

    button->on_click = on_click;
    button->user = user;

}


void LAB_GuiButton_Render(LAB_GuiComponent* self, SDL_Surface* surf,
                          int x, int y)
{
    LAB_GuiButton* cself = (LAB_GuiButton*)self;

    LAB_RenderRect(surf, x, y, self->w, self->h, 0, 1);

    if(!cself->text_surf)
    {
        SDL_Color fg = { 255, 255, 255, 255 };
        TTF_Font* ttf = LAB_GuiFont();
        if(!ttf) return;
        //cself->text_surf = TTF_RenderUTF8_Solid(ttf, cself->title, fg);
        cself->text_surf = TTF_RenderUTF8_Blended(ttf, cself->title, fg);
        if(!cself->text_surf) return;
    }
    SDL_Rect dst;

    dst.x = x + (self->w-cself->text_surf->w)/2;
    dst.y = y + (self->h-cself->text_surf->h)/2;
    dst.w = cself->text_surf->w;
    dst.h = cself->text_surf->h;

    SDL_BlitSurface(cself->text_surf, NULL, surf, &dst);
}



int LAB_GuiButton_OnEvent(LAB_GuiComponent* self, SDL_Event* event)
{
    LAB_GuiButton* cself = (LAB_GuiButton*)self;
    switch(event->type)
    {
        case SDL_MOUSEBUTTONUP:
        {
            cself->on_click(cself->user);
        } break;

        case SDL_KEYUP:
        {
            if(event->key.keysym.sym == SDLK_RETURN)
                cself->on_click(cself->user);
        } break;
    }
    return 0;
}
