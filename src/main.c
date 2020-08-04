#include <stdlib.h>
#include <stdio.h>
#include <LAB.h>
#include <SDL2/SDL.h>



static LAB_Chunk* ChunkGenerateFlat(void* user, LAB_World* world, int x, int y, int z)
{
    LAB_Block* block = y < 0 ? &LAB_BLOCK_STONE : &LAB_BLOCK_AIR;
    LAB_Chunk* chunk = LAB_CreateChunk(block);
    if(!chunk) return NULL;

    #if 0
    if(y == -1)
    {
        for(int zz = 0; zz < 16; ++zz)
        for(int xx = 0; xx < 16; ++xx)
        for(int yy = 0; yy < 16; ++yy)
        {
            //if(rand() & 1)
            //    chunk->blocks[LAB_CHUNK_OFFSET(xx, yy, zz)] = &LAB_BLOCK_GRASS;
            //chunk->blocks[LAB_CHUNK_OFFSET(xx, 15^yy, zz)] = &LAB_BLOCK_GRASS;
            chunk->blocks[LAB_CHUNK_OFFSET(xx, 15^yy, zz)] = &LAB_BLOCK_COBBLESTONE;
            /*if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;*/
        }


    }
    #endif
    return chunk;
}



int main(int argc, char** argv) {
    #define CHECK_INIT(expr) if(expr) { init_msg = #expr; goto INIT_ERROR; }

    char const* init_msg;
    int return_value;

    int init = 0;
    LAB_Window*    main_window = NULL;
    LAB_World*     the_world   = NULL;
    LAB_View*      world_view  = NULL;
    LAB_ViewInput  view_input  = {};

    CHECK_INIT(LAB_Init() != 0);
    init = 1;


    uint32_t sdl_window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    main_window = LAB_CreateWindow(1024, 576, sdl_window_flags);
    CHECK_INIT(main_window == NULL);
    //SDL_SetWindowFullscreen(main_window->window, SDL_WINDOW_FULLSCREEN);


    the_world = LAB_CreateWorld();
    CHECK_INIT(the_world == NULL);

    the_world->chunkgen      = &ChunkGenerateFlat;
    the_world->chunkgen_user = NULL;

    world_view = LAB_CreateView(the_world);
    CHECK_INIT(world_view == NULL);

    view_input.view = world_view;

    the_world->chunkview      = &LAB_ViewChunkProc;
    the_world->chunkview_user = world_view;


    main_window->onevent      = &LAB_ViewInputOnEventProc;
    main_window->onevent_user = &view_input;

    main_window->render       = &LAB_ViewRenderProc;
    main_window->render_user  = world_view;


    while(LAB_WindowLoop(main_window))
    {
        LAB_Chunk* chunks[27];
        LAB_GetChunkNeighborhood(the_world, chunks, (int)(world_view->x / LAB_CHUNK_SIZE),
                                                    (int)(world_view->y / LAB_CHUNK_SIZE),
                                                    (int)(world_view->z / LAB_CHUNK_SIZE),
                                                    LAB_CHUNK_GENERATE);

        LAB_ViewInputTick(&view_input);
    };


    return_value = EXIT_SUCCESS;
    //goto EXIT;

EXIT:
    if(world_view != NULL)  LAB_DestroyView(world_view);
    if(the_world != NULL)   LAB_DestroyWorld(the_world);
    if(main_window != NULL) LAB_DestroyWindow(main_window);

    if(init) LAB_Quit();

    return return_value;


INIT_ERROR:
    fprintf(stderr, "Lab-Initialization failed: %s: %s\n", init_msg, LAB_GetError());

    return_value = EXIT_FAILURE;
    goto EXIT;
}
