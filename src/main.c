#include <stdlib.h>
#include <stdio.h>
#include "LAB.h"
#include <SDL2/SDL.h>

#include <math.h>
#include "LAB_bits.h"

#include "LAB_memory.h"

#include "LAB_gen_flat.h"
#include "LAB_gen_overworld.h"

int main(int argc, char** argv) {
    #define CHECK_INIT(expr) if(expr) { init_msg = #expr; goto INIT_ERROR; }

    char const* init_msg;
    int return_value;

    int init = 0;
    LAB_Window*    main_window = NULL;
    LAB_World*     the_world   = NULL;
    LAB_View*      world_view  = NULL;
    LAB_ViewInput  view_input  = {};
    LAB_GenFlat    gen_flat    = {};

    CHECK_INIT(LAB_Init() != 0);
    init = 1;


    uint32_t sdl_window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    main_window = LAB_CreateWindow(1024, 576, sdl_window_flags);
    CHECK_INIT(main_window == NULL);
    //SDL_SetWindowFullscreen(main_window->window, SDL_WINDOW_FULLSCREEN);


    the_world = LAB_CreateWorld();
    CHECK_INIT(the_world == NULL);

    gen_flat.block = &LAB_BLOCK_STONE;
    the_world->chunkgen      = &LAB_GenFlatProc;
    the_world->chunkgen_user = &gen_flat;

    world_view = LAB_CreateView(the_world);
    CHECK_INIT(world_view == NULL);
    world_view->dist = 5;

    view_input.view = world_view;

    the_world->chunkview      = &LAB_ViewChunkProc;
    the_world->chunkview_user = world_view;


    main_window->onevent      = &LAB_ViewInputOnEventProc;
    main_window->onevent_user = &view_input;

    main_window->render       = &LAB_ViewRenderProc;
    main_window->render_user  = world_view;


    while(LAB_WindowLoop(main_window))
    {
        /*LAB_Chunk* chunks[27];
        LAB_GetChunkNeighborhood(the_world, chunks, (int)(world_view->x / LAB_CHUNK_SIZE),
                                                    (int)(world_view->y / LAB_CHUNK_SIZE),
                                                    (int)(world_view->z / LAB_CHUNK_SIZE),
                                                    LAB_CHUNK_GENERATE);*/

        LAB_ViewLoadNearChunks(world_view);

        LAB_WorldTick(the_world);
        LAB_ViewInputTick(&view_input);
    };


    return_value = EXIT_SUCCESS;
    //goto EXIT;

EXIT:
    if(world_view != NULL)  LAB_DestroyView(world_view);
    if(the_world != NULL)   LAB_DestroyWorld(the_world);
    if(main_window != NULL) LAB_DestroyWindow(main_window);

    if(init) LAB_Quit();

    LAB_DbgMemShow();

    return return_value;


INIT_ERROR:
    fprintf(stderr, "Lab-Initialization failed: %s: %s\n", init_msg, LAB_GetError());

    return_value = EXIT_FAILURE;
    goto EXIT;
}
