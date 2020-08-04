#pragma once

#include "LAB_chunk.h"
#include "LAB_stdinc.h"


typedef struct LAB_ChunkPos
{
    int x, y, z;
} LAB_ChunkPos;

unsigned LAB_ChunkPosHash(LAB_ChunkPos);
int LAB_ChunkPosComp(LAB_ChunkPos, LAB_ChunkPos);


#define LAB_CHUNK_MAP_NAME             LAB_ChunkMap
#define LAB_CHUNK_MAP_KEY_TYPE         LAB_ChunkPos
#define LAB_CHUNK_MAP_VALUE_TYPE       LAB_Chunk*
#define LAB_CHUNK_MAP_HASH_FUNC        LAB_ChunkPosHash
#define LAB_CHUNK_MAP_COMP_FUNC        LAB_ChunkPosComp
#define LAB_CHUNK_MAP_CALLOC           LAB_Calloc
#define LAB_CHUNK_MAP_FREE             LAB_Free
#define LAB_CHUNK_MAP_LOAD_NUM         3
#define LAB_CHUNK_MAP_LOAD_DEN         4
#define LAB_CHUNK_MAP_INITIAL_CAPACITY 16
#define LAB_CHUNK_MAP_NULL_REPR        entry->value == NULL

#define HTL_PARAM LAB_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM



typedef struct LAB_World LAB_World;

typedef void(LAB_ChunkViewer)(void* user, LAB_World* world, int x, int y, int z);


typedef struct LAB_ChunkEntry
{
    int x, y, z;
    LAB_Chunk* chunk;
} LAB_ChunkEntry;

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




LAB_World* LAB_CreateWorld();
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
