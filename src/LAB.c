#include "LAB.h"

#include "LAB_stdinc.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "LAB_error.h"
#include "LAB_obj.h"

// MAIN
int LAB_Init(void)
{
    uint32_t sdl_subsystems = 0u;

    sdl_subsystems |= SDL_INIT_AUDIO;
    sdl_subsystems |= SDL_INIT_VIDEO;
    sdl_subsystems |= SDL_INIT_EVENTS;

    LAB_OBJ_SDL(SDL_Init(sdl_subsystems) == 0,
                SDL_Quit(),
    
    LAB_OBJ_SDL(IMG_Init(IMG_INIT_PNG) != 0,
                IMG_Quit(),

    LAB_OBJ_SDL(TTF_Init() == 0,
                TTF_Quit(),

        return 1;
    );););
    return 0;
}

void LAB_Quit(void)
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
