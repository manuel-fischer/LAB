#if 0
#include "LAB_chunk_queue.h"
#include "LAB_obj.h"
#include "LAB_memory.h"

#if 0
bool LAB_ChunkQueue_Create(LAB_ChunkQueue* q, size_t capacity)
{
    LAB_OBJ_SDL(q->sem_cnt = SDL_CreateSemaphore(0),
                SDL_DestroySemaphore(q->sem_cnt),
    LAB_OBJ_SDL(q->sem_free = SDL_CreateSemaphore(capacity),
                SDL_DestroySemaphore(q->sem_free),
    LAB_OBJ_SDL(q->mtx_lock = SDL_CreateMutex(),
                SDL_DestroyMutex(q->mtx_lock),
    LAB_OBJ(q->entries = LAB_Malloc(sizeof(LAB_ChunkQueueEnt)*capacity),
            LAB_Free(q->entries),
    {
        q->capacity = capacity;
        q->head = 0;
        q->count = 0;
        
        return true;
    }););););
    return false;
}

void LAB_ChunkQueue_Destroy(LAB_ChunkQueue* q)
{
    LAB_Free(q->entries);
    SDL_DestroyMutex(q->mtx_lock);
    SDL_DestroySemaphore(q->sem_free);
    SDL_DestroySemaphore(q->sem_cnt);
}

bool LAB_ChunkQueue_TryPush(LAB_ChunkQueue* q, LAB_Chunk* cnk, int action)
{
    SDL_LockMutex(q->mtx_lock);

    bool inserted = q->count != q->capacity;

    if(inserted)
    {
        size_t index = q->head+q->count;
        if(index > q->capacity) index -= q->capacity;

        q->entries[index].cnk    = cnk;
        q->entries[index].action = action;

        ++q->count;
        SDL_SemWait(q->sem_free); // decrement, should never block
    }

    SDL_UnlockMutex(q->mtx_lock);
    SDL_SemPost(q->sem_cnt);

    return inserted;
}

void LAB_ChunkQueue_Push(LAB_ChunkQueue* q, LAB_Chunk* cnk, int action)
{
    SDL_SemWait(q->sem_free);
    SDL_LockMutex(q->mtx_lock);

    size_t index = q->head+q->count;
    if(index > q->capacity) index -= q->capacity;

    q->entries[index].cnk    = cnk;
    q->entries[index].action = action;

    ++q->count;

    SDL_UnlockMutex(q->mtx_lock);
    SDL_SemPost(q->sem_cnt);
}

void LAB_ChunkQueue_Pop(LAB_ChunkQueue* q, LAB_Chunk** cnk, int* action)
{
    SDL_SemWait(q->sem_cnt);
    SDL_LockMutex(q->mtx_lock);

    *cnk    = q->entries[q->head].cnk;
    *action = q->entries[q->head].action;

    ++q->head;
    if(q->head == q->capacity) q->head = 0;
    --q->count;
    
    SDL_UnlockMutex(q->mtx_lock);
    SDL_SemPost(q->sem_free);
}
#endif

#define HTL_PARAM LAB_CHUNK_QUEUE
#include "HTL/mt_queue.t.c"
#undef HTL_PARAM
#endif
static int empty;