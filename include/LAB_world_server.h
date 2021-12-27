#pragma once

#include "LAB_stdinc.h"

#include "LAB_htl_config.h"
#include "LAB_world.h"


/*enum LAB_Action
{
    LAB_CQ_TERMINATE, // Not counted as pending
    LAB_CQ_GENERATE,
    LAB_CQ_LIGHT,
};*/

#define LAB_CHUNK_PRIORITIES 8

LAB_INLINE
int LAB_ChunkPriority_Distance(int chunk_distance_sq)
{
    int d = chunk_distance_sq;
#if 1
    return d < 16 ? d <  4 ? d <  2 ? 0
                                    : 1
                           : d <  8 ? 2
                                    : 3
                  : d < 64 ? d < 32 ? 4
                                    : 5
                           : d <128 ? 6
                                    : 7;
#else
    return d < 25 ? d <  9 ? d <  4 ? 0
                                    : 1
                           : d < 16 ? 2
                                    : 3
                  : d < 49 ? d < 36 ? 4
                                    : 5
                           : d < 64 ? 6
                                    : 7;
#endif
}

LAB_INLINE
int LAB_ChunkPriority(LAB_World* w, LAB_Chunk* chunk)
{
    int distance = (w->px-chunk->pos.x)*(w->px-chunk->pos.x)
                 + (w->py-chunk->pos.y)*(w->py-chunk->pos.y)
                 + (w->pz-chunk->pos.z)*(w->pz-chunk->pos.z);
    
    return LAB_ChunkPriority_Distance(distance);
}


typedef enum LAB_UpdateParity
{
    LAB_PARITY_FROM_POS = 0,
    LAB_PARITY_000      = 0,
    LAB_PARITY_100      = 1,
    LAB_PARITY_010      = 2,
    LAB_PARITY_110      = 3,
    LAB_PARITY_001      = 4,
    LAB_PARITY_101      = 5,
    LAB_PARITY_011      = 6,
    LAB_PARITY_111      = 7,
    LAB_PARITY_NONE     = 8,
    //LAB_PARITY_ALL    = 9,

    LAB_PARITY_MAIN     = 10, // run in main thread

    LAB_PARITY_COUNT
} LAB_UpdateParity;

struct LAB_WorldServer;

typedef void(*LAB_ChunkCallback)(struct LAB_WorldServer* srv,
                                 LAB_Chunk* chunk, LAB_ChunkPos pos,
                                 void* uparam);

typedef struct LAB_WorldTask
{
    LAB_ChunkCallback cb; // NULL for terminate
    LAB_Chunk* chunk;
    LAB_ChunkPos pos;
    void* uparam;
    
} LAB_WorldTask;

#define LAB_WORLD_TASK_QUEUE_TYPE LAB_WorldTask
#define LAB_WORLD_TASK_QUEUE_NAME LAB_WorldTaskQueue

#define HTL_PARAM LAB_WORLD_TASK_QUEUE
#include "HTL/queue.t.h"
#undef HTL_PARAM


#define LAB_CHUNK_QUEUE_NAME LAB_ChunkQueue
#define LAB_CHUNK_QUEUE_TYPE LAB_Chunk
#define LAB_CHUNK_QUEUE_PREV(chunk) ((chunk)->queue_prev)
#define LAB_CHUNK_QUEUE_NEXT(chunk) ((chunk)->queue_next)

#define HTL_PARAM LAB_CHUNK_QUEUE
#include "HTL/node_queue.t.h"
#undef HTL_PARAM

#define LAB_WorldServerChunkQueue_Empty(q) ((q)->first == NULL)

typedef struct LAB_WorldServer
{
    LAB_World* world;
    LAB_ChunkQueue tasks[LAB_PARITY_COUNT][LAB_CHUNK_PRIORITIES];
    
    SDL_mutex* mutex;
    SDL_cond* update; // Signaled whenever an element was added to the queue
                      // or a stop has been requested
    SDL_cond* paused;
    SDL_cond* finished; // Signaled, whenever a task was finished in a worker thread
                        // Only the main thread waits for it
                        // Only signaled when the main thread waits for it
                        // (main_waiting)


    SDL_Thread** workers;
    size_t worker_count;

    LAB_UpdateParity parity;
    size_t threads_parity; // number of threads, that currently work on the current parity
    size_t rest_parity;
    bool stop;
    size_t parity_task_count;

    bool pause;
    bool main_waiting;
    size_t threads_paused;

    size_t update_pointer;

    size_t completed_cycles_thrd;
    size_t completed_cycles; // completed parity cycles since the last call to LAB_WorldServer_Tick
    size_t completed_mainthread;

    size_t timestamp;
    size_t max_age; // maximum age of chunks currently in the queue

    size_t task_count;

    LAB_Nanos runtime;
    LAB_Nanos runtime_computed;
} LAB_WorldServer;

/**
 *  create server and start threads
 */
bool LAB_WorldServer_Create(LAB_WorldServer* srv,
                            LAB_World* world,
                            size_t worker_count);

/**
 *  stop threads and destroy server
 */
void LAB_WorldServer_Destroy(LAB_WorldServer* srv);

void LAB_WorldServer_Tick(LAB_WorldServer* srv);




// parity: LAB_PARITY_POS or LAB_PARITY_NONE
// do not use this for terminating
bool LAB_WorldServer_PushChunkTask(LAB_WorldServer* srv,
                                   int /*LAB_UpdateParity*/ parity,
                                   LAB_Chunk* chunk,
                                   unsigned int update);



// DO NOT CALL INSIDE TASK-ROUTINES
void LAB_WorldServer_Lock(LAB_WorldServer* srv);
bool LAB_WorldServer_LockTimeout(LAB_WorldServer* srv, LAB_Nanos ns);
void LAB_WorldServer_Unlock(LAB_WorldServer* srv);


// Locks chunk and its neighbors
// You should not change neighbors while locking a specific chunk
void LAB_WorldServer_LockChunk(LAB_WorldServer* srv, LAB_Chunk* chunk);
void LAB_WorldServer_UnlockChunk(LAB_WorldServer* srv, LAB_Chunk* chunk);
