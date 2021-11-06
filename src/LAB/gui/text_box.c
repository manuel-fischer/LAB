#include "LAB/gui/text_box.h"
#include "LAB_memory.h"
#include "LAB_sdl.h"
#include "LAB_attr.h"

// TODO sel_start & sel_len

enum LAB_GuiTextBoxState
{
    LAB_GUI_TEXT_BOX_NORMAL = 0,
    LAB_GUI_TEXT_BOX_FOCUSED,
};

LAB_STATIC
void LAB_GuiTextBox_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                          int x, int y);

LAB_STATIC
bool LAB_GuiTextBox_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);

LAB_STATIC
void LAB_GuiTextBox_Destroy(LAB_GuiComponent* self);


void LAB_GuiTextBox_Create(LAB_GuiTextBox* cself,
                          int x, int y, int w, int h)
{
    cself->x = x; cself->y = y;
    cself->w = w; cself->h = h;

    cself->on_event = &LAB_GuiTextBox_OnEvent;
    cself->render = &LAB_GuiTextBox_Render;
    cself->destroy = &LAB_GuiTextBox_Destroy;

    cself->content_capacity = 0;
    cself->content = NULL;

    cself->text_surf = NULL;

    cself->state = LAB_GUI_TEXT_BOX_NORMAL;
}

/**
 *  Return 0 on failure
 */
LAB_STATIC bool LAB_GuiTextBox_EnsureCapacity(LAB_GuiTextBox* cself, size_t min_capacity)
{
    if(min_capacity > cself->content_capacity) // new character + '\0'
    {
        size_t new_capacity;
        if(cself->content_capacity == 0)
            new_capacity = 16;
        else
            new_capacity = cself->content_capacity;

        while(min_capacity > new_capacity)
            new_capacity *= 2;

        char* new_content = LAB_Realloc(cself->content, new_capacity);
        if(new_content == NULL)
            return 0;

        cself->content = new_content;
    }
    return 1;
}

LAB_STATIC void LAB_GuiTextBox_UpdateText(LAB_GuiTextBox* cself)
{
    LAB_SDL_FREE(SDL_FreeSurface, &cself->text_surf);
    cself->text_surf = NULL;
}

LAB_STATIC
void LAB_GuiTextBox_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                           int x, int y)
{
    LAB_GuiTextBox* cself = (LAB_GuiTextBox*)self;

    int s = mgr->scale;

    int cx = cself->state;
    LAB_RenderRect(surf, s, s*x, s*y, s*self->w, s*self->h, cx, 0);

    int cursor_left = s*5;
    if(cself->content != NULL && cself->content[0] != '\0')
    {
        if(!cself->text_surf)
        {
            SDL_Color fg = { 255, 255, 255, 255 };
            TTF_Font* ttf = mgr->button_font;
            if(!ttf) return;
            LAB_SDL_ALLOC(TTF_RenderUTF8_Blended, &cself->text_surf, ttf, cself->content, fg);
            if(!cself->text_surf) return;
        }
        SDL_Rect dst, src;

        dst.x = s*(x + 5);
        dst.y = s*y + (s*self->h-cself->text_surf->h)/2;
        dst.w = cself->text_surf->w;
        dst.h = cself->text_surf->h;

        SDL_Rect* src_p;
        int max_text_width = s*(cself->w-10-1); // 1 additional pixel for cursor
        if(cself->text_surf->w > max_text_width)
        {
            src.x = cself->text_surf->w-max_text_width;
            src.y = 0;

            src.w = max_text_width;
            src.h = cself->text_surf->h;
            src_p = &src;
        }
        else
        {
            src_p = NULL;
        }

        SDL_BlitSurface(cself->text_surf, src_p, surf, &dst);

        cursor_left += dst.w;
    }

    if(cself->state == LAB_GUI_TEXT_BOX_FOCUSED)
    {
        SDL_Rect dst;

        dst.x = s*x + cursor_left;
        dst.y = s*(y + 5);
        dst.w = s*1;
        dst.h = s*(self->h - 10);

        Uint32 cursor_color = 0xffffffff;

        SDL_FillRect(surf, &dst, cursor_color);
    }
}

LAB_STATIC
bool LAB_GuiTextBox_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiTextBox* cself = (LAB_GuiTextBox*)self;
    switch(event->type)
    {
        case SDL_KEYDOWN:
        {
            SDL_KeyboardEvent* key = &event->key;
            if(key->keysym.scancode == SDL_SCANCODE_BACKSPACE)
            {
                if(cself->content != NULL)
                {
                    size_t slen = strlen(cself->content);
                    if(slen != 0)
                    {
                        for(--slen; (cself->content[slen]&0xC0) == 0x80; --slen)
                            cself->content[slen] = '\0';
                        cself->content[slen] = '\0';
                        LAB_GuiTextBox_UpdateText(cself);
                        return 1;
                    }
                }
            }
        } break;
        case SDL_TEXTINPUT:
        {
            SDL_TextInputEvent* txt = &event->text;
            char* mb_char = txt->text;
            //if(mb_char[0] == 1) return 0;
            size_t charlen = strlen(mb_char);
            size_t slen;

            if(cself->content != NULL)
                slen = strlen(cself->content);
            else
                slen = 0;

            if(!LAB_GuiTextBox_EnsureCapacity(cself, slen+charlen+1)) return 0;

            //strcat(cself->content, mb_char);
            memcpy(cself->content+slen, mb_char, charlen+1);
            LAB_GuiTextBox_UpdateText(cself);
            return 1;
        } break;
    }
    if(LAB_IS_GUI_EVENT(event->type))
    {
        switch(LAB_GUI_EVENT(event->type))
        {
            case LAB_GUI_EVENT_FOCUS:
            {
                if(cself->state != LAB_GUI_TEXT_BOX_FOCUSED)
                {
                    cself->state = LAB_GUI_TEXT_BOX_FOCUSED;
                    return 1;
                }
            } break;
            case LAB_GUI_EVENT_UNFOCUS:
            {
                if(cself->state != LAB_GUI_TEXT_BOX_NORMAL)
                {
                    cself->state = LAB_GUI_TEXT_BOX_NORMAL;
                    return 1;
                }
            } break;
        }
    }
    return 0;
}

LAB_STATIC
void LAB_GuiTextBox_Destroy(LAB_GuiComponent* self)
{
    LAB_GuiTextBox* cself = (LAB_GuiTextBox*)self;
    LAB_SDL_FREE(SDL_FreeSurface, &cself->text_surf);
    LAB_Free(cself->content);
}



bool LAB_GuiTextBox_SetContent(LAB_GuiTextBox* cself, const char* txt)
{
    size_t slen = strlen(txt);
    if(!LAB_GuiTextBox_EnsureCapacity(cself, slen+1)) return 0;

    memcpy(cself->content, txt, slen+1);

    LAB_GuiTextBox_UpdateText(cself);
    return 1;
}
