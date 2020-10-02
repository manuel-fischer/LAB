#pragma once

#include "LAB_block.h"
#include "LAB_light_node.h"
#include "LAB_color.h"

typedef struct LAB_World LAB_World;

enum LAB_ChunkUpdate
{
    LAB_CHUNK_UPDATE_LIGHT = 1, // light changed, usually by neighboring chunk
    LAB_CHUNK_UPDATE_BLOCK_ = 2, // block changed, usually in combination with light
    LAB_CHUNK_UPDATE_BLOCK = 2+1,
    LAB_CHUNK_UPDATE_LOCAL = 2+0*4, // the chunk itself has changed -> higher priority
                                // when only the neighboring chunk had changed, the update
                                // is not propagated
};
typedef int LAB_ChunkUpdate;

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
    LAB_Block* blocks[LAB_CHUNK_LENGTH];
    LAB_Color  light[LAB_CHUNK_LENGTH];
    unsigned int dirty:8, modified:1;
    int age;
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
