#pragma once

#include <SDL2/SDL.h>

#ifdef __WIN32__

typedef struct RECT RECT;
typedef int BOOL;
BOOL ClipCursor(const RECT *lpRect);

static inline void SDL_SetWindowGrab_Fix(SDL_Window* window, SDL_bool grabbed)
{
    // There is a bug in SDL, where ClipCursor is not called, when the window itself
    // is partly outside of the screen. When this happens, the cursor keeps being
    // clipped in the window. This is due to Windows clipping the clipping rect to the
    // visible part on the screen. In the sourcecode this is checked against a cached
    // clipping rect, that is not clipped to the screen
    // The condition is never met:
    // http://hg.libsdl.org/SDL/file/369fea168258/src/video/windows/SDL_windowswindow.c#l968
    SDL_SetWindowGrab(window, grabbed);
    if(!grabbed) ClipCursor(NULL);
}

#else
#define SDL_SetWindowGrab_Fix(window, grabbed) SDL_SetWindowGrab(window, grabbed)
#endif
