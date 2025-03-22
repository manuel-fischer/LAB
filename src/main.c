#include <stdlib.h>
#include <stdio.h>
#include <limits.h> // INT_MAX
#include "LAB.h"
#include "LAB_config.h"
#include <SDL2/SDL.h>

#include <math.h>
#include "LAB_bits.h"
#include "LAB_math.h"
#include "LAB_util.h"
#include "LAB_error.h"
#include "LAB_vec3.h"

#include "LAB_memory.h"

#include "LAB_gen_flat.h"
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

#include "LAB_builtin.h"
#include "LAB_builtin_blocks.h" // set default block to LAB_BLOCK_STONE.cobble

#include "LAB_text_renderer.h"

#include "LABtestcases.h"

#include "LAB_gen_dimension_wrapper.h"


static bool load_assets(LAB_TexAtlas* atlas, LAB_ModelSet* models, LAB_ItemTexSet* items)
{
    static LAB_Assets     assets = {0};
    LAB_Assets_Create(&assets, atlas, models, items);

    if(LAB_FAILED(LAB_Builtin_Init(&assets)))
    {
        printf("HELLLO\n");
        return false;
    }

    LAB_Dimension_FinishInit();

    LAB_TexAtlas_MakeMipmap(atlas);

    LAB_TexAtlas_Upload2GL(atlas);

    #ifndef NDEBUG
    LAB_TexAtlas_DbgDumpToFile(atlas, "dbg_terrain");
    #endif


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


#define LAB_Client_Create() LAB_Client_Obj(LAB_OBJ_CREATE)
#define LAB_Client_Destroy() ((void)LAB_Client_Obj(LAB_OBJ_DESTROY))
static bool LAB_Client_Obj(LAB_OBJ_Action action)
{
    uint32_t sdl_window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

    LAB_BEGIN_OBJ(action);

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

        LAB_YIELD_OBJ(true);
    }););););););););

    LAB_END_OBJ(false);
}



static struct
{
    union {
        LAB_GenFlat flat;
        LAB_GenDimensionWrapper_Ctx dimension;
    } gen;

    LAB_World       world;
    LAB_PerfInfo    perf_info;
    LAB_GameServer  game_server;
} LAB_main;
typedef struct LAB_MainConfig
{
    size_t worker_count;
} LAB_MainConfig;
#define LAB_Main_Create(cfg) LAB_Main_Obj(LAB_OBJ_CREATE, cfg)
#define LAB_Main_Destroy() ((void)LAB_Main_Obj(LAB_OBJ_DESTROY, NULL))
bool LAB_Main_Obj(LAB_OBJ_Action action, const LAB_MainConfig* cfg)
{
    LAB_BEGIN_OBJ(action);

    LAB_OBJ(LAB_World_Create(&LAB_main.world),
            (
                LAB_View_SetWorld(&LAB_client.view, NULL),
                LAB_World_Destroy(&LAB_main.world)
            ),

    LAB_OBJ(LAB_PerfInfo_Create(&LAB_main.perf_info),
            LAB_PerfInfo_Destroy(&LAB_main.perf_info),

    LAB_OBJ(LAB_InitThread(),
            (void)0,

    LAB_OBJ(LAB_GameServer_Create(&LAB_main.game_server, &LAB_main.world, cfg->worker_count),
            LAB_GameServer_Destroy(&LAB_main.game_server),

        LAB_YIELD_OBJ(true);
    ););););

    LAB_END_OBJ(false);
}


void LAB_SetupWorld(const LAB_WorldConfig* world_cfg)
{
    #if GEN_FLAT
    LAB_main.gen.flat.block = LAB_BLOCK_MARBLE.raw;
    LAB_main.world.chunkgen      = &LAB_GenFlatProc;
    LAB_main.world.chunkgen_user = &LAB_main.gen.flat;
    #else
    LAB_main.gen.dimension.world_seed = 9876543210;
    LAB_main.gen.dimension.dim = *LAB_Dimension_GetDefault();
    LAB_main.world.chunkgen = &LAB_GenDimensionWrapper_Gen_Func;
    LAB_main.world.chunkgen_user = &LAB_main.gen.dimension;
    #endif
    LAB_main.world.cfg = *world_cfg;
    LAB_main.world.perf_info = &LAB_main.perf_info;
    LAB_main.world.server = &LAB_main.game_server;
}

