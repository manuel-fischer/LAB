#pragma once

#include "LAB_chunk.h"
#include "LAB_stdinc.h"

#include "LAB_check.h"
#include "LAB_opt.h"


typedef struct LAB_ChunkPos
{
    int16_t x, y, z;
    LAB_PAD(int16_t);
} LAB_ChunkPos;
LAB_CHECK_STRUCT_SIZE(LAB_ChunkPos);

unsigned LAB_ChunkPosHash(LAB_ChunkPos);
int LAB_ChunkPosComp(LAB_ChunkPos, LAB_ChunkPos);

#define LAB_MAX_CHUNK_AGE 256
//#define LAB_MAX_LOAD_CHUNK 16
#define LAB_MAX_LOAD_CHUNK 256

#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)+1)
#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)+2)



#define LAB_CHUNK_MAP_NAME                 LAB_ChunkMap
#define LAB_CHUNK_MAP_KEY_TYPE             LAB_ChunkPos
#define LAB_CHUNK_MAP_VALUE_TYPE           LAB_Chunk*
#define LAB_CHUNK_MAP_HASH_FUNC            LAB_ChunkPosHash
#define LAB_CHUNK_MAP_COMP_FUNC            LAB_ChunkPosComp
#define LAB_CHUNK_MAP_CALLOC               LAB_Calloc
#define LAB_CHUNK_MAP_FREE                 LAB_Free
#define LAB_CHUNK_MAP_LOAD_NUM             3
#define LAB_CHUNK_MAP_LOAD_DEN             4
#define LAB_CHUNK_MAP_GROW_FACTOR          2
#define LAB_CHUNK_MAP_INITIAL_CAPACITY     16
#define LAB_CHUNK_MAP_NULL_REPR            (entry->value == NULL)

#define HTL_PARAM LAB_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM


#define LAB_CHUNKPOS_QUEUE_NAME     LAB_ChunkPosQueue
#define LAB_CHUNKPOS_QUEUE_TYPE     LAB_ChunkPos
#define LAB_CHUNKPOS_QUEUE_CAPACITY LAB_MAX_LOAD_CHUNK

#define HTL_PARAM LAB_CHUNKPOS_QUEUE
#include "HTL_queue.t.h"
#undef HTL_PARAM

#define LAB_CHUNKPOS2_QUEUE_NAME     LAB_ChunkPos2Queue
#define LAB_CHUNKPOS2_QUEUE_TYPE     LAB_ChunkPos
#define LAB_CHUNKPOS2_QUEUE_CAPACITY 128

#define HTL_PARAM LAB_CHUNKPOS2_QUEUE
#include "HTL_queue.t.h"
#undef HTL_PARAM


/*typedef struct LAB_LightUpdate
{
    int x, y, z;
} LAB_LightUpdate;

#define LAB_LIGHT_UPDATE_QUEUE_NAME LAB_LightUpdateQueue
#define LAB_LIGHT_UPDATE_QUEUE_TYPE LAB_LightUpdate
#define LAB_LIGHT_UPDATE_QUEUE_CAPACITY (16*16*16*4)

#define HTL_PARAM LAB_LIGHT_UPDATE_QUEUE
#include "HTL_queue.t.h"
#undef HTL_PARAM*/


typedef struct LAB_World LAB_World;

typedef void(LAB_ChunkViewer)(void* user, LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update);
typedef bool(LAB_ChunkKeeper)(void* user, LAB_World* world, int x, int y, int z);


/*typedef struct LAB_ChunkEntry
{
    int x, y, z;
    LAB_Chunk* chunk;
} LAB_ChunkEntry;*/

typedef struct LAB_World
{
    LAB_ChunkGenerator* chunkgen;
    void*               chunkgen_user;

    LAB_ChunkViewer* chunkview;
    void*            chunkview_user;

    LAB_ChunkKeeper* chunkkeep;
    void*            chunkkeep_user;


    /*size_t chunk_count, chunk_capacity;
    LAB_ChunkEntry* chunks;*/
    LAB_ChunkMap chunks;
    //LAB_ChunkMap_Entry* last_entry; // TODO: invalid when chunk map gets reallocated

    LAB_ChunkPosQueue gen_queue;
    LAB_ChunkPos2Queue update_queue;

    size_t max_gen,
           max_update;

    //LAB_LightUpdateQueue light_queue;
} LAB_World;


