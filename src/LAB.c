#include "LAB.h"

#include "LAB_stdinc.h"
#include <SDL2/SDL.h>


// MAIN
int LAB_Init()
{
    uint32_t sdl_subsystems = 0u;

    sdl_subsystems |= SDL_INIT_AUDIO;
    sdl_subsystems |= SDL_INIT_VIDEO;
    sdl_subsystems |= SDL_INIT_EVENTS;

    SDL_Init(sdl_subsystems);


    return 0;
}

void LAB_Quit()
{
    SDL_Quit();
}
