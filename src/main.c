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
    #define CHECK_INIT(expr) if(expr); else { init_msg = #expr; goto INIT_ERROR; }

    char const* init_msg;
    int return_value;

    int init = 0;
    static LAB_Window     main_window = {};
    static LAB_World      the_world   = {};
    static LAB_View       view        = {};
    static LAB_ViewInput  view_input  = {};
    //static LAB_GenFlat    gen_flat    = {};
    static LAB_GenOverworld gen_overworld = {};

    CHECK_INIT(LAB_Init());
    init = 1;


    uint32_t sdl_window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    CHECK_INIT(LAB_ConstructWindow(&main_window, 1024, 576, sdl_window_flags));
    //SDL_SetWindowFullscreen(main_window->window, SDL_WINDOW_FULLSCREEN);


    CHECK_INIT(LAB_ConstructWorld(&the_world));

    /*gen_flat.block = &LAB_BLOCK_STONE;
    the_world->chunkgen      = &LAB_GenFlatProc;
    the_world->chunkgen_user = &gen_flat;*/

    gen_overworld.seed = 0x13579bdf;
    the_world.chunkgen      = &LAB_GenOverworldProc;
    the_world.chunkgen_user = &gen_overworld;

    CHECK_INIT(LAB_ConstructView(&view, &the_world));
    view.render_dist = 5;
    view.preload_dist = view.render_dist+0;
    view.keep_dist = view.render_dist+2;
    view.flags = LAB_VIEW_SHOW_GUI;

    view_input.view = &view;

    the_world.chunkview      = &LAB_ViewChunkProc;
    the_world.chunkview_user = &view;


    main_window.onevent      = &LAB_ViewInputOnEventProc;
    main_window.onevent_user = &view_input;

    main_window.render       = &LAB_ViewRenderProc;
    main_window.render_user  = &view;


    while(LAB_WindowLoop(&main_window))
    {
        LAB_ViewInputTick(&view_input);
        LAB_ViewTick(&view);
        LAB_WorldTick(&the_world);
    };


    return_value = EXIT_SUCCESS;
    //goto EXIT;

EXIT:
    // Null destructable
    LAB_DestructView(&view);
    LAB_DestructWorld(&the_world);
    LAB_DestructWindow(&main_window);

    if(init) LAB_Quit();

    LAB_DbgMemShow();

    return return_value;


INIT_ERROR:
    fprintf(stderr, "Lab-Initialization failed: %s: %s\n", init_msg, LAB_GetError());

    return_value = EXIT_FAILURE;
    goto EXIT;
}
