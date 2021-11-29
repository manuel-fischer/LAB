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



#define LAB_MAX_CHUNK_AGE 256
//#define LAB_MAX_LOAD_CHUNK 16
#define LAB_MAX_LOAD_CHUNK 256

//#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)+1)
//#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)+2)
#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)+3)
#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)*2+3)





typedef struct LAB_World LAB_World;

typedef void(LAB_IView_ChunkViewer)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
typedef bool(LAB_IView_ChunkKeeper)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
typedef void(LAB_IView_ChunkUnlinker)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
typedef void(LAB_IView_Position)(void* user, LAB_World* world, LAB_OUT double xyz[3]);

typedef struct LAB_IView
{
    LAB_IView_ChunkViewer* chunkview;
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

    LAB_ChunkTBL chunks;

    LAB_ChunkPosQueue gen_queue;
    //LAB_ChunkPos2Queue update_queue;

    LAB_WorldConfig cfg;

    //LAB_LightUpdateQueue light_queue;

    LAB_PerfInfo* perf_info;
} LAB_World;


/**
 *  Init world, return 0 on error
 */
int  LAB_ConstructWorld(LAB_World* world);
/**
 *  Destruct world
 */
void LAB_DestructWorld(LAB_World* world);


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


void LAB_GetChunkNeighbors(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27]);


/**
 *  Refresh a chunk by notifying the view
 */
//void LAB_NotifyChunk(LAB_World* world, int x, int y, int z);
void LAB_UpdateChunk(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
//void LAB_NotifyChunkLater(LAB_World* world, int x, int y, int z/*, LAB_ChunkPeekType flags*/);
void LAB_UpdateChunkLater(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);

LAB_Block* LAB_GetBlock(LAB_World* world, int x, int y, int z);
void LAB_SetBlock(LAB_World* world, int x, int y, int z, LAB_Block* block);
void LAB_FillBlocks(LAB_World* world, int x0, int y0, int z0, int x1, int y1, int z1, LAB_Block* block);


// dir should be an unit vector
int LAB_TraceBlock(LAB_World* world, int max_distance, float vpos[3], float dir[3], unsigned block_flags,
                   LAB_OUT int target[3], LAB_OUT int prev[3], LAB_OUT float hit[3]);


void LAB_WorldTick(LAB_World* world, uint32_t delta_ms, void(*cb)(void*), void* cb_user);



LAB_INLINE LAB_Chunk* LAB_GetNeighborhoodRef(LAB_Chunk*const neighborhood[27], int x, int y, int z, int* /*out*/ index);
LAB_INLINE LAB_Block* LAB_GetNeighborhoodBlock(LAB_Chunk*const neighborhood[27], int x, int y, int z);






/**
 *  the origin of x,y,z is at (0,0,0) of the chunk at [1+3+3*3]
 */
LAB_ALWAYS_INLINE LAB_INLINE
LAB_Chunk* LAB_GetNeighborhoodRef(LAB_Chunk*const neighborhood[27], int x, int y, int z, int* /*out*/ index)
{
    LAB_ASSUME(x >= -16 && x < 32);
    LAB_ASSUME(y >= -16 && y < 32);
    LAB_ASSUME(z >= -16 && z < 32);

    int cx, cy, cz,  ix, iy, iz;

    cx = (x+16) >> LAB_CHUNK_SHIFT;
    ix = LAB_CHUNK_X(x & LAB_CHUNK_MASK);

    cy = 3*((y+16) >> LAB_CHUNK_SHIFT);
    iy = LAB_CHUNK_Y(y & LAB_CHUNK_MASK);

    cz = 3*3*((z+16) >> LAB_CHUNK_SHIFT);
    iz = LAB_CHUNK_Z(z & LAB_CHUNK_MASK);

    *index = ix+iy+iz;
    return neighborhood[cx+cy+cz];
}

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Block* LAB_GetNeighborhoodBlock(LAB_Chunk*const neighborhood[27], int x, int y, int z)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
    //if(LAB_UNLIKELY(chunk == NULL)) return &LAB_BLOCK_OUTSIDE;
    if(chunk == NULL) return &LAB_BLOCK_OUTSIDE;
    return chunk->blocks[block_index];
}


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_GetVisualNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, int face, LAB_Color default_color)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
    if(chunk == NULL) return default_color;


    LAB_Color c = 0;
    LAB_Color max = 0;

    int mask =        1  << (face>>1);
    int bit  = !(face&1) << (face>>1);
    LAB_UNROLL(8)
    for(int i = 0; i < 8; ++i)
    {
        LAB_Color cf = chunk->light[block_index].quadrants[i];
        if((i & mask) != bit)
        {
            cf = LAB_MixColor50(cf, 0);
        }
        max = LAB_MaxColor(max, cf);
        c = LAB_AddColor(c, cf >> 2 & 0x3f3f3f3f);
    }

    return max;
}