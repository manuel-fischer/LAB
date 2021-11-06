#include "LAB/gui/component.h"
#include "LAB_debug.h"

bool LAB_Gui_OnEvent_Ignore(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
{
    return false;
}


void LAB_GuiComponent_Destroy_Ignore(LAB_GuiComponent* self)
{
}
