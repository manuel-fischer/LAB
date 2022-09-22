#pragma once

#include "LAB_crammed_chunk_pos_set.h"
#include "LAB_chunk.h"
#include "LAB_stdinc.h"
#include "LAB_debug.h"

#include "LAB_check.h"
#include "LAB_opt.h"
#include "LAB_perf_info.h"
#include "LAB_htl_config.h"

#include "LAB_builtin_blocks.h"

#include "LAB_world_chunk_table.h"



#define LAB_MAX_CHUNK_AGE 1
//#define LAB_MAX_CHUNK_AGE 256
//#define LAB_MAX_LOAD_CHUNK 16
//#define LAB_MAX_LOAD_CHUNK 256

//#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)+1)
//#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)+2)
//#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)+3)
//#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)*5/4+3)
//#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)*3/2+3)
//#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)*2+3)
#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)*5/4+3)
#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)*3/2+3)





typedef struct LAB_World LAB_World;

typedef void(LAB_IView_ChunkViewer)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
typedef void(LAB_IView_ChunkMesh)(void* user, LAB_World* world, LAB_Chunk* chunk);
typedef bool(LAB_IView_ChunkKeeper)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
typedef void(LAB_IView_ChunkUnlinker)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
typedef void(LAB_IView_Position)(void* user, LAB_World* world, LAB_OUT double xyz[3]);

typedef struct LAB_IView
{
    LAB_IView_ChunkViewer* chunkview;
    LAB_IView_ChunkMesh* chunkmesh;
    LAB_IView_ChunkKeeper* chunkkeep;
    LAB_IView_ChunkUnlinker* chunkunlink;
    LAB_IView_Position* position;
} LAB_IView;

/*typedef struct LAB_ChunkEntry
{
    int x, y, z;
    LAB_Chunk* chunk;
} LAB_ChunkEntry;*/


typedef struct LAB_WorldConfig
{
    size_t max_gen,
           max_update;
} LAB_WorldConfig;

typedef struct LAB_World
{
    LAB_ChunkGenerator* chunkgen;
    void*               chunkgen_user;

    const LAB_IView* view;
    void*            view_user;
    int px, py, pz; // TODO change this

    LAB_ChunkTBL chunks;

    //LAB_ChunkBufQueue gen_queue;
    //LAB_ChunkPos2Queue update_queue;

    LAB_WorldConfig cfg;

    //LAB_LightUpdateQueue light_queue;

    LAB_PerfInfo* perf_info;

    struct LAB_GameServer* server;
} LAB_World;


/**
 *  Init world, return 0 on error
 */
int  LAB_World_Create(LAB_World* world);
/**
 *  Destruct world
 */
void LAB_World_Destroy(LAB_World* world);


/**
 *  Get chunk,
 *  Return chunk or NULL if chunk was not generated yet
 */
LAB_Chunk* LAB_GetChunk(LAB_World* world, int x, int y, int z);

/**
 *  Create the chunk, when not existing
 *  Return chunk or NULL if chunk was not generated yet
 */
LAB_Chunk* LAB_GenerateChunk(LAB_World* world, int x, int y, int z);




/**
 *  Refresh a chunk by notifying the view
 */
//void LAB_NotifyChunk(LAB_World* world, int x, int y, int z);
void LAB_UpdateChunk(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
//void LAB_NotifyChunkLater(LAB_World* world, int x, int y, int z/*, LAB_ChunkPeekType flags*/);
void LAB_UpdateChunkLater(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);

LAB_Block* LAB_GetBlock(LAB_World* world, int x, int y, int z);
bool LAB_SetBlock(LAB_World* world, int x, int y, int z, LAB_Block* block); // return false on failure
bool LAB_FillBlocks(LAB_World* world, int x0, int y0, int z0, int x1, int y1, int z1, LAB_Block* block); // return false on failure


// dir should be an unit vector
int LAB_TraceBlock(LAB_World* world, int max_distance, float vpos[3], float dir[3], unsigned block_flags,
                   LAB_OUT int target[3], LAB_OUT int prev[3], LAB_OUT float hit[3]);

void LAB_WorldTick(LAB_World* world);