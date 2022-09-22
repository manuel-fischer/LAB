#include <stdlib.h>
#include <stdio.h>
#include <limits.h> // INT_MAX
#include "LAB.h"
#include "LAB_config.h"
#include <SDL2/SDL.h>

#include <math.h>
#include "LAB_bits.h"
#include "LAB_math.h"

#include "LAB_memory.h"

#include "LAB_gen_flat.h"
#include "LAB_gen_overworld.h"
#include "LAB_gen_overworld_shape.h"
#include "LAB_asset_manager.h"
#include "LAB_gl.h"
#include "LAB_sdl.h"

#include "LAB/gui.h"

#include "LAB_perf_info.h"

#include "LAB_game_server.h"

#define GEN_FLAT 0

#include "LAB_texture_atlas.h" // TODO remove from here
#include "LAB_render_item.h"
#include "LAB_thread.h"

#include "LAB_text_renderer.h"

LAB_INLINE bool LAB_DoTests()
{
#if 0
    LAB_TestTextureAtlas();
    return false;
#else
    return true;
#endif
}

void LAB_Temp_RecreateTerrain(LAB_TexAtlas* atlas); // TODO remove
static bool load_assets(LAB_TexAtlas* atlas, LAB_ModelSet* models, LAB_ItemTexSet* items)
{
    static LAB_Assets     assets = {0};
    LAB_Assets_Create(&assets, atlas, models, items);
    
    LAB_BuiltinBlocks_Init(&assets);
    
    LAB_TexAtlas_MakeMipmap(atlas);

    glEnable(GL_TEXTURE_2D);
    LAB_TexAtlas_Upload2GL(atlas);
    LAB_TexAtlas_LoadTexMatrix(atlas);

    LAB_Assets_Destroy(&assets);

    return true;
}


///// CLIENT /////
#include "LAB_obj.h"

static struct
{
    LAB_Window window;
    LAB_View   view;
    LAB_Input  input;

    LAB_TexAtlas atlas;
    LAB_ModelSet models;
    LAB_ItemTexSet items;
} LAB_client;


#define LAB_Client_Create() LAB_Client_Obj(false)
#define LAB_Client_Destroy() ((void)LAB_Client_Obj(true))
static bool LAB_Client_Obj(bool destroy)
{
    uint32_t sdl_window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

    if(destroy) goto lbl_destroy;

    LAB_OBJ(LAB_GuiInit(),
            LAB_GuiQuit(),
        
    LAB_OBJ(LAB_Window_Create(&LAB_client.window, 1024, 576, sdl_window_flags),
            LAB_Window_Destroy(&LAB_client.window),


    LAB_OBJ(LAB_View_Create(&LAB_client.view, NULL, &LAB_client.atlas),
            LAB_View_Destroy(&LAB_client.view),

    LAB_OBJ(LAB_Input_Create(&LAB_client.input, &LAB_client.view),
            LAB_Input_Destroy(&LAB_client.input),


    LAB_OBJ(LAB_TexAtlas_Create(&LAB_client.atlas, 32),
            LAB_TexAtlas_Destroy(&LAB_client.atlas),

    LAB_OBJ(LAB_ModelSet_Create(&LAB_client.models),
            LAB_ModelSet_Destroy(&LAB_client.models),

    LAB_OBJ(LAB_ItemTexSet_Create(&LAB_client.items),
            LAB_ItemTexSet_Destroy(&LAB_client.items),
            

    LAB_OBJ(load_assets(&LAB_client.atlas, &LAB_client.models, &LAB_client.items),
            (void)0,

    {
        LAB_client.input.selected_block = LAB_BLOCK_STONE.cobble;

        return true;
        lbl_destroy:;
    }););););););););
    return false;
}



