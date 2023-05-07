#include "LAB/gui/vscroll.h"
#include "LAB/gui/util.h"

LAB_STATIC
void LAB_GuiVScroll_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                           int x, int y);

LAB_STATIC
bool LAB_GuiVScroll_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event);


void LAB_GuiVScroll_Create(LAB_GuiVScroll* cself, LAB_GuiScroll_Spec spec)
{
    LAB_GuiComponent_SetRect(cself, spec.rect);

    cself->state = 0;

    cself->total_height    = spec.total;
    cself->viewport_height = spec.viewport;
    cself->scroll_value    = 0;

    cself->on_scroll = spec.on_scroll;
    cself->on_scroll_ctx = spec.on_scroll_ctx;

    cself->on_event = &LAB_GuiVScroll_OnEvent;
    cself->render = &LAB_GuiVScroll_Render;
    cself->destroy = &LAB_GuiComponent_Destroy_Ignore;
}

LAB_STATIC
int LAB_GuiVScroll_RowCount(LAB_GuiVScroll* cself)
{
    return LAB_MAX(1, cself->total_height-cself->viewport_height+1);
}

LAB_STATIC
int LAB_GuiVScroll_ButtonHeight(LAB_GuiVScroll* cself)
{
    return LAB_MIN(cself->h, LAB_MAX(cself->h*cself->viewport_height/cself->total_height, 10));
}

LAB_STATIC
void LAB_GuiVScroll_Render(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf,
                           int x, int y)
{
    LAB_GuiVScroll* cself = (LAB_GuiVScroll*)self;

    const LAB_Vec2Z border = {0, 4};
    const LAB_Vec2Z button = cself->state == 0 ? (LAB_Vec2Z) {1, 4} : (LAB_Vec2Z) {2, 4};

    int s = mgr->scale;

    LAB_Box2I border_rect = LAB_Box2I_New_Sized(x, y, self->w, self->h);
    LAB_RenderRect_Scaled(surf, s, border_rect, border);

    int row_steps = LAB_GuiVScroll_RowCount(cself)+cself->viewport_height-1;
    int button_height = LAB_GuiVScroll_ButtonHeight(cself);
    int button_pos = self->h*cself->scroll_value/row_steps;

    LAB_Box2I button_rect = LAB_Box2I_New_Sized(x, y+button_pos, self->w, button_height);
    button_rect = LAB_Box2I_Expand(button_rect, (LAB_Vec2I) { -1, -1 });
    LAB_RenderRect_Scaled(surf, s, button_rect, button);
}

LAB_STATIC
bool LAB_GuiVScroll_OnEvent(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    LAB_GuiVScroll* cself = (LAB_GuiVScroll*)self;

    bool redraw = false;

    if(event->type == SDL_MOUSEBUTTONDOWN) { cself->state = 2; redraw = true; }
    if(event->type == SDL_MOUSEBUTTONUP || event->type == LAB_GUI_EVENT2SDL(LAB_GUI_EVENT_UNFOCUS)) { cself->state = 0; redraw = true; }

    if(event->type == SDL_MOUSEBUTTONDOWN || (event->type == SDL_MOUSEMOTION && event->motion.state))
    {
        int* mx,* my;
        LAB_GetMouseCoordPtr(event, &mx, &my);


        int row_count = LAB_GuiVScroll_RowCount(cself);
        int row_steps = row_count+cself->viewport_height-1;
        int button_height = LAB_MIN(self->h, LAB_MAX(self->h*cself->viewport_height/row_steps, 10));

        int new_scroll = ((*my - button_height/2)*row_steps + self->h/2)/self->h;
        redraw |= LAB_GuiVScroll_SetScrollValue(cself, new_scroll);
    }
    return redraw;
}


bool LAB_GuiVScroll_SetScrollValue(LAB_GuiVScroll* cself, int new_scroll)
{
    int row_count = LAB_GuiVScroll_RowCount(cself);
    new_scroll = LAB_CLAMP(new_scroll, 0, row_count-1);

    if(new_scroll == cself->scroll_value) return false;

    cself->scroll_value = new_scroll;
    cself->on_scroll(cself->on_scroll_ctx, new_scroll);

    return true;
}