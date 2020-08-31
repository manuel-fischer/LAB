#include "LAB.h"

#include "LAB_stdinc.h"
#include <SDL2/SDL.h>
#include "LAB_error.h"

// MAIN
int LAB_Init(void)
{
    uint32_t sdl_subsystems = 0u;

    sdl_subsystems |= SDL_INIT_AUDIO;
    sdl_subsystems |= SDL_INIT_VIDEO;
    sdl_subsystems |= SDL_INIT_EVENTS;

    if(SDL_Init(sdl_subsystems) != 0)
        return (LAB_SetError("SDL_Init failed %s", SDL_GetError()), 0);

    if(TTF_Init() != 0)
        return (LAB_SetError("TTF_Init failed %s", SDL_GetError()), 0);

    return 1;
}

void LAB_Quit(void)
{
    SDL_Quit();
}
