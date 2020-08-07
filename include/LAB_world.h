#pragma once

#include "LAB_chunk.h"
#include "LAB_stdinc.h"

#include "LAB_check.h"
#include "LAB_opt.h"


typedef struct LAB_ChunkPos
{
    //_Alignas(4) struct {
    //int x, y, z;
    int16_t x, y, z, unused;
    //};
} LAB_ChunkPos;
LAB_CHECK_STRUCT_SIZE(LAB_ChunkPos);

unsigned LAB_ChunkPosHash(LAB_ChunkPos);
int LAB_ChunkPosComp(LAB_ChunkPos, LAB_ChunkPos);


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
#define LAB_CHUNK_MAP_NULL_REPR            entry->value == NULL

#define HTL_PARAM LAB_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM



typedef struct LAB_World LAB_World;

typedef void(LAB_ChunkViewer)(void* user, LAB_World* world, int x, int y, int z);


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

    /*size_t chunk_count, chunk_capacity;
    LAB_ChunkEntry* chunks;*/
    LAB_ChunkMap chunks;


} LAB_World;


typedef unsigned LAB_ChunkPeekType;
#define LAB_CHUNK_EXISTING       0
#define LAB_CHUNK_GENERATE       1
#define LAB_CHUNK_GENERATE_LATER 1
//#define LAB_CHUNK_GENERATE_LATER 2




LAB_World* LAB_CreateWorld(void);
void       LAB_DestroyWorld(LAB_World* world);


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
void LAB_NotifyChunk(LAB_World* world, int x, int y, int z);


LAB_Block* LAB_GetBlock(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags);
void LAB_SetBlock(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags, LAB_Block* block);
void LAB_FillBlocks(LAB_World* world, int x0, int y0, int z0, int x1, int y1, int z1, LAB_ChunkPeekType flags, LAB_Block* block);


int LAB_TraceBlock(LAB_World* world, int max_distance, float vpos[3], float dir[3], LAB_ChunkPeekType flags, unsigned block_flags,
                   /*out*/ int target[3],/*out*/ int prev[3],/*out*/ float hit[3]);












/**
 *  the origin of x,y,z is at (0,0,0) of the chunk at [1+3+3*3]
 */
LAB_INLINE
LAB_Chunk* LAB_GetNeighborhoodRef(LAB_Chunk* neighborhood[27], int x, int y, int z, int* /*out*/ index)
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
LAB_Block* LAB_GetNeighborhoodBlock(LAB_Chunk* neighborhood[27], int x, int y, int z)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
#if 0 // branchless
    static LAB_Block*const ptr_outside = &LAB_BLOCK_OUTSIDE;

    LAB_Block*const* ptr0 = &ptr_outside;
    LAB_Block*const* ptr1 = &chunk->blocks[block_index];
    //LAB_Block*const* ptr1 = chunk?&chunk->blocks[block_index]:NULL;
    /*static volatile void* calc;
    calc = ptr0;
    calc = ptr1;*/
    //if(__builtin_unpredictable(chunk == NULL)) ptr1 = ptr0; // cmov
    //LAB_CMOV(chunk==NULL, ptr1, ptr0);
    (void)LAB_UNLIKELY(chunk == NULL);
    LAB_CMOV_NOT(chunk, ptr1, ptr0);
    //if(LAB_UNLIKELY(chunk == NULL)) ptr1 = ptr0; // cmov

    return *ptr1;
#elif 0 // branchless
    //block_index *= (chunk!=NULL);
    block_index &= -(chunk!=NULL);
    static LAB_Block*const ptr_outside = &LAB_BLOCK_OUTSIDE;

    //LAB_Block** arrays = (chunk==NULL) ? &ptr_outside : chunk->blocks;
    LAB_Block*const* arrays[2] = {&ptr_outside, &chunk->blocks[0]};

    return arrays[chunk!=NULL][block_index];
#else
    if(LAB_UNLIKELY(chunk == NULL)) return &LAB_BLOCK_OUTSIDE;
    return chunk->blocks[block_index];
#endif
}

LAB_HOT LAB_INLINE
int LAB_GetNeighborhoodLight(LAB_Chunk* neighborhood[27], int x, int y, int z)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);

    if(LAB_UNLIKELY(chunk == NULL)) return 255;
    return chunk->light[block_index];
}
