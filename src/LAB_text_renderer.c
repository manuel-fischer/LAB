#include "LAB_text_renderer.h"

#include "LAB_memory.h"
#include <stdio.h> // snprintf

bool LAB_TextRenderer_Create(LAB_TextRenderer* r, TTF_Font* font)
{
    r->font = font;
    r->lines = NULL;
    r->lines_tail = &r->lines;
    r->w = 0;
    r->h = 0;

    r->pending_spacing = 0;

    return true;
}

void LAB_TextRenderer_Destroy(LAB_TextRenderer* r)
{
    struct LAB_TextRenderer_Line* l, *nl;
    for(l = r->lines; l != NULL; l = nl)
    {
        nl = l->next;

        LAB_SDL_FREE(SDL_FreeSurface, &l->surf);
        LAB_Free(l);
    }
}

SDL_Surface* LAB_TextRenderer_Render(LAB_TextRenderer* r)
{
    SDL_Surface* surf;
    LAB_SDL_ALLOC(SDL_CreateRGBSurfaceWithFormat, &surf, 0, r->w, r->h, 32, SDL_PIXELFORMAT_RGBA32);
    if(!surf) return NULL;

    size_t y = 0;
    struct LAB_TextRenderer_Line* l;
    for(l = r->lines; l != NULL; l = l->next)
    {   
        y += l->spacing;

        SDL_Rect dst;
        dst.x = 0;
        dst.y = y;
        dst.w = l->surf->w;
        dst.h = l->surf->h;
        SDL_BlitSurface(l->surf, NULL, surf, &dst);

        y += l->surf->h;
    }
    return surf;
}

bool LAB_TextRenderer_Println(LAB_TextRenderer* r, const char* line)
{
    if(!line || !*line)
    {
        LAB_TextRenderer_VSpace(r, 1);
        return true;
    }

    struct LAB_TextRenderer_Line* l = LAB_Malloc(sizeof*l);
    if(!l) return false;



    SDL_Color fg = { 255, 255, 255, 255 };
    SDL_Color bg = {   0,   0,   0, 255 };

    LAB_SDL_ALLOC(TTF_RenderUTF8_Shaded, &l->surf, r->font, line, fg, bg);
    if(!l->surf)
    {
        LAB_Free(l);
        return false;
    }

    // Link
    l->next = NULL;
    *r->lines_tail = l;
    r->lines_tail = &l->next;

    // Layout
    l->spacing = r->pending_spacing;
    r->pending_spacing = 0;

    if(l->surf->w > r->w) r->w = l->surf->w;
    r->h += l->surf->h + l->spacing;

    return true;
}

bool LAB_TextRenderer_Printfln(LAB_TextRenderer* r, const char* fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    return LAB_TextRenderer_Println(r, buf);
}

void LAB_TextRenderer_VSpace(LAB_TextRenderer* r, float percentage)
{
    int height = TTF_FontHeight(r->font);
    r->pending_spacing += height*percentage;
}