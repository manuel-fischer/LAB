#include <stdlib.h>
#include <stdio.h>
#include "LAB.h"
#include <SDL2/SDL.h>

#include <math.h>
#include "LAB_bits.h"

#include "LAB_memory.h"

#include "LAB_gen_flat.h"
#include "LAB_gen_overworld.h"
#include "LAB_gen_overworld_shape.h"
#include "LAB_asset_manager.h"
#include "LAB_gl.h"
#include "LAB_sdl.h"

#include "LAB_gui.h"

#include "LAB_perf_info.h"

#define GEN_FLAT 0

#include "LAB_texture_atlas.h" // TODO remove from here

LAB_INLINE bool LAB_DoTests()
{
#if 0
    LAB_TestTextureAtlas();
    return false;
#else
    return true;
#endif
}


int main(int argc, char** argv)
{
    #define CHECK_INIT(expr) if(expr); else { init_msg = #expr; goto INIT_ERROR; }

    char const* init_msg;
    int return_value;

    int init = 0;
    static LAB_Window     main_window = {0};
    static LAB_PerfInfo   perf_info   = {0};
    static LAB_World      the_world   = {0};
    static LAB_View       view        = {0};
    static LAB_Input      input       = {0};

    static union {
        LAB_GenFlat flat;
        LAB_GenOverworld overworld;
    } gen = {0};

    CHECK_INIT(LAB_Init());
    init = 1;

    CHECK_INIT(LAB_GuiInit());


    uint32_t sdl_window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    CHECK_INIT(LAB_ConstructWindow(&main_window, 1024, 576, sdl_window_flags));
    CHECK_INIT(LAB_PerfInfo_Create(&perf_info));
    //SDL_SetWindowFullscreen(main_window->window, SDL_WINDOW_FULLSCREEN);


    CHECK_INIT(LAB_ConstructWorld(&the_world));
    #if GEN_FLAT
    gen.flat.block = &LAB_BLOCK_STONE;
    the_world.chunkgen      = &LAB_GenFlatProc;
    the_world.chunkgen_user = &gen.flat;
    #else
    //gen_overworld.seed = 0x13579bdf;
    //gen_overworld.seed = 2347818473829147;
    //gen_overworld.seed = 58925789342573489;
    //gen_overworld.seed = 78434678123467586;
    gen.overworld.seed = 7823489034819884932;
    the_world.chunkgen      = &LAB_GenOverworldProc;
    the_world.chunkgen_user = &gen.overworld;
    #endif
    the_world.max_gen = 0;
    the_world.max_update = 0;
    the_world.perf_info = &perf_info;

    CHECK_INIT(LAB_ConstructView(&view, &the_world));
    LAB_GL_CHECK();
    view.render_dist = 5;
    view.preload_dist = LAB_PRELOAD_CHUNK(view.render_dist);
    view.keep_dist = LAB_KEEP_CHUNK(view.render_dist);
    view.flags = LAB_VIEW_SHOW_HUD | LAB_VIEW_USE_VBO;
    //view.max_update = 10;//160;
    view.max_update = 100;
    view.max_unload = 20;
    //view.load_amount = 3;
    //view.load_amount = 10;
    //view.load_amount = 1;
    view.load_amount = 7;
    view.empty_load_amount = 5;
    //view.load_amount = 100; // DBG
    
    /*view.load_amount = 20;
    view.empty_load_amount = 20;*/
    
    view.perf_info = &perf_info;

    view.x = view.z = 0.5;
    #if GEN_FLAT
    view.y = 2;
    #else
    view.y = LAB_Gen_Surface_Shape_Func(&gen.overworld, 0, 0) + 3;
    #endif

    CHECK_INIT(LAB_Input_Create(&input, &view));
    LAB_GL_CHECK();

    the_world.view = &LAB_view_interface;
    the_world.view_user = &view;

    main_window.onevent      = &LAB_Input_OnEvent_Proc;
    main_window.onevent_user = &input;

    main_window.render       = &LAB_ViewRenderProc;
    main_window.render_user  = &view;


    CHECK_INIT(LAB_DoTests());


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

        LAB_PerfInfo_Tick(&perf_info);

        LAB_PerfInfo_Next(&perf_info, LAB_TG_INPUT);
        LAB_Input_Tick(&input, delta_ms);

        LAB_PerfInfo_Next(&perf_info, LAB_TG_WORLD);
        LAB_WorldTick(&the_world, delta_ms);

        LAB_PerfInfo_Next(&perf_info, LAB_TG_VIEW);
        LAB_ViewTick(&view, delta_ms);

        LAB_PerfInfo_Next(&perf_info, LAB_TG_NONE);


        LAB_FpsGraph_SetSample(&perf_info.fps_graphs[LAB_TG_WHOLE], delta_ms);
    }


    return_value = EXIT_SUCCESS;
    //goto EXIT;

EXIT:
    // Null destructable
    LAB_Input_Destroy(&input);
    LAB_DestructView(&view);
    LAB_DestructWorld(&the_world);
    LAB_PerfInfo_Destroy(&perf_info);
    LAB_DestructWindow(&main_window);

    LAB_QuitAssets();
    LAB_GuiQuit();

    if(init) LAB_Quit();

    #ifndef NDEBUG
    LAB_DbgMemShow();
    printf("Alive GL-Objects: %i\n", LAB_gl_debug_alloc_count);
    printf("Alive SDL-Objects: %i\n", LAB_sdl_debug_alloc_count);
    #endif

    return return_value;


INIT_ERROR:
    fprintf(stderr, "Lab-Initialization failed: %s: %s\n", init_msg, LAB_GetError());

    return_value = EXIT_FAILURE;
    goto EXIT;
}