typedef unsigned LAB_ChunkPeekType;
#define LAB_CHUNK_EXISTING       0
#define LAB_CHUNK_GENERATE       1
#define LAB_CHUNK_GENERATE_LATER 2




/**
 *  Init world, return 0 on error
 */
int  LAB_ConstructWorld(LAB_World* world);
/**
 *  Destruct world
 */
void LAB_DestructWorld(LAB_World* world);


/**
 *  Get chunk, create the chunk, when not existing, depending on flags
 *  Return chunk or NULL if chunk was not generated
 */
LAB_Chunk* LAB_GetChunk(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags);

/**
 *
 */
void LAB_GetChunkNeighborhood(LAB_World* world, LAB_Chunk* chunks[27], int x, int y, int z, LAB_ChunkPeekType flags);

/**
 *  Refresh a chunk by notifying the view
 */
//void LAB_NotifyChunk(LAB_World* world, int x, int y, int z);
void LAB_UpdateChunk(LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update);
//void LAB_NotifyChunkLater(LAB_World* world, int x, int y, int z/*, LAB_ChunkPeekType flags*/);
void LAB_UpdateChunkLater(LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update);

LAB_Block* LAB_GetBlock(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags);
void LAB_SetBlock(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags, LAB_Block* block);
void LAB_FillBlocks(LAB_World* world, int x0, int y0, int z0, int x1, int y1, int z1, LAB_ChunkPeekType flags, LAB_Block* block);


// dir should be an unit vector
int LAB_TraceBlock(LAB_World* world, int max_distance, float vpos[3], float dir[3], LAB_ChunkPeekType flags, unsigned block_flags,
                   /*out*/ int target[3],/*out*/ int prev[3],/*out*/ float hit[3]);


void LAB_WorldTick(LAB_World* world, uint32_t delta_ms);



LAB_INLINE LAB_Chunk* LAB_GetNeighborhoodRef(LAB_Chunk*const neighborhood[27], int x, int y, int z, int* /*out*/ index);
LAB_INLINE LAB_Block* LAB_GetNeighborhoodBlock(LAB_Chunk*const neighborhood[27], int x, int y, int z);
LAB_INLINE LAB_Color LAB_GetNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, LAB_Color default_color);
LAB_INLINE uint32_t LAB_World_PeekFlags3x3(LAB_Chunk* chunk, int x, int y, int z, unsigned flag);








/**
 *  the origin of x,y,z is at (0,0,0) of the chunk at [1+3+3*3]
 */
LAB_INLINE
LAB_Chunk* LAB_GetNeighborhoodRef(LAB_Chunk*const neighborhood[27], int x, int y, int z, int* /*out*/ index)
{
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

LAB_HOT LAB_INLINE
LAB_Block* LAB_GetNeighborhoodBlock(LAB_Chunk*const neighborhood[27], int x, int y, int z)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
    if(LAB_UNLIKELY(chunk == NULL)) return &LAB_BLOCK_OUTSIDE;
    return chunk->blocks[block_index];
}

LAB_HOT LAB_INLINE
LAB_Color LAB_GetNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, LAB_Color default_color)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);

    //if(LAB_UNLIKELY(chunk == NULL)) return LAB_RGB(255, 255, 255);
    if(chunk == NULL) return default_color;
    return chunk->light[block_index];
}

LAB_HOT LAB_INLINE
uint32_t LAB_World_PeekFlags3x3(LAB_Chunk* chunk, int x, int y, int z, unsigned flag)
{
    uint32_t bitset = 0, curbit = 1;
    for(int zz = z-1; zz <= z+1; ++zz)
    for(int yy = y-1; yy <= y+1; ++yy)
    for(int xx = x-1; xx <= x+1; ++xx)
    {
        if(xx >= 0 && xx < 16 &&
           yy >= 0 && yy < 16 &&
           zz >= 0 && zz < 16)
        {
            if(chunk->blocks[xx|yy<<4|zz<<8]->flags & flag)
                bitset |= curbit;
        }
        curbit <<= 1;
    }
    return bitset;
}
