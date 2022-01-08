#include "LAB_game_server_routines.h"

#include "LAB_world.h"
#include "LAB_world_light.h"
#include "LAB_chunk_neighborhood.h"

LAB_STATIC
void LAB_GameServer_ChunkGenerate_CB(LAB_GameServer* srv,
                                      LAB_Chunk* chunk, LAB_ChunkPos pos,
                                      void* uparam);

LAB_STATIC
void LAB_GameServer_ChunkUpdateLight_CB(LAB_GameServer* srv,
                                         LAB_Chunk* chunk, LAB_ChunkPos pos,
                                         void* uparam);

LAB_STATIC
void LAB_GameServer_ChunkViewMesh_CB(LAB_GameServer* srv,
                                      LAB_Chunk* chunk, LAB_ChunkPos pos,
                                      void* uparam);






LAB_ChunkCallback LAB_ChunkStageCallback(int update)
{
    switch(update)
    {
        case LAB_CHUNK_STAGE_GENERATE:    return &LAB_GameServer_ChunkGenerate_CB;
        case LAB_CHUNK_STAGE_LIGHT:       return &LAB_GameServer_ChunkUpdateLight_CB;
        case LAB_CHUNK_STAGE_VIEW_MESH:   return &LAB_GameServer_ChunkViewMesh_CB;
        default:                          LAB_ASSERT_FALSE("Invalid chunk stage");
    }
}

void LAB_ChunkStageNeighbors(int update, LAB_Chunk* chunk, LAB_Chunk* chunks[27])
{
//    LAB_GetChunkNeighborsNone(chunk, chunks);
//    return;

    if(update == LAB_CHUNK_STAGE_GENERATE)
        LAB_GetChunkNeighborsNone(chunk, chunks);
    else
        LAB_GetChunkNeighborsAll(chunk, chunks);
}



LAB_STATIC
void LAB_GameServer_ChunkGenerate_CB(LAB_GameServer* srv,
                                      LAB_Chunk* chunk, LAB_ChunkPos pos,
                                      void* uparam)
{
    LAB_ASSERT(!chunk->generated);

    srv->world->chunkgen(
        srv->world->chunkgen_user,
        chunk,
        pos.x, pos.y, pos.z
    );


    //update empty bit
    chunk->empty = true;
    for(int i = 0; i < 16*16*16; ++i)
    {
        if(chunk->blocks[i] != &LAB_BLOCK_AIR)
        {
            chunk->empty = false;
            break;
        }
    }


    atomic_store_explicit(&chunk->generated, true, memory_order_release);

    if(!LAB_GameServer_PushChunkTask(srv, chunk, LAB_CHUNK_STAGE_LIGHT))
        LAB_DbgPrintf("Generate->Light: Chunk queue full\n");
}



LAB_STATIC
void LAB_GameServer_ChunkUpdateLight_CB(LAB_GameServer* srv,
                                         LAB_Chunk* chunk, LAB_ChunkPos pos,
                                         void* uparam)
{
    LAB_ASSERT(LAB_Chunk_Access(chunk));

    LAB_Chunk* chunks[27];
    LAB_GetChunkNeighbors(chunk, chunks);

    LAB_CCPS dirty_blocks = atomic_exchange(&chunk->dirty_blocks, 0);
    //LAB_CCPS relit_blocks = atomic_exchange(&chunk->relit_blocks, 0); // used by next stage

    int dirty_neighbors = atomic_exchange(&chunk->dirty_neighbors, 0);
    int relit_neighbors = atomic_exchange(&chunk->relit_neighbors, 0);

    chunk->relit_blocks = 0;

    for(int i = 0; i < 27; ++i)
        if((i != 1+3+9 && chunks[i] && !chunks[i]->light_generated))
            chunks[i] = NULL;

    LAB_CCPS relit_blocks = LAB_TickLight(srv->world, chunks, dirty_neighbors|relit_neighbors, dirty_blocks);
    chunk->relit_blocks |= relit_blocks;

    LAB_ASSERT(chunk->light_generated);

    int dirty_bits27 = LAB_CCPS_Neighborhood(dirty_blocks);
    int relit_bits27 = LAB_CCPS_Neighborhood(relit_blocks);
    int bits27 = dirty_bits27 | relit_bits27;
    
    LAB_ASSERT(!!dirty_bits27 == !!dirty_blocks);
    LAB_ASSERT(!!relit_bits27 == !!relit_blocks);

    if(dirty_blocks || relit_blocks || dirty_neighbors || relit_neighbors)
    {
        //if(!LAB_GameServer_PushChunkTask(srv, LAB_VIEW_NOTIFY_PARITY, chunk, LAB_CHUNK_STAGE_VIEW_NOTIFY))
        if(!LAB_GameServer_PushChunkTask(srv, chunk, LAB_CHUNK_STAGE_VIEW_MESH))
        {}
    }

    int i = 0;
    for(int zz = -1; zz < 2; ++zz)
    for(int yy = -1; yy < 2; ++yy)
    for(int xx = -1; xx < 2; ++xx, ++i)
    {
        if(i != 1+3+9 &&
            chunks[i] && 
            chunks[i]->light_generated)
        {
            if(bits27 & 1<<i)
            {
                if(dirty_bits27 & 1<<i) chunks[i]->dirty_neighbors |= 1<<(26-i);
                if(relit_bits27 & 1<<i) chunks[i]->relit_neighbors |= 1<<(26-i);


                if(!LAB_GameServer_PushChunkTask(srv, chunks[i], LAB_CHUNK_STAGE_LIGHT))
                {}
            }
        }
    }
}

LAB_STATIC
void LAB_GameServer_ChunkViewMesh_CB(LAB_GameServer* srv,
                                      LAB_Chunk* chunk, LAB_ChunkPos pos,
                                      void* uparam)
{
    LAB_ASSERT(chunk->generated);
    LAB_ASSERT(chunk->light_generated);

    (*srv->world->view->chunkmesh)(srv->world->view_user, srv->world, chunk);
}