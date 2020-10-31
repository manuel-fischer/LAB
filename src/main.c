#include <stdlib.h>
#include <stdio.h>
#include "LAB.h"
#include <SDL2/SDL.h>

#include <math.h>
#include "LAB_bits.h"

#include "LAB_memory.h"

#include "LAB_gen_flat.h"
#include "LAB_gen_overworld.h"
#include "LAB_asset_manager.h"
#include "LAB_gl.h"

#include "LAB_gui.h"

#define GEN_FLAT 0

int main(int argc, char** argv) {
    #define CHECK_INIT(expr) if(expr); else { init_msg = #expr; goto INIT_ERROR; }

    char const* init_msg;
    int return_value;

    int init = 0;
    static LAB_Window     main_window = {0};
    static LAB_World      the_world   = {0};
    static LAB_View       view        = {0};
    static LAB_ViewInput  view_input  = {0};
    #if GEN_FLAT
    static LAB_GenFlat    gen_flat    = {0};
    #else
    static LAB_GenOverworld gen_overworld = {0};
    #endif

    CHECK_INIT(LAB_Init());
    init = 1;

    CHECK_INIT(LAB_GuiInit());


    uint32_t sdl_window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    CHECK_INIT(LAB_ConstructWindow(&main_window, 1024, 576, sdl_window_flags));
    LAB_GL_CHECK();
    //SDL_SetWindowFullscreen(main_window->window, SDL_WINDOW_FULLSCREEN);


    CHECK_INIT(LAB_ConstructWorld(&the_world));
    #if GEN_FLAT
    gen_flat.block = &LAB_BLOCK_STONE;
    the_world.chunkgen      = &LAB_GenFlatProc;
    the_world.chunkgen_user = &gen_flat;
    #else
    gen_overworld.seed = 0x13579bdf;
    the_world.chunkgen      = &LAB_GenOverworldProc;
    the_world.chunkgen_user = &gen_overworld;
    #endif
    the_world.max_gen = 0;
    the_world.max_update = 0;

    CHECK_INIT(LAB_ConstructView(&view, &the_world));
    LAB_GL_CHECK();
    view.render_dist = 5;
    view.preload_dist = LAB_PRELOAD_CHUNK(view.render_dist);
    view.keep_dist = LAB_KEEP_CHUNK(view.render_dist);
    view.flags = LAB_VIEW_SHOW_HUD;
    view.max_update = 160;
    //view.load_amount = 3;
    //view.load_amount = 10;
    view.load_amount = 1;
    view.load_amount = 100; // DBG

    CHECK_INIT(LAB_ConstructViewInput(&view_input, &view));
    LAB_GL_CHECK();

    the_world.chunkview      = &LAB_ViewChunkProc;
    the_world.chunkview_user = &view;

    the_world.chunkkeep      = &LAB_ViewChunkKeepProc;
    the_world.chunkkeep_user = &view;


    main_window.onevent      = &LAB_ViewInputOnEventProc;
    main_window.onevent_user = &view_input;

    main_window.render       = &LAB_ViewRenderProc;
    main_window.render_user  = &view;


    uint32_t time_ms = SDL_GetTicks();

    int itr = 0;
    while(LAB_WindowLoop(&main_window))
    {
        const char* labErr;
        while((labErr = LAB_GetError())[0] != '\0')
        {
            fprintf(stderr, "[Error tick %i] %s\n", itr, labErr);
            LAB_ClearError();
        }
        ++itr;

        uint32_t time2_ms = SDL_GetTicks();
        uint32_t delta_ms = time2_ms-time_ms;
        ///*if(itr%20 == 0)*/ printf("%i fps          \r", 1000/delta_ms);
        time_ms = time2_ms;

        #if 0
        LAB_ViewInputTick(&view_input, delta_ms);
        /*if(itr%10==0)*/ LAB_WorldTick(&the_world, delta_ms);
        LAB_ViewTick(&view, delta_ms);
        #else
            uint64_t t0 = LAB_NanoSeconds();
        LAB_ViewInputTick(&view_input, delta_ms);
            uint64_t t1 = LAB_NanoSeconds();
        LAB_WorldTick(&the_world, delta_ms);
            uint64_t t2 = LAB_NanoSeconds();
        LAB_ViewTick(&view, delta_ms);
            uint64_t t3 = LAB_NanoSeconds();

        uint64_t d_01, d_12, d_23;
        d_01 = t1-t0;
        d_12 = t2-t1;
        d_23 = t3-t2;

        /*if(itr%16==0)
        {
            printf("ViewInput %i\nWorld %i\nView %i\n\n", (int)d_01, (int)d_12, (int)d_23);
        }
        //printf("%i\r", the_world.chunks.size);*/

        LAB_FpsGraph_AddSample(&view.fps_graph,       delta_ms);
        LAB_FpsGraph_AddSample(&view.fps_graph_input, (float)d_01/1000000.f);
        LAB_FpsGraph_AddSample(&view.fps_graph_world, (float)d_12/1000000.f);
        LAB_FpsGraph_AddSample(&view.fps_graph_view,  (float)d_23/1000000.f);
        #endif
    }


    return_value = EXIT_SUCCESS;
    //goto EXIT;

EXIT:
    // Null destructable
    LAB_DestructViewInput(&view_input);
    LAB_DestructView(&view);
    LAB_DestructWorld(&the_world);
    LAB_DestructWindow(&main_window);

    if(init) LAB_Quit();

    LAB_QuitAssets();

    #ifndef NDEBUG
    LAB_DbgMemShow();
    printf("Alive GL-Objects: %i\n", LAB_gl_debug_alloc_count);
    #endif

    return return_value;


INIT_ERROR:
    fprintf(stderr, "Lab-Initialization failed: %s: %s\n", init_msg, LAB_GetError());

    return_value = EXIT_FAILURE;
    goto EXIT;
}
