#include "LAB_gui_list_box.h"
#include "LAB_memory.h"

void LAB_GuiListBox_Create(LAB_GuiListBox* cself,
                          int x, int y, int w, int h,
                          size_t element_count, const char* const* elements)
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
    cself->selected_element = 1;

    cself->state = LAB_GUI_LIST_BOX_NORMAL;
}


void LAB_GuiListBox_Render(LAB_GuiComponent* self, SDL_Surface* surf,
                           int x, int y)
{
    LAB_GuiListBox* cself = (LAB_GuiListBox*)self;

    int cx = cself->state;
    LAB_RenderRect(surf, x, y, self->w, self->h, cx, 0);

    if(cself->selected_element < cself->element_count)
    {
        LAB_RenderRect(surf, x+3, y+3+cself->selected_element*LAB_GUI_LIST_BOX_ELEMENT_HEIGHT, self->w-6, LAB_GUI_LIST_BOX_ELEMENT_HEIGHT, 2, 0);
    }

    for(size_t i = 0; i < cself->element_count; ++i)
    {
        if(!cself->text_surfs[i])
        {
            SDL_Color fg = { 255, 255, 255, 255 };
            TTF_Font* ttf = LAB_GuiFont();
            if(!ttf) return;
            cself->text_surfs[i] = TTF_RenderUTF8_Blended(ttf, cself->elements[i], fg);
            if(!cself->text_surfs[i]) return;
        }

        SDL_Rect dst;

        dst.x = x + 5+1;
        dst.y = y + 3 + i*LAB_GUI_LIST_BOX_ELEMENT_HEIGHT + (LAB_GUI_LIST_BOX_ELEMENT_HEIGHT-cself->text_surfs[i]->h)/2;
        dst.w = cself->text_surfs[i]->w;
        dst.h = cself->text_surfs[i]->h;

        SDL_BlitSurface(cself->text_surfs[i], NULL, surf, &dst);
    }
}

bool LAB_GuiListBox_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiListBox* cself = (LAB_GuiListBox*)self;
    switch(event->type)
    {

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

void LAB_GuiListBox_Destroy(LAB_GuiComponent* self)
{
    LAB_GuiListBox* cself = (LAB_GuiListBox*)self;
    for(size_t i = 0; i < cself->element_count; ++i)
    {
        SDL_FreeSurface(cself->text_surfs[i]);
    }
    LAB_Free(cself->text_surfs);
}
