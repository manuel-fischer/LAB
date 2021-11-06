#include "LAB/gui/list_box.h"
#include "LAB_memory.h"
#include "LAB_sdl.h"
#include "LAB_attr.h"

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


void LAB_GuiListBox_Create(LAB_GuiListBox* cself,
                          int x, int y, int w, int h,
                          size_t element_count, const char* const* elements,
                          void (*on_selection)(void* user, size_t element),
                          void*  on_selection_user)
{
    cself->x = x; cself->y = y;
    cself->w = w; cself->h = h;

    cself->on_event = &LAB_GuiListBox_OnEvent;
    cself->render = &LAB_GuiListBox_Render;
    cself->destroy = &LAB_GuiListBox_Destroy;

    //cself->content_capacity = 0;
    //cself->content = NULL;
    cself->element_count = element_count;
    cself->elements = elements;
    cself->text_surfs = LAB_Calloc(sizeof(cself->text_surfs[0]), element_count);

    cself->selected_element = ~0;

    cself->on_selection = on_selection;
    cself->on_selection_user = on_selection_user;

    cself->state = LAB_GUI_LIST_BOX_NORMAL;
}


LAB_STATIC
void LAB_GuiListBox_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                           int x, int y)
{
    LAB_GuiListBox* cself = (LAB_GuiListBox*)self;

    int s = mgr->scale;

    int cx = cself->state;
    LAB_RenderRect(surf, s, s*x, s*y, s*self->w, s*self->h, cx, 0);

    if(cself->selected_element < cself->element_count)
    {
        LAB_RenderRect(surf, s, s*(x+3), s*(y+3+cself->selected_element*LAB_GUI_LIST_BOX_ELEMENT_HEIGHT), s*(self->w-6), s*(LAB_GUI_LIST_BOX_ELEMENT_HEIGHT), 2, 0);
    }

    for(size_t i = 0; i < cself->element_count; ++i)
    {
        if(!cself->text_surfs[i])
        {
            SDL_Color fg = { 255, 255, 255, 255 };
            TTF_Font* ttf = mgr->button_font;
            if(!ttf) return;
            LAB_SDL_ALLOC(TTF_RenderUTF8_Blended, &cself->text_surfs[i], ttf, cself->elements[i], fg);
            if(!cself->text_surfs[i]) return;
        }

        SDL_Rect dst;

        dst.x = s*(x + 5+1);
        dst.y = s*(y + 3 + i*LAB_GUI_LIST_BOX_ELEMENT_HEIGHT) + (s*LAB_GUI_LIST_BOX_ELEMENT_HEIGHT-cself->text_surfs[i]->h)/2;
        dst.w = s*cself->text_surfs[i]->w;
        dst.h = s*cself->text_surfs[i]->h;

        SDL_BlitSurface(cself->text_surfs[i], NULL, surf, &dst);
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
            cself->selected_element = (event->button.y-3) / LAB_GUI_LIST_BOX_ELEMENT_HEIGHT;
            if(cself->on_selection) cself->on_selection(cself->on_selection_user, cself->selected_element);
            return 1;
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
