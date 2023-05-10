#include "LAB/gui/list_box.h"
#include "LAB_memory.h"
#include "LAB_sdl.h"
#include "LAB_attr.h"
#include "LAB_vec_sdl.h"

enum LAB_GuiListBoxState
{
    LAB_GUI_LIST_BOX_NORMAL = 0,
    LAB_GUI_LIST_BOX_FOCUSED,
};

LAB_STATIC
void LAB_GuiListBox_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                          int x, int y);

LAB_STATIC
bool LAB_GuiListBox_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);

LAB_STATIC
void LAB_GuiListBox_Destroy(LAB_GuiComponent* self);


void LAB_GuiListBox_Create(LAB_GuiListBox* cself, LAB_GuiListBox_Spec spec)
{
    LAB_GuiComponent_SetRect(cself, spec.rect);

    cself->on_event = &LAB_GuiListBox_OnEvent;
    cself->render = &LAB_GuiListBox_Render;
    cself->destroy = &LAB_GuiListBox_Destroy;

    //cself->content_capacity = 0;
    //cself->content = NULL;
    cself->element_count = spec.element_count;
    cself->elements = spec.elements;
    cself->text_surfs = LAB_Calloc(sizeof(cself->text_surfs[0]), spec.element_count);

    cself->top_element = 0;
    cself->selected_element = ~0;

    cself->on_selection = spec.on_selection;
    cself->ctx = spec.ctx;

    cself->state = LAB_GUI_LIST_BOX_NORMAL;
}


size_t LAB_GuiListBox_Viewport(LAB_GuiListBox* cself)
{
    size_t inner_height = cself->h - 2*LAB_GUI_LIST_BOX_BORDER_INSET;
    return inner_height / LAB_GUI_LIST_BOX_ELEMENT_HEIGHT;
}


LAB_STATIC
LAB_Box2I LAB_GuiListBox_ElementRect(LAB_GuiListBox* cself, int index, int x, int y)
{
    int inset = LAB_GUI_LIST_BOX_BORDER_INSET;
    int height = LAB_GUI_LIST_BOX_ELEMENT_HEIGHT;
    return LAB_Box2I_New_Sized(x+inset, y+inset+index*height, cself->w-2*inset, height);
}


LAB_STATIC
void LAB_GuiListBox_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                           int x, int y)
{
    LAB_GuiListBox* cself = (LAB_GuiListBox*)self;

    int s = mgr->scale;
    size_t viewport_height = LAB_GuiListBox_Viewport(cself);

    int cx = cself->state;
    LAB_RenderRect(surf, s, s*x, s*y, s*self->w, s*self->h, cx, 0);

    if(cself->top_element <= cself->selected_element && cself->selected_element < cself->top_element+viewport_height)
    {
        size_t screen_selected = cself->selected_element-cself->top_element;
        LAB_Box2I rect = LAB_GuiListBox_ElementRect(cself, screen_selected, x, y);
        LAB_Vec2Z tex = { 2, 0 };
        LAB_RenderRect_Scaled(surf, s, rect, tex);
    }

    for(size_t i = 0; i < viewport_height; ++i)
    {
        size_t e = i + cself->top_element;
        if(e >= cself->element_count) break;

        SDL_Surface* txt_surf = cself->text_surfs[e];
        if(txt_surf == NULL)
        {
            SDL_Color fg = { 255, 255, 255, 255 };
            TTF_Font* ttf = mgr->button_font;
            if(!ttf) return;
            LAB_SDL_ALLOC(TTF_RenderUTF8_Blended, &txt_surf, ttf, cself->elements[e], fg);
            if(txt_surf == NULL) return;
            cself->text_surfs[e] = txt_surf;
        }

        LAB_Box2I element_rect = LAB_GuiListBox_ElementRect(cself, i, x, y);
        LAB_Box2I text_rect = LAB_Box2I_New_Sized(
            s*(element_rect.a.x+3),
            (s*(element_rect.a.y+element_rect.b.y) - txt_surf->h)/2,
            txt_surf->w,
            txt_surf->h
        );

        SDL_Rect dst;
        dst = LAB_Box2I2SDL(text_rect);

        SDL_BlitSurface(txt_surf, NULL, surf, &dst);
    }
}

LAB_STATIC
bool LAB_GuiListBox_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiListBox* cself = (LAB_GuiListBox*)self;
    switch(event->type)
    {
        case SDL_MOUSEBUTTONDOWN:
        {
            // negative values get large positive numbers
            int screen_row = (event->button.y-3) / LAB_GUI_LIST_BOX_ELEMENT_HEIGHT;
            if(screen_row < 0 || (size_t)screen_row > LAB_GuiListBox_Viewport(cself)) return false;

            size_t screen_selected = (size_t)screen_row;

            size_t new_selected_element = cself->top_element+screen_selected;
            if(new_selected_element >= cself->element_count) new_selected_element = ~0;
            if(new_selected_element == cself->selected_element) new_selected_element = ~0;
            cself->selected_element = new_selected_element;
            if(cself->on_selection) cself->on_selection(cself->ctx, cself->selected_element);
            return 1;
        } break;

        case SDL_KEYDOWN:
        {
            switch(event->key.keysym.sym)
            {
                case 'j':
                case SDLK_DOWN: // << TODO: BUG
                    if(cself->selected_element == cself->element_count-1) { return false; }
                    if(cself->selected_element < cself->element_count) { cself->selected_element++; }
                    else { cself->selected_element = 0u; }

                    if(cself->on_selection) cself->on_selection(cself->ctx, cself->selected_element);
                return true;

                case 'k':
                case SDLK_UP: // << TODO: BUG
                    if(cself->selected_element == 0u) { return false; }
                    if(cself->selected_element < cself->element_count) { cself->selected_element--; }
                    else { cself->selected_element = cself->element_count-1; }

                    if(cself->on_selection) cself->on_selection(cself->ctx, cself->selected_element);
                return true;

                default: break;
            }
        } break;
    }
    if(LAB_IS_GUI_EVENT(event->type))
    {
        switch(LAB_GUI_EVENT(event->type))
        {
            case LAB_GUI_EVENT_FOCUS:
            {
                if(cself->state != LAB_GUI_LIST_BOX_FOCUSED)
                {
                    cself->state = LAB_GUI_LIST_BOX_FOCUSED;
                    return 1;
                }
            } break;
            case LAB_GUI_EVENT_UNFOCUS:
            {
                if(cself->state != LAB_GUI_LIST_BOX_NORMAL)
                {
                    cself->state = LAB_GUI_LIST_BOX_NORMAL;
                    return 1;
                }
            } break;
        }
    }
    return 0;
}

LAB_STATIC
void LAB_GuiListBox_Destroy(LAB_GuiComponent* self)
{
    LAB_GuiListBox* cself = (LAB_GuiListBox*)self;
    for(size_t i = 0; i < cself->element_count; ++i)
    {
        LAB_SDL_FREE(SDL_FreeSurface, &cself->text_surfs[i]);
    }
    LAB_Free(cself->text_surfs);
}
