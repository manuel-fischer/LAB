#pragma once
#if 0

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "LAB_chunk.h"

typedef struct LAB_ChunkQueueEnt
{
    LAB_Chunk* cnk;
    int action;

} LAB_ChunkQueueEnt;

#if 0
// Do not move a LAB_ChunkQueue around, because it stores pointers into itself
typedef struct LAB_ChunkQueue
{
    SDL_sem*           sem_cnt;
    SDL_sem*           sem_free;
    SDL_mutex*         mtx_lock;
    LAB_ChunkQueueEnt* entries;
    size_t capacity;
    size_t head;
    size_t count; // mirrors sem_cnt

} LAB_ChunkQueue;

bool LAB_ChunkQueue_Create(LAB_ChunkQueue* q, size_t capacity);
void LAB_ChunkQueue_Destroy(LAB_ChunkQueue* q);

bool LAB_ChunkQueue_TryPush(LAB_ChunkQueue* q, LAB_Chunk* cnk, int action);
void LAB_ChunkQueue_Push(LAB_ChunkQueue* q, LAB_Chunk* cnk, int action);
void LAB_ChunkQueue_Pop(LAB_ChunkQueue* q, LAB_Chunk** cnk, int* action);
#endif

#include "LAB_htl_config.h"

#define LAB_CHUNK_QUEUE_TYPE LAB_ChunkQueueEnt
#define LAB_CHUNK_QUEUE_NAME LAB_ChunkQueue

#define HTL_PARAM LAB_CHUNK_QUEUE
#include "HTL/mt_queue.t.h"
#undef HTL_PARAM
#endif