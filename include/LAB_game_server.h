#pragma once
// TODO remove

#include "LAB_stdinc.h"

#include "LAB_htl_config.h"
#include "LAB_world.h"
#include "LAB_chunk_lock.h"
#include "LAB_game_server_stats.h"

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
    //return 0;

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

struct LAB_GameServer;

typedef void(*LAB_ChunkCallback)(struct LAB_GameServer* srv,
                                 LAB_Chunk* chunks[27], LAB_ChunkPos pos,
                                 void* uparam);

#define LAB_CHUNK_QUEUE_NAME LAB_ChunkQueue
#define LAB_CHUNK_QUEUE_TYPE LAB_Chunk
#define LAB_CHUNK_QUEUE_PREV(chunk) ((chunk)->queue_prev)
#define LAB_CHUNK_QUEUE_NEXT(chunk) ((chunk)->queue_next)

#define HTL_PARAM LAB_CHUNK_QUEUE
#include "HTL/node_queue.t.h"
#undef HTL_PARAM

#define LAB_GameServerChunkQueue_Empty(q) ((q)->first == NULL)

typedef struct LAB_GameServer
{
    LAB_World* world;
    LAB_ChunkQueue tasks[LAB_CHUNK_PRIORITIES];
    
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

    bool stop;

    bool pause;
    bool main_waiting;
    size_t threads_paused;

    size_t update_pointer;
    size_t unload_pointer;

    LAB_GameServerStats stats;
} LAB_GameServer;


/**
 *  create server and start threads
 */
bool LAB_GameServer_Create(LAB_GameServer* srv,
                            LAB_World* world,
                            size_t worker_count);

/**
 *  stop threads and destroy server
 */
void LAB_GameServer_Destroy(LAB_GameServer* srv);

void LAB_GameServer_Tick(LAB_GameServer* srv);
void LAB_GameServer_GetStats(LAB_GameServer* srv, LAB_GameServerStats* out);




// parity: LAB_PARITY_POS or LAB_PARITY_NONE
// do not use this for terminating
bool LAB_GameServer_PushChunkTask(LAB_GameServer* srv,
                                   LAB_Chunk* chunk,
                                   unsigned int update);



// DO NOT CALL INSIDE TASK-ROUTINES
void LAB_GameServer_Lock(LAB_GameServer* srv);
bool LAB_GameServer_LockTimeout(LAB_GameServer* srv, LAB_Nanos ns);
void LAB_GameServer_Unlock(LAB_GameServer* srv);


// Locks chunk and its neighbors
// You should not change neighbors while locking a specific chunk
bool LAB_GameServer_Locked_TryLockChunk(LAB_GameServer* srv, LAB_Chunk* chunks[27]);
void LAB_GameServer_Locked_UnlockChunk(LAB_GameServer* srv, LAB_Chunk* chunks[27]);

void LAB_GameServer_Lock1Chunk(LAB_GameServer* srv, LAB_Chunk* chunk);
void LAB_GameServer_Unlock1Chunk(LAB_GameServer* srv, LAB_Chunk* chunk);