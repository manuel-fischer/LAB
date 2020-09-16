#include "LAB_gui_label.h"

#include "LAB_gui.h"

void LAB_GuiLabel_Create(LAB_GuiLabel* label,
                         int x, int y, int w, int h,
                         const char* title)
{
    label->x = x; label->y = y;
    label->w = w; label->h = h;

    label->on_event = &LAB_Gui_OnEvent_Ignore;
    label->render = &LAB_GuiLabel_Render;

    label->title = title;
    label->text_surf = NULL;
}


void LAB_GuiLabel_Render(LAB_GuiComponent* self, SDL_Surface* surf,
                         int x, int y)
{
    LAB_GuiLabel* cself = (LAB_GuiLabel*)self;

    if(!cself->text_surf)
    {
        SDL_Color fg = { 255, 255, 255, 255 };
        TTF_Font* ttf = LAB_GuiTitleFont();
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
