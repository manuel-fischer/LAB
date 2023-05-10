#include "LAB/gui/label.h"

#include "LAB/gui.h"
#include "LAB_sdl.h"
#include "LAB_attr.h"

LAB_INLINE
void LAB_GuiLabel_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                         int x, int y);

LAB_INLINE
void LAB_GuiLabel_Destroy(LAB_GuiComponent* self);


void LAB_GuiLabel_Create(LAB_GuiLabel* label, LAB_GuiLabel_Spec spec)
{
    LAB_GuiComponent_SetRect(label, spec.rect);

    label->on_event = &LAB_Gui_OnEvent_Ignore;
    label->render = &LAB_GuiLabel_Render;
    label->destroy = &LAB_GuiLabel_Destroy;

    label->title = spec.title;
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
