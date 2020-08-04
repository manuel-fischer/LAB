#pragma once

typedef struct LAB_View LAB_View;
typedef struct LAB_Window LAB_Window;
typedef union SDL_Event SDL_Event;

typedef struct LAB_ViewInput
{
    LAB_View* view;
    int dir_set;
    int updown;
} LAB_ViewInput;


int LAB_ViewInputOnEventProc(void* user, LAB_Window* window, SDL_Event* event);
void LAB_ViewInputTick(LAB_ViewInput* view_input);
