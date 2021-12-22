#include "LAB.h"

#include "LAB_stdinc.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "LAB_error.h"
#include "LAB_obj.h"

#include "LAB_blocks.h"
#include "LAB_builtin_blocks.h"


// MAIN
static int LAB_InitSDL(void)
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

        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH); // Don't care if it fails
        return true;
    );););
    return false;
}

static void LAB_QuitSDL(void)
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


bool LAB_Init()
{
    LAB_OBJ(LAB_InitSDL(),
            LAB_QuitSDL(),
    
    LAB_OBJ(LAB_Blocks_Init(),
            LAB_Blocks_Quit(),
            
        return true;
    ););
    return false;
}

void LAB_Quit()
{
    LAB_Blocks_Quit();
    LAB_QuitSDL();
}