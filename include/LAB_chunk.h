#pragma once

#include "LAB_block.h"
#include "LAB_light_node.h"

typedef struct LAB_World LAB_World;


#define LAB_CHUNK_SHIFT  4
#define LAB_CHUNK_SIZE   (1 << LAB_CHUNK_SHIFT)
#define LAB_CHUNK_LENGTH (LAB_CHUNK_SIZE*LAB_CHUNK_SIZE*LAB_CHUNK_SIZE)
#define LAB_CHUNK_MASK   (LAB_CHUNK_SIZE - 1)


#define LAB_CHUNK_X(x) (x)
#define LAB_CHUNK_Y(y) (LAB_CHUNK_SIZE*(y))
#define LAB_CHUNK_Z(z) (LAB_CHUNK_SIZE*LAB_CHUNK_SIZE*(z))
#define LAB_CHUNK_OFFSET(x, y, z) ((x) + LAB_CHUNK_SIZE*((y) + LAB_CHUNK_SIZE*(z)))

typedef struct LAB_Chunk
{
    LAB_Block*    blocks[LAB_CHUNK_LENGTH];
    int light[LAB_CHUNK_LENGTH];
} LAB_Chunk;

typedef LAB_Chunk* (LAB_ChunkGenerator)(void* user, LAB_World* world, int x, int y, int z);

/**
 *  Create chunk filled with fill_block
 *  Return NULL on failure
 */
LAB_Chunk* LAB_CreateChunk(LAB_Block* fill_block);

/**
 *
 */
void LAB_DestroyChunk(LAB_Chunk* chunk);
