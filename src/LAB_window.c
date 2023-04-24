#include "LAB_window.h"

#include "LAB_memory.h"
#include "LAB_error.h"
#include "LAB_opt.h"

#include <SDL2/SDL.h>
#include "LAB_gl.h"
#include "LAB_util.h" // nanos


int LAB_Window_Create(LAB_Window* window, int w, int h, uint32_t sdl_flags)
{
    memset(window, 0, sizeof *window);

    if(SDL_CreateWindowAndRenderer(w, h, sdl_flags, &window->window, &window->renderer) != 0)
    {
        LAB_SetError("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        goto INIT_ERROR;
    }

    SDL_SetWindowTitle(window->window, "LAB");

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);


    window->glcontext = SDL_GL_CreateContext(window->window);
    if(window->glcontext == NULL)
    {
        LAB_SetError("SDL_GL_CreateContext failed: %s", SDL_GetError());
        goto INIT_ERROR;
    }

    //  0 for immediate updates,
    //  1 for updates synchronized with the vertical retrace,
    // -1 for adaptive vsync
    #if 0
    SDL_GL_SetSwapInterval(0);
    #else
    if(SDL_GL_SetSwapInterval(-1) < 0) // Adaptive VSync
    {
        if(SDL_GL_SetSwapInterval(1) < 0) // VSync
        {
            LAB_SetError("SDL_GL_SetSwapInterval failed twice: %s", SDL_GetError());
            goto INIT_ERROR;
        }
    }
    #endif

#ifdef LAB_USES_GLEW
    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        LAB_SetError("glewInit failed: %s", glewGetErrorString(err));
        goto INIT_ERROR;
    }
#endif

    return 1;

INIT_ERROR:
    LAB_Window_Destroy(window);
    return 0;
}


void LAB_Window_Destroy(LAB_Window* window)
{
    if(window->glcontext != NULL) SDL_GL_DeleteContext(window->glcontext);

    if(window->window != NULL)    SDL_DestroyWindow(window->window);
    if(window->renderer != NULL)  SDL_DestroyRenderer(window->renderer);
}


int LAB_WindowLoop(LAB_Window* window)
{
    SDL_Event event;
    //LAB_Nanos stoptime = LAB_NanoSeconds() + 1000*1000;
    while (SDL_PollEvent(&event) == 1) {
        if(event.type == SDL_QUIT) {
            return 0;
        }
        if(LAB_LIKELY(window->onevent != NULL))
            if(window->onevent(window->onevent_user, window, &event) == 0)
                return 0;

        //if(stoptime < LAB_NanoSeconds()) break;
    }

    if(LAB_LIKELY(window->render != NULL))
    {
        //uint64_t t0 = LAB_NanoSeconds();
        // TODO: pass w, h to hook, remove glViewport from here
        int w, h;
        SDL_GetWindowSize(window->window, &w, &h);
        glViewport(0, 0, w, h);
        window->render(window->render_user, window);

        //uint64_t t1 = LAB_NanoSeconds();
        SDL_GL_SwapWindow(window->window);
        //uint64_t t2 = LAB_NanoSeconds();
        //printf("%14i %14i\n", (int)(t1-t0), (int)(t2-t1));
    }

    return 1;
}
