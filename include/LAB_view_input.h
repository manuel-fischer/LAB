#pragma once

typedef struct LAB_View LAB_View;
typedef struct LAB_Window LAB_Window;
typedef union SDL_Event SDL_Event;

typedef struct LAB_ViewInput
{
    LAB_View* view;
    //int dir_set;
    //int updown;
    unsigned flags;
    float speed;
    unsigned brushsize;
    LAB_Block* selected_block;
} LAB_ViewInput;

#define LAB_VIEWINPUT_FORWARD 1u
#define LAB_VIEWINPUT_DESTROY 2u
#define LAB_VIEWINPUT_CREATE  4u

#define LAB_VIEWINPUT_NOCLIP  8u


/**
 *  Create view input, with given world
 */
bool LAB_ConstructViewInput(LAB_ViewInput* view_input, LAB_View* view);

/**
 *  Destruct view input
 *  The view can be filled with 0 bytes
 */
void LAB_DestructViewInput(LAB_ViewInput* view_input);

int LAB_ViewInputOnEventProc(void* user, LAB_Window* window, SDL_Event* event);
void LAB_ViewInputTick(LAB_ViewInput* view_input, uint32_t delta_ms);
