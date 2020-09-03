#include "LAB_chunk.h"

#include "LAB_memory.h"
#include "LAB_error.h"

#include "LAB_stdinc.h"
#include "LAB_opt.h"

LAB_Chunk* LAB_CreateChunk(LAB_Block* fill_block)
{
    LAB_Chunk* chunk;

    chunk = LAB_Malloc(sizeof *chunk);
    if(LAB_UNLIKELY(chunk == NULL))
        return (LAB_SetError("CreateChunk failed to allocate"), NULL);

    for(int i = 0; i < LAB_CHUNK_LENGTH; ++i) chunk->blocks[i] = fill_block;
    memset(chunk->light, 0, sizeof chunk->light);

    chunk->dirty = 1;

    return chunk;
}

void LAB_DestroyChunk(LAB_Chunk* chunk)
{
    LAB_Free(chunk);
}
