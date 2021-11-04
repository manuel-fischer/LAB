#include "LAB/gui/label.h"

#include "LAB/gui.h"
#include "LAB_sdl.h"

void LAB_GuiLabel_Create(LAB_GuiLabel* label,
                         int x, int y, int w, int h,
                         const char* title)
{
    label->x = x; label->y = y;
    label->w = w; label->h = h;

    label->on_event = &LAB_Gui_OnEvent_Ignore;
    label->render = &LAB_GuiLabel_Render;
    label->destroy = &LAB_GuiLabel_Destroy;

    label->title = title;
    label->text_surf = NULL;
}


void LAB_GuiLabel_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                         int x, int y)
{
    LAB_GuiLabel* cself = (LAB_GuiLabel*)self;

    int s = mgr->scale;

    if(!cself->text_surf)
    {
        SDL_Color fg = { 255, 255, 255, 255 };
        LAB_SDL_ALLOC(TTF_RenderUTF8_Blended, &cself->text_surf, mgr->title_font, cself->title, fg);
        if(!cself->text_surf) return;
    }
    SDL_Rect dst;

    dst.x = s*x + (s*self->w-cself->text_surf->w)/2;
    dst.y = s*y + (s*self->h-cself->text_surf->h)/2;
    dst.w = cself->text_surf->w;
    dst.h = cself->text_surf->h;

    SDL_BlitSurface(cself->text_surf, NULL, surf, &dst);
}

void LAB_GuiLabel_Destroy(LAB_GuiComponent* self)
{
    LAB_GuiLabel* cself = (LAB_GuiLabel*)self;
    LAB_SDL_FREE(SDL_FreeSurface, &cself->text_surf);
}
