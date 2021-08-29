#pragma once
#include "LAB_stdinc.h"

typedef struct LAB_Window LAB_Window;
typedef union SDL_Event SDL_Event;

/**
 *  Return 0 to quit
 */
typedef int(LAB_WindowEventHandler)(void* user, LAB_Window* window, SDL_Event* event);
typedef void(LAB_WindowRenderer)(void* user, LAB_Window* window);

typedef struct LAB_Window
{
    struct SDL_Window*      window;
    struct SDL_Renderer*    renderer;
    void*/*SDL_GLContext*/  glcontext;

    LAB_WindowEventHandler* onevent;
    void*                   onevent_user;

    LAB_WindowRenderer* render;
    void*               render_user;
} LAB_Window;

/**
 *  Construct window with width and height,
 *  Pass sdl_flags to SDL_CreateWindowAndRenderer
 *  return 0 on error
 */
int  LAB_Window_Create(LAB_Window* window, int w, int h, uint32_t sdl_flags);

/**
 *  Destruct window and its SDL resources
 */
void LAB_Window_Destroy(LAB_Window* window);


/**
 *  Return 0 if window has been closed
 */
int LAB_WindowLoop(LAB_Window* window);
