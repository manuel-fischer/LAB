#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "LAB_chunk.h"

typedef struct LAB_ChunkQueueEnt
{
    LAB_Chunk* cnk;
    int action;

} LAB_ChunkQueueEnt;


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