void LAB_SetupView(const LAB_ViewConfig* view_cfg)
{
    LAB_client.view.perf_info = &LAB_main.perf_info;
    LAB_client.view.server = &LAB_main.game_server;
    LAB_client.view.cfg = *view_cfg;
}
void LAB_SetupSpawn(void)
{
    LAB_client.view.pos = (LAB_Vec3D) { 0.5, 1.5, 0.5 };
    #if !GEN_FLAT
    LAB_GenDimensionWrapper_Ctx* dim = &LAB_main.gen.dimension;
    dim->dim.spawn_point(dim->dim.ctx, dim->world_seed,
        &LAB_client.view.pos.x,
        &LAB_client.view.pos.y,
        &LAB_client.view.pos.z
    );
    #endif
}
void LAB_LinkMain(void)
{
    // Link callbacks
    LAB_main.world.view = &LAB_view_interface;
    LAB_main.world.view_user = &LAB_client.view;

    LAB_View_SetWorld(&LAB_client.view, &LAB_main.world);

    LAB_client.window.onevent      = &LAB_Input_OnEvent_Proc;
    LAB_client.window.onevent_user = &LAB_client.input;

    LAB_client.window.render       = &LAB_ViewRenderProc;
    LAB_client.window.render_user  = &LAB_client.view;
}



void LAB_MainLoop(void)
{
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

        LAB_PerfInfo_Tick(&LAB_main.perf_info);

        LAB_PerfInfo_Next(&LAB_main.perf_info, LAB_TG_INPUT);
        //LAB_WorldServer_Lock(&world_server);
        LAB_Input_Tick(&LAB_client.input, delta_ms);
        //LAB_WorldServer_Unlock(&world_server);

        LAB_PerfInfo_Next(&LAB_main.perf_info, LAB_TG_WORLD);
        LAB_WorldTick(&LAB_main.world);
        LAB_GameServer_Tick(&LAB_main.game_server);

        LAB_PerfInfo_Next(&LAB_main.perf_info, LAB_TG_VIEW);
        //LAB_WorldServer_Lock(&world_server);
        LAB_ViewTick(&LAB_client.view, delta_ms);
        //LAB_WorldServer_Unlock(&world_server);

        LAB_PerfInfo_Next(&LAB_main.perf_info, LAB_TG_NONE);


        LAB_FpsGraph_SetSampleOffset(&LAB_main.perf_info.fps_graphs[LAB_TG_WHOLE], delta_ms);
        LAB_FpsGraph_SetSampleOffset(&LAB_main.perf_info.fps_graphs[LAB_TG_OUT_OF_LOOP], ool);

        if((itr & 0x3f) == 0)
        {
            LAB_GameServerStats server_stats;
            LAB_GameServer_GetStats(&LAB_main.game_server, &server_stats);
            //LAB_WorldServer_Lock(&world_server);

            LAB_TextRenderer r;
            LAB_TextRenderer_Create(&r, LAB_client.view.gui_mgr.mono_font_small);

            LAB_GameServerStats_RenderTo(&server_stats, &LAB_main.world, &LAB_client.view, &r);
            memset(&LAB_client.view.stats, 0, sizeof LAB_client.view.stats);

            LAB_SDL_FREE(SDL_FreeSurface, &LAB_client.view.stats_display.surf);
            LAB_client.view.stats_display.surf = LAB_TextRenderer_Render(&r);
            LAB_client.view.stats_display.reupload = true;

            LAB_TextRenderer_Destroy(&r);
        }

        endtime = SDL_GetTicks();
    }
}



int main(int argc, char** argv)
{
    LAB_DbgInitOrAbort();

    #ifndef NDEBUG
    LAB_DoTestCases();
    #endif

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

        //.gamma_map = &LAB_gamma_dark,
        .exposure = 1,
        .saturation = 1,
    };

    LAB_WorldConfig world_cfg = {
        .max_gen = 0,
        .max_update = 0,
    };

    bool success = false;
    LAB_OBJ(LAB_Init(),
            LAB_Quit(),

    LAB_OBJ(LAB_Client_Create(),
            LAB_Client_Destroy(),

    LAB_OBJ((LAB_GL_SetupDebug(), true), // TODO move into window creation
            (void)0,

    LAB_OBJ(LAB_Main_Create(&(LAB_MainConfig){.worker_count=worker_count}),
            LAB_Main_Destroy(),

    {
        success = true;
        LAB_SetupWorld(&world_cfg);
        LAB_SetupView(&view_cfg);
        LAB_SetupSpawn();
        LAB_LinkMain();
        LAB_MainLoop();
    }););););


    if(!success)
        fprintf(stderr, "LAB-Initialization failed: \n%s\n", LAB_GetError());


    #ifndef NDEBUG
    LAB_DbgMemShow();
    printf("Alive GL-Objects: %i\n", LAB_gl_debug_alloc_count);
    printf("Alive SDL-Objects: %i\n", LAB_sdl_debug_alloc_count);
    #endif

    LAB_DbgExit();

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
