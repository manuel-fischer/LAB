#pragma once

#include <stdbool.h>

typedef struct LAB_View LAB_View;
typedef struct LAB_Window LAB_Window;
typedef union SDL_Event SDL_Event;

#define LAB_CLIP_AX 1

typedef struct LAB_Input
{
    LAB_View* view;
    //int dir_set;
    //int updown;
    unsigned flags;
    unsigned mouse_grabbed : 1;
    unsigned state;
    float speed;
    unsigned brushsize;
    LAB_BlockID selected_block;

    bool space_pressed;
    bool graph_selected;
    uint32_t prev_space_down;
} LAB_Input;

#define LAB_VIEWINPUT_FORWARD 1u
#define LAB_VIEWINPUT_DESTROY 2u
#define LAB_VIEWINPUT_CREATE  4u

#define LAB_VIEWINPUT_NOCLIP  8u


/**
 *  Create input, with given view
 */
bool LAB_Input_Create(LAB_Input* input, LAB_View* view);

/**
 *  Destruct input
 */
void LAB_Input_Destroy(LAB_Input* input);

int LAB_Input_OnEvent_Proc(void* user, LAB_Window* window, SDL_Event* event);
void LAB_Input_Tick(LAB_Input* input, uint32_t delta_ms);
