#pragma once

#include "LAB/gui.h"
#include <SDL2/SDL.h>
#include "LAB_gl_types.h"
#include "LAB_view_render_surface.h"

typedef struct LAB_GuiManager
{
    LAB_GuiComponent* component;
    LAB_GuiComponent* dismiss_component;
    LAB_GL_Texture gl_tex;
    SDL_Surface* surf;
    bool rerender;
    unsigned scale;
    unsigned zoom;
    //bool component_focused; // TODO: focus/unfocus event should be yielded once

    TTF_Font* button_font,
            * title_font,
            * mono_font,
            * mono_font_small;
} LAB_GuiManager;

bool LAB_GuiManager_Create(LAB_GuiManager* manager);
void LAB_GuiManager_Destroy(LAB_GuiManager* manager);

void LAB_GuiManager_ShowDialog(LAB_GuiManager* manager, LAB_GuiComponent* component);
void LAB_GuiManager_Dismiss(LAB_GuiManager* manager);
void LAB_GuiManager_Tick(LAB_GuiManager* manager);

void LAB_GuiManager_Render(LAB_GuiManager* manager, LAB_SurfaceRenderer* r, int w, int h);

bool LAB_GuiManager_HandleEvent(LAB_GuiManager* manager, SDL_Event* event);
