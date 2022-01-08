#pragma once

#include "LAB_stdinc.h"
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include "LAB_sdl.h"

struct LAB_TextRenderer_Line
{
    size_t spacing;
    SDL_Surface* surf;
    struct LAB_TextRenderer_Line* next;
};

typedef struct LAB_TextRenderer
{
    TTF_Font* font;
    struct LAB_TextRenderer_Line* lines,** lines_tail;
    int w, h;

    int pending_spacing;

} LAB_TextRenderer;

bool LAB_TextRenderer_Create(LAB_TextRenderer* r, TTF_Font* font);
void LAB_TextRenderer_Destroy(LAB_TextRenderer* r);

SDL_Surface* LAB_TextRenderer_Render(LAB_TextRenderer* r);

bool LAB_TextRenderer_Println(LAB_TextRenderer* r, const char* line);
bool LAB_TextRenderer_Printfln(LAB_TextRenderer* r, const char* fmt, ...);

void LAB_TextRenderer_VSpace(LAB_TextRenderer* r, float percentage);