int main(int argc, char** argv)
{
    LAB_DbgInitOrAbort();

    #define CHECK_INIT(expr) if(expr); else { init_msg = #expr; goto INIT_ERROR; }

    char const* init_msg;
    int return_value;

    int init = 0;
    static LAB_PerfInfo    perf_info   = {0};
    static LAB_World       the_world   = {0};
    static LAB_GameServer  game_server;

    size_t core_count = LAB_CoreCount();
    size_t worker_count = core_count > 1 ? core_count-1 : 1;

    LAB_ViewConfig view_cfg = {
        .flags = LAB_VIEW_SHOW_HUD,

        .preload_dist = LAB_PRELOAD_CHUNK(12),
        .render_dist = 12, //5,
        .keep_dist = LAB_KEEP_CHUNK(12),
        
        // Limits
        .max_update = INT_MAX,//100,
        .max_unload = 20,

        .load_amount = INT_MAX,//100,
        .empty_load_amount = 5,

        .gamma_map = &LAB_gamma_dark,
    };

    LAB_WorldConfig world_cfg = {
        .max_gen = 0,
        .max_update = 0,
    };


    static union {
        LAB_GenFlat flat;
        LAB_GenOverworld overworld;
    } gen = {0};
    
    CHECK_INIT(LAB_Init());
    init = 1;

    CHECK_INIT(LAB_Client_Create());
    LAB_ObjCopy(&LAB_client.view.cfg, &view_cfg);

    CHECK_INIT(LAB_PerfInfo_Create(&perf_info));
    //SDL_SetWindowFullscreen(main_window->window, SDL_WINDOW_FULLSCREEN);



    CHECK_INIT(LAB_InitThread());
    
    CHECK_INIT(LAB_GameServer_Create(&game_server, &the_world, worker_count));

    CHECK_INIT(LAB_World_Create(&the_world));
    #if GEN_FLAT
    gen.flat.block = &LAB_BLOCK_STONE;
    the_world.chunkgen      = &LAB_GenFlatProc;
    the_world.chunkgen_user = &gen.flat;
    #else
    //gen.overworld.seed = 0x13579bdf;
    //gen.overworld.seed = 2347818473829147;
    gen.overworld.seed = 58925789342573489;
    //gen.overworld.seed = 78434678123467586;
    //gen.overworld.seed = 7823489034819884932;
    gen.overworld.seed = 123456789;
    gen.overworld.seed = 1234567890123456789;
    gen.overworld.seed = 9876543210;
    //gen.overworld.seed = 73489564378791825;
    //gen.overworld.seed = 42;
    the_world.chunkgen      = &LAB_GenOverworldProc;
    the_world.chunkgen_user = &gen.overworld;
    #endif
    LAB_ObjCopy(&the_world.cfg, &world_cfg);
    the_world.perf_info = &perf_info;
    LAB_client.view.perf_info = &perf_info;

    the_world.server = &game_server;
    LAB_client.view.server = &game_server;

    LAB_GL_CHECK();



    LAB_client.view.x = LAB_client.view.z = 0.5;
    //LAB_client.view.x = -524288+3.5;
    #if GEN_FLAT
    LAB_client.view.y = 2;
    #else
    LAB_client.view.y = LAB_Gen_Surface_Shape_Func(&gen.overworld, LAB_FastFloorD2I(LAB_client.view.x), LAB_FastFloorD2I(LAB_client.view.z)) + 3;
    #endif


    // Link callbacks
    the_world.view = &LAB_view_interface;
    the_world.view_user = &LAB_client.view;

    LAB_View_SetWorld(&LAB_client.view, &the_world);

    LAB_client.window.onevent      = &LAB_Input_OnEvent_Proc;
    LAB_client.window.onevent_user = &LAB_client.input;

    LAB_client.window.render       = &LAB_ViewRenderProc;
    LAB_client.window.render_user  = &LAB_client.view;


    CHECK_INIT(LAB_DoTests());


    uint32_t time_ms = SDL_GetTicks();
    uint32_t endtime = time_ms;

    int itr = 0;
    while(LAB_WindowLoop(&LAB_client.window))
    {
        const char* labErr;
        while((labErr = LAB_GetError())[0] != '\0')
        {
            LAB_DbgPrintf("[Error tick %i] %s\n", itr, labErr);
            LAB_ClearError();
        }
        ++itr;

        /*LAB_client.view.x = (((LAB_FloorD2I(LAB_client.view.x)+0x80000)&0xfffff)-0x80000) + LAB_FractD(LAB_client.view.x);
        LAB_client.view.y = (((LAB_FloorD2I(LAB_client.view.y)+0x80000)&0xfffff)-0x80000) + LAB_FractD(LAB_client.view.y);
        LAB_client.view.z = (((LAB_FloorD2I(LAB_client.view.z)+0x80000)&0xfffff)-0x80000) + LAB_FractD(LAB_client.view.z);*/

        uint32_t time2_ms = SDL_GetTicks();
        //uint32_t ool = time2_ms-endtime;
        uint32_t ool = endtime-time_ms; // in of loop
        uint32_t delta_ms = time2_ms-time_ms;
        ///*if(itr%20 == 0)*/ printf("%i fps          \r", 1000/delta_ms);
        time_ms = time2_ms;

        LAB_PerfInfo_Tick(&perf_info);

        LAB_PerfInfo_Next(&perf_info, LAB_TG_INPUT);
        //LAB_WorldServer_Lock(&world_server);
        LAB_Input_Tick(&LAB_client.input, delta_ms);
        //LAB_WorldServer_Unlock(&world_server);

        LAB_PerfInfo_Next(&perf_info, LAB_TG_WORLD);
        LAB_WorldTick(&the_world);
        LAB_GameServer_Tick(&game_server);

        LAB_PerfInfo_Next(&perf_info, LAB_TG_VIEW);
        //LAB_WorldServer_Lock(&world_server);
        LAB_ViewTick(&LAB_client.view, delta_ms);
        //LAB_WorldServer_Unlock(&world_server);

        LAB_PerfInfo_Next(&perf_info, LAB_TG_NONE);


        LAB_FpsGraph_SetSampleOffset(&perf_info.fps_graphs[LAB_TG_WHOLE], delta_ms);
        LAB_FpsGraph_SetSampleOffset(&perf_info.fps_graphs[LAB_TG_OUT_OF_LOOP], ool);

        //LAB_DbgPrintf("Completed Cycles: %i\n", world_server.completed_cycles);
        //LAB_DbgPrintf("Completed MTTasks: %i\n", world_server.completed_mainthread);
        //LAB_DbgPrintf("Max Age: %i\n", world_server.max_age);
        //LAB_DbgPrintf("Task Count: %i\n", world_server.task_count);
        
        #if 1//!defined NDEBUG && 1
        if((itr & 0x3f) == 0)
        {
            LAB_GameServerStats server_stats;
            LAB_GameServer_GetStats(&game_server, &server_stats);
            //LAB_WorldServer_Lock(&world_server);

            int divW = the_world.chunks.size ? the_world.chunks.size : 1;
            //int divV = LAB_client.view.chunks.size ? LAB_client.view.chunks.size : 1;
            #if 0
            char buf[1024];
            snprintf(buf, sizeof(buf),
                            //"\r"
                            "Max Age:%5i, Task Count:%5i, MTTasks:%5i, "
                            "Thread Usage: %3i%%, "
                            "Requeued: %3i, Cycles: %3i, Spins:%4i, "
                            "WChunks:%5i, In Queue: %3i%%, WProbe: %i (%i%%), ",
                            //"VChunks:%5i, VProbe: %i (%i%%), "
                            //"Upload: %i MB/s "
                            //"                   ",
                        server_stats.max_age,
                        server_stats.completed_task_count,
                        server_stats.completed_mainthread,

                        server_stats.runtime_computed*100 / (server_stats.runtime+1),

                        server_stats.requeued_count,
                        0,//server_stats.completed_cycles,
                        server_stats.noop_spins,

                        the_world.chunks.size,
                        server_stats.waiting_task_count*100 / divW,
                        
                        the_world.chunks.dbg_max_probe,
                        the_world.chunks.dbg_max_probe*100 / divW
                        
                        /*LAB_client.view.chunks.size,
                        LAB_client.view.chunks.dbg_max_probe,
                        LAB_client.view.chunks.dbg_max_probe*100 / divV,*/
                        
                        //LAB_client.view.upload_time ?
                        //    (int)((float)LAB_client.view.upload_amount*1000/LAB_client.view.upload_time):0
            );
            #endif

            LAB_TextRenderer r;
            LAB_TextRenderer_Create(&r, LAB_client.view.gui_mgr.mono_font_small);

            LAB_TextRenderer_VSpace(&r, 0.5f);
            LAB_TextRenderer_Printfln(&r, "Max Age:%10i", server_stats.max_age);
            LAB_TextRenderer_Printfln(&r, "Task Count:%7i", server_stats.completed_task_count);
            LAB_TextRenderer_Printfln(&r, "New:%14i", server_stats.new_task_count);
            LAB_TextRenderer_Printfln(&r, "Overriden:%8i", server_stats.overriden_task_count);
            LAB_TextRenderer_Printfln(&r, "Requeued:%9i", server_stats.requeued_count);
            LAB_TextRenderer_Printfln(&r, "Queue:%7i  %2i%%", 
                        server_stats.waiting_task_count,
                        server_stats.waiting_task_count*100 / divW);


            LAB_TextRenderer_VSpace(&r, 0.5f);
            LAB_TextRenderer_Printfln(&r, "Thread Usage: %3i%%", server_stats.runtime_computed*100 / (server_stats.runtime+1));
            LAB_TextRenderer_Printfln(&r, "GenCount:%9i", server_stats.update_counts[LAB_CHUNK_STAGE_GENERATE]);
            LAB_TextRenderer_Printfln(&r, "LightCount:%7i", server_stats.update_counts[LAB_CHUNK_STAGE_LIGHT]);
            LAB_TextRenderer_Printfln(&r, "MeshCount:%8i", server_stats.update_counts[LAB_CHUNK_STAGE_VIEW_MESH]);

            LAB_TextRenderer_Printfln(&r, "GenTime:%9i%%", server_stats.update_runtimes[LAB_CHUNK_STAGE_GENERATE]*100 / (server_stats.runtime_computed+1));
            LAB_TextRenderer_Printfln(&r, "LightTime:%7i%%", server_stats.update_runtimes[LAB_CHUNK_STAGE_LIGHT]*100 / (server_stats.runtime_computed+1));
            LAB_TextRenderer_Printfln(&r, "MeshTime:%8i%%", server_stats.update_runtimes[LAB_CHUNK_STAGE_VIEW_MESH]*100 / (server_stats.runtime_computed+1));
            LAB_TextRenderer_Printfln(&r, "GenQCount:%8i", server_stats.gen_queue_size);


            LAB_TextRenderer_VSpace(&r, 0.5f);
            LAB_TextRenderer_Printfln(&r, "WChunks:%10i", the_world.chunks.size);
            //LAB_TextRenderer_Printfln(&r, "InQueue:%10i", server_stats.waiting_task_count*100 / divW);
            LAB_TextRenderer_Printfln(&r, "WProbe: %5i  %2i%%",
                        the_world.chunks.dbg_max_probe,
                        the_world.chunks.dbg_max_probe*100 / divW
            );
            LAB_TextRenderer_Printfln(&r, "Capacity:%9i", the_world.chunks.capacity);


            LAB_TextRenderer_VSpace(&r, 0.5f);
            //LAB_TextRenderer_Printfln(&r, "WPointer:%9i", game_server.update_pointer);
            LAB_TextRenderer_Printfln(&r, "WCycles:%10i", server_stats.update_cycles);
            LAB_TextRenderer_Printfln(&r, "UnloadCycles:%5i", server_stats.unload_cycles);
            LAB_TextRenderer_Printfln(&r, "VCycles:%10i", server_stats.view_update_cycles);
            LAB_TextRenderer_Printfln(&r, "MaxUpdates:%7i", server_stats.max_chunk_updates);
            LAB_TextRenderer_Printfln(&r, "AvgUpdates:%7i", (int)(server_stats.avg_chunk_updates));


            LAB_TextRenderer_VSpace(&r, 0.5f);
            LAB_TextRenderer_Printfln(&r, "VChunks:%10i", LAB_client.view.chunk_array.entries_count);
            LAB_TextRenderer_Printfln(&r, "VChunks<:%9i", LAB_client.view.chunk_array.entries_sorted_count);
            LAB_TextRenderer_Printfln(&r, "VChunks?:%9i", LAB_client.view.chunk_array.entries_sorted_nonempty_count);
            LAB_TextRenderer_Printfln(&r, "V/WChunks:%7i%%", LAB_client.view.chunk_array.entries_sorted_count*100/the_world.chunks.size);

            LAB_TextRenderer_Printfln(&r, "VDelQueue:%8i", LAB_client.view.chunk_array.del_list_count);
            LAB_TextRenderer_Printfln(&r, "VRecovered:%7i", LAB_client.view.stats.recovered_count);
            LAB_TextRenderer_Printfln(&r, "VDeleted:%9i", LAB_client.view.stats.deleted);
            memset(&LAB_client.view.stats, 0, sizeof LAB_client.view.stats);

            LAB_SDL_FREE(SDL_FreeSurface, &LAB_client.view.stats_display.surf);
            LAB_client.view.stats_display.surf = LAB_TextRenderer_Render(&r);
            LAB_client.view.stats_display.reupload = true;

            LAB_TextRenderer_Destroy(&r);

            //LAB_ASSERT(server_cpy.task_count <= the_world.chunks.size);
            //LAB_WorldServer_Unlock(&world_server);
        }
        #endif

        //int den = game_server.stats.max_age+1+2;
        //LAB_client.view.cfg.load_amount = (200+(rand()%den))/(den);

        endtime = SDL_GetTicks();
    }


    return_value = EXIT_SUCCESS;
    //goto EXIT;

EXIT:
    LAB_GameServer_Destroy(&game_server);

    LAB_View_SetWorld(&LAB_client.view, NULL);

    LAB_World_Destroy(&the_world);
    
    LAB_PerfInfo_Destroy(&perf_info);
    LAB_Client_Destroy();

    if(init) LAB_Quit();

    #ifndef NDEBUG
    LAB_DbgMemShow();
    printf("Alive GL-Objects: %i\n", LAB_gl_debug_alloc_count);
    printf("Alive SDL-Objects: %i\n", LAB_sdl_debug_alloc_count);
    #endif

    LAB_DbgExit();

    return return_value;


INIT_ERROR:
    fprintf(stderr, "Lab-Initialization failed: %s: %s\n", init_msg, LAB_GetError());

    return_value = EXIT_FAILURE;
    goto EXIT;
}
