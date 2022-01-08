#include "LAB_chunk.h"

#include "LAB_memory.h"
#include "LAB_error.h"

#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_obj.h"

//#include <pthread.h>

#ifndef NDEBUG
#include "LAB_image.h"
#endif

#if 0
// Allocate 
#define LAB_PAGE_SIZE 0x1000
#define LAB_PAGE_COUNT 128

#define LAB_MALLOC_OVERHEAD 64
#define LAB_CHUNK_BLOCK_COUNT ((LAB_PAGE_SIZE*LAB_PAGE_COUNT-LAB_MALLOC_OVERHEAD) / sizeof(LAB_ChunkMem))

typedef union LAB_ChunkMem
{
    LAB_Chunk chunk;
    union LAB_ChunkMem* next;
} LAB_ChunkMem;

static LAB_ChunkMem* free_chunk_list = NULL;

LAB_STATIC_ASSUME(LAB_CHUNK_BLOCK_COUNT, "should be at least 1");

LAB_STATIC
LAB_Chunk* LAB_AllocChunkMem(void)
{
    if(free_chunk_list == NULL)
    {
        LAB_ChunkMem* mem = LAB_MallocN(LAB_CHUNK_BLOCK_COUNT, sizeof(LAB_ChunkMem));
        if(mem == NULL) return NULL;
        for(size_t i = 0; i < LAB_CHUNK_BLOCK_COUNT; ++i)
        {
            mem[i].next = free_chunk_list;
            free_chunk_list = &mem[i];
        }
    }
    LAB_ChunkMem* cm = free_chunk_list;
    free_chunk_list = free_chunk_list->next;
    return &cm->chunk;
}

LAB_STATIC
void LAB_FreeChunkMem(LAB_Chunk* chunk)
{
    if(chunk == NULL) return;
    LAB_ChunkMem* cm = (LAB_ChunkMem*)chunk;
    cm->next = free_chunk_list;
    free_chunk_list = cm;
}

#else

#define LAB_AllocChunkMem() ((LAB_Chunk*)LAB_Malloc(sizeof(LAB_Chunk)))
#define LAB_FreeChunkMem(chunk) LAB_Free(chunk)

#endif



LAB_Chunk* LAB_CreateChunk(LAB_ChunkPos pos)
{
    LAB_Chunk* chunk;

    chunk = LAB_AllocChunkMem();
    if(LAB_UNLIKELY(chunk == NULL))
        return (LAB_SetError("CreateChunk failed to allocate"), NULL);

    memset(chunk->light, 0, sizeof chunk->light);

    chunk->pos = pos;

    chunk->modified = false;
    chunk->empty = true;

    chunk->dirty = 1;
    chunk->dirty_blocks = ~0;
    chunk->relit_blocks = ~0;
    chunk->dirty_neighbors = 63;
    chunk->relit_neighbors = 63;


    chunk->light_generated = 0;
    chunk->generated = 0;

    chunk->age = 0;
    // initialize neighbors with NULL
    for(int face = 0; face < 6; ++face)
        chunk->_neighbors[face] = NULL;
    chunk->view_user = NULL;


    //chunk->is_accessed = false;
    chunk->access_mode = 0;

    memset(&chunk->stats, 0, sizeof chunk->stats);

    chunk->queue_prev = NULL;
    chunk->queue_next = NULL;

    //chunk->reenqueue = 0;

    return chunk;
}

void LAB_DestroyChunk(LAB_Chunk* chunk)
{
    LAB_ASSUME(chunk);

    // unlink neighbors
    LAB_UnlinkChunk(chunk);
    LAB_DestroyChunk_Unlinked(chunk);
}

void LAB_UnlinkChunk(LAB_Chunk* chunk)
{
    LAB_ASSUME(chunk);

    // unlink neighbors
    for(int face = 0; face < 6; ++face)
    {
        LAB_Chunk* neighbor = chunk->_neighbors[face];
        if(neighbor)
        {
            LAB_ASSUME(neighbor->_neighbors[face^1] == chunk);
            neighbor->_neighbors[face^1] = NULL;
        }
    }
}

void LAB_DestroyChunk_Unlinked(LAB_Chunk* chunk)
{
    LAB_ASSUME(chunk);


    //LAB_RWLock_Destroy(&chunk->lock);
    

    //LAB_Free(chunk);
    LAB_FreeChunkMem(chunk);
}


void LAB_FillChunk(LAB_Chunk* chunk, LAB_Block* fill_block)
{
    LAB_ASSERT(LAB_READABLE(fill_block));
    for(int i = 0; i < LAB_CHUNK_LENGTH; ++i) chunk->blocks[i] = fill_block;
}






/*
void LAB_Chunk_LockRead(LAB_Chunk* chunk)
{
    LAB_RWLock_RLock(&chunk->lock);
}

void LAB_Chunk_UnlockRead(LAB_Chunk* chunk)
{
    LAB_RWLock_RUnlock(&chunk->lock);
}

void LAB_Chunk_LockWrite(LAB_Chunk* chunk)
{
    LAB_RWLock_WLock(&chunk->lock);
}

void LAB_Chunk_UnlockWrite(LAB_Chunk* chunk)
{
    LAB_RWLock_WUnlock(&chunk->lock);
}



void LAB_Chunk_LockNeighbors(LAB_Chunk* chunks[27])
{
    // Lock lesser coordinates first, avoids deadlock
    for(int i = 0; i < 27; ++i)
    {
        if(chunks[i]) 
        {
            if(i == 1+3+9)
                LAB_Chunk_LockRead(chunks[i]);
            else
                LAB_Chunk_LockWrite(chunks[i]);
        }
    }
}

void LAB_Chunk_UnlockNeighbors(LAB_Chunk* chunks[27])
{
    for(int i = 0; i < 27; ++i)
    {
        if(chunks[i]) 
        {
            if(i == 1+3+9)
                LAB_Chunk_UnlockRead(chunks[i]);
            else
                LAB_Chunk_UnlockWrite(chunks[i]);
        }
    }
}
*/