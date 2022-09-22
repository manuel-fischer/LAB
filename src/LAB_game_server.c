// TODO remove
#include "LAB_game_server.h"
#include "LAB_memory.h"
#include "LAB_obj.h"
#include "LAB_opt.h"
#include "LAB_thread.h"

#include "LAB_chunk_neighborhood.h"
#include "LAB_game_server_routines.h"

LAB_STATIC int LAB_GameServer_ThreadRoutine(void* vsrv);

LAB_STATIC void LAB_GameServer_ValidateQueues(LAB_GameServer* srv);
LAB_STATIC void LAB_GameServer_StartTasks(LAB_GameServer* srv);
LAB_STATIC void LAB_GameServer_UnloadChunks(LAB_GameServer* srv);

LAB_STATIC void LAB_GameServer_SendStop(LAB_GameServer* srv);

#define HTL_PARAM LAB_CHUNK_QUEUE
#include "HTL/node_queue.t.c"
#undef HTL_PARAM

LAB_STATIC
bool LAB_ChunkQueues_AllEmpty(LAB_ChunkQueue qs[LAB_CHUNK_PRIORITIES])
{
    for(size_t i = 0; i < LAB_CHUNK_PRIORITIES; ++i)
        if(!LAB_ChunkQueue_IsEmpty(&qs[i])) return false;
    return true;
}

LAB_STATIC
int LAB_ChunkQueues_NextQueue(LAB_ChunkQueue qs[LAB_CHUNK_PRIORITIES])
{
    for(size_t i = 0; i < LAB_CHUNK_PRIORITIES; ++i)
        if(!LAB_ChunkQueue_IsEmpty(&qs[i])) return i;
    LAB_ASSERT_FALSE("All queues empty");
}

LAB_STATIC
LAB_Chunk* LAB_ChunkQueues_PopFront(LAB_ChunkQueue qs[LAB_CHUNK_PRIORITIES])
{
    for(size_t i = 0; i < LAB_CHUNK_PRIORITIES; ++i)
        if(!LAB_ChunkQueue_IsEmpty(&qs[i]))
            return LAB_ChunkQueue_PopFront(&qs[i]);

    LAB_ASSERT_FALSE("All queues empty");
}



LAB_STATIC
void LAB_SDL_LockMutexV(void* lock)
{
    SDL_LockMutex(lock);
    LAB_DbgPrintf("Locking %p\n", lock);
}

bool LAB_GameServer_Create(LAB_GameServer* srv,
                            LAB_World* world,
                            size_t worker_count)
{
    srv->world = world;
    srv->worker_count = worker_count;

    srv->stop = false;
    srv->pause = false;
    srv->main_waiting = false;
    srv->threads_paused = 0;
    srv->update_pointer = 0;
    srv->unload_pointer = 0;

    memset(&srv->stats, 0, sizeof srv->stats);

    for(size_t j = 0; j < LAB_CHUNK_PRIORITIES; ++j)
        LAB_ChunkQueue_Create(&srv->tasks[j]); // Don't need to be destroyed

    LAB_OBJ_SDL(srv->mutex = SDL_CreateMutex(),
                SDL_DestroyMutex(srv->mutex),
    LAB_OBJ(LAB_DbgAtHalt(LAB_SDL_LockMutexV, srv->mutex),
            LAB_DbgRemoveHalt(LAB_SDL_LockMutexV, srv->mutex),
    LAB_OBJ_SDL(srv->update = SDL_CreateCond(),
                SDL_DestroyCond(srv->update),
    LAB_OBJ_SDL(srv->paused = SDL_CreateCond(),
                SDL_DestroyCond(srv->paused),
    LAB_OBJ_SDL(srv->finished = SDL_CreateCond(),
                SDL_DestroyCond(srv->finished),
    LAB_OBJ(srv->workers = LAB_MallocN(worker_count, sizeof(*srv->workers)),
            LAB_Free(srv->workers),
    LAB_OBJ_FOR2(i, worker_count,
                 srv->workers[i] = SDL_CreateThread(LAB_GameServer_ThreadRoutine,
                                                   "LAB_GameServer_ThreadRoutine", srv),
                 LAB_GameServer_SendStop(srv),
                 SDL_WaitThread(srv->workers[i], NULL),
                 
    {
            return true;
    });););););););
    return false;
}

/**
 *  stop threads and destroy server
 */
void LAB_GameServer_Destroy(LAB_GameServer* srv)
{
    LAB_GameServer_SendStop(srv);

    for(size_t j = 0; j < srv->worker_count; ++j)
        SDL_WaitThread(srv->workers[j], NULL);

    LAB_Free(srv->workers);
    LAB_DbgRemoveHalt(LAB_SDL_LockMutexV, srv->mutex);

    SDL_DestroyCond(srv->finished); LAB_SDL_DEBUG_FREE_1();
    SDL_DestroyCond(srv->paused);   LAB_SDL_DEBUG_FREE_1();
    SDL_DestroyCond(srv->update);   LAB_SDL_DEBUG_FREE_1();
    SDL_DestroyMutex(srv->mutex);   LAB_SDL_DEBUG_FREE_1();
}


void LAB_GameServer_SendStop(LAB_GameServer* srv)
{
    LAB_ASSERT(LAB_IsMainThread());

    LAB_SDL_LockMutex(srv->mutex);
    srv->stop = true;
    LAB_SDL_UnlockMutex(srv->mutex);
    LAB_SDL_CondBroadcast(srv->update);
}


int LAB_GameServer_ThreadRoutine(void* vsrv)
{
    LAB_ASSERT(!LAB_IsMainThread());

    LAB_GameServer* srv = vsrv;

    LAB_Nanos prev = LAB_NanoSeconds();

    LAB_SDL_LockMutex(srv->mutex);

    while(true)
    {

        if(srv->pause)
        {
            srv->threads_paused++;
            if(srv->threads_paused == srv->worker_count)
                LAB_SDL_CondSignal(srv->paused);
            while(srv->pause && !srv->stop)
                LAB_SDL_CondWait(srv->update, srv->mutex);

            srv->threads_paused--;
        }

        if(srv->stop)
        {
            LAB_SDL_UnlockMutex(srv->mutex);
            return 0;
        }

        LAB_ChunkQueue* qs = srv->tasks;

        if(LAB_ChunkQueues_AllEmpty(qs))
        {
            LAB_SDL_CondWait(srv->update, srv->mutex);
            continue;
        }        
        
        int priority = LAB_ChunkQueues_NextQueue(qs);
        next_prio:;
        LAB_Chunk* chunk = LAB_ChunkQueue_PopFront(&qs[priority]);

        size_t age = srv->stats.timestamp - chunk->queue_timestamp;
        if(age > srv->stats.max_age) srv->stats.max_age = age;

        LAB_Chunk* neighbors[27];
        LAB_ChunkStageNeighbors(chunk->update_stage, chunk, neighbors);

        if(!LAB_GameServer_Locked_TryLockChunk(srv, neighbors))
        {
            LAB_ChunkQueue_PushBack(&qs[priority], chunk);
            srv->stats.requeued_count++;

            for(++priority; priority != LAB_CHUNK_PRIORITIES; ++priority)
            {
                if(!LAB_ChunkQueue_IsEmpty(&qs[priority])) break;
            }

            if(priority == LAB_CHUNK_PRIORITIES)
            {
                LAB_SDL_CondWait(srv->finished, srv->mutex);
                continue;
            }
            else
            {
                goto next_prio;
            }
        }

        int update_stage = chunk->update_stage;
        chunk->update_stage = 0;

        chunk->stats.executed_updates++;
        if(chunk->stats.executed_updates > srv->stats.max_chunk_updates)
            srv->stats.max_chunk_updates = chunk->stats.executed_updates;

        srv->stats.avg_chunk_updates = srv->stats.avg_chunk_updates*0.99 + chunk->stats.executed_updates*0.01;

        srv->stats.waiting_task_count--;

        srv->stats.update_counts[update_stage]++;
        LAB_SDL_UnlockMutex(srv->mutex);

        LAB_Nanos start = LAB_NanoSeconds();

        LAB_ChunkCallback cb = LAB_ChunkStageCallback(update_stage);
        cb(srv, chunk, chunk->pos, NULL);

        LAB_Nanos end = LAB_NanoSeconds();
        LAB_Nanos comp = end-start;
        LAB_Nanos cycle = end-prev;
        prev = end;

        LAB_SDL_LockMutex(srv->mutex);
        LAB_GameServer_Locked_UnlockChunk(srv, neighbors);

        srv->stats.runtime += cycle;
        srv->stats.runtime_computed += comp;
        srv->stats.update_runtimes[update_stage] += comp;

        if(srv->stats.runtime > 1000*1000*1000) // all 1 sec
        {
            srv->stats.runtime >>= 1;
            srv->stats.runtime_computed >>= 1;

            for(int i = 0; i < LAB_CHUNK_STAGE_COUNT; ++i)
                srv->stats.update_runtimes[i] >>= 1;
        }

        srv->stats.completed_task_count++;

        if(srv->main_waiting)
        {
            LAB_SDL_UnlockMutex(srv->mutex);
            LAB_SDL_CondSignal(srv->finished);
            LAB_SDL_LockMutex(srv->mutex);
        }
    }
}



void LAB_GameServer_Tick(LAB_GameServer* srv)
{
    LAB_ASSERT(LAB_IsMainThread());
    LAB_GameServer_ValidateQueues(srv);

    LAB_GameServer_UnloadChunks(srv);

    //LAB_SDL_CondBroadcast(srv->update); // keep alive
    LAB_GameServer_StartTasks(srv);

    LAB_SDL_LockMutex(srv->mutex);
    srv->stats.timestamp++;
    LAB_SDL_UnlockMutex(srv->mutex);
}

void LAB_GameServer_GetStats(LAB_GameServer* srv, LAB_GameServerStats* out)
{
    LAB_SDL_LockMutex(srv->mutex);
    memcpy(out, &srv->stats, sizeof *out);

    srv->stats.noop_spins = 0;
    srv->stats.completed_task_count = 0;
    srv->stats.completed_mainthread = 0;
    srv->stats.requeued_count = 0;
    srv->stats.new_task_count = 0;
    srv->stats.overriden_task_count = 0;
    srv->stats.update_cycles = 0;
    srv->stats.unload_cycles = 0;
    srv->stats.view_update_cycles = 0;
    srv->stats.gen_queue_size = 0;
    memset(srv->stats.update_counts, 0, sizeof srv->stats.update_counts);
    LAB_SDL_UnlockMutex(srv->mutex);
}



LAB_STATIC
void LAB_GameServer_ValidateQueues(LAB_GameServer* srv)
{
    LAB_ASSERT(LAB_IsMainThread());

    // Collect results
    #if !defined NDEBUG && 0
    LAB_SDL_LockMutex(srv->mutex);
    {
        size_t task_count = 0;
        for(int j = 0; j < LAB_CHUNK_PRIORITIES; ++j)
        {
            LAB_ChunkQueue_DbgValidate(&srv->tasks[j]);
            task_count += srv->tasks[j].dbg_size;
        }
        LAB_ASSERT(srv->stats.waiting_task_count == task_count);
    }
    LAB_SDL_UnlockMutex(srv->mutex);
    #endif
}


LAB_STATIC
void LAB_GameServer_StartTasks(LAB_GameServer* srv)
{
    LAB_ASSERT(LAB_IsMainThread());

    srv->stats.max_age = 0;

    LAB_Nanos stoptime = LAB_NanoSeconds() + 1000*1000;
    {
        size_t i0 = srv->update_pointer;
        size_t i = i0;
        LAB_ChunkTBL* tbl = &srv->world->chunks;

        if(i >= tbl->capacity)
        {
            i = 0;
            srv->stats.update_cycles++;
        }

        if(tbl->size)
        for(;;)
        {

            LAB_World_ChunkEntry* e = &tbl->table[i];
            if(LAB_ChunkTBL_IsEntry(tbl, e))
            {
                // Adjust chunk priority
                LAB_Chunk* chunk = e->chunk;
                LAB_SDL_LockMutex(srv->mutex);
                if(chunk->queue_prev)
                {
                    size_t age = srv->stats.timestamp - chunk->queue_timestamp;
                    if(age > srv->stats.max_age) srv->stats.max_age = age;

                    int priority = LAB_ChunkPriority(srv->world, chunk);
                    int cpriority = chunk->update_priority;
                    if(priority != cpriority)
                    {
                        LAB_ChunkQueue_Splice(&srv->tasks[cpriority], chunk);
                        LAB_ChunkQueue_PushBack(&srv->tasks[priority], chunk);
                        chunk->update_priority = priority;
                        srv->stats.requeued_count++;
                    }
                }
                LAB_SDL_UnlockMutex(srv->mutex);

                // Update chunk
                if(LAB_Chunk_Access(e->chunk) && e->chunk->dirty)
                {
                    if(LAB_GameServer_PushChunkTask(srv, e->chunk, LAB_CHUNK_STAGE_LIGHT))
                    {
                        e->chunk->dirty = 0;
                    }
                }
            }

            ++i;
            if(i >= tbl->capacity)
            {
                i = 0;
                srv->stats.update_cycles++;
            }
            if(i == i0) break;
            if(stoptime < LAB_NanoSeconds()) break;
        }
        srv->update_pointer = i;
    }
    stoptime += 1000*1000;
}


LAB_STATIC
void LAB_GameServer_UnloadChunks(LAB_GameServer* srv)
{
    LAB_ASSERT(LAB_IsMainThread());

    LAB_Nanos stoptime = LAB_NanoSeconds() + 1000*1000; // 1000 us

    LAB_World* world = srv->world;

    LAB_ChunkTBL_ShrinkToFit(&world->chunks);

    size_t i = srv->unload_pointer;
    if(i >= world->chunks.capacity)
    {
        i = 0;
        srv->stats.unload_cycles++;
    }
    
    if(world->chunks.size)
    for(; i < world->chunks.capacity; ++i)
    {
        // TODO: remove chunkkeep hook, instead add callback to get
        //       position(s) and distance(s), multiple to be considered, if there are multiple views
        LAB_World_ChunkEntry* entry = &world->chunks.table[i];
        LAB_Chunk* chunk = entry->chunk;
        if(chunk)
        {
            chunk->age++;
            if(chunk->age >= LAB_MAX_CHUNK_AGE && !chunk->modified)
            {
                int cx, cy, cz;
                cx = entry->pos.x;
                cy = entry->pos.y;
                cz = entry->pos.z;
                bool keep = world->view->chunkkeep(world->view_user, world, chunk, cx, cy, cz);

                if(keep)
                {
                    chunk->age = 0;
                }
                else
                {
                    LAB_SDL_LockMutex(srv->mutex);
                    LAB_Chunk* neighbors[27];
                    // TODO: what if new chunks are created in the 3x3x3 region simultaneously
                    // in another thread
                    LAB_GetChunkNeighbors(chunk, neighbors);
                    if(LAB_GameServer_Locked_TryLockChunk(srv, neighbors))
                    {

                        if(chunk->queue_prev)
                        {
                            LAB_ChunkQueue_Splice(&srv->tasks[chunk->update_priority], chunk);
                            srv->stats.waiting_task_count--;
                        }

                        // Only entries after this entry are changed, another entry
                        // might be moved into this entry, the array itself is not
                        // reallocated when removing entries
                        if(chunk->view_user) world->view->chunkunlink(world->view_user, world, chunk, cx, cy, cz);
                        LAB_ChunkTBL_RemoveEntry(&world->chunks, &world->chunks.table[i]);
                        LAB_UnlinkChunk(chunk);
                        LAB_GameServer_Locked_UnlockChunk(srv, neighbors);
                        LAB_DestroyChunk_Unlinked(chunk);
                        --i; // repeat index
                    }
                    LAB_SDL_UnlockMutex(srv->mutex);
                }
            }
            if(stoptime < LAB_NanoSeconds())
                break;
        }
    }
    srv->unload_pointer = i;
}











bool LAB_GameServer_PushChunkTask(LAB_GameServer* srv,
                                   LAB_Chunk* chunk,
                                   unsigned int update)
{
    int priority = LAB_ChunkPriority(srv->world, chunk);

    LAB_ChunkQueue* q = &srv->tasks[priority];

    LAB_SDL_LockMutex(srv->mutex);
    bool success = LAB_ChunkQueue_PushBack(q, chunk);
    if(success)
    {
        chunk->update_stage = update;
        chunk->update_priority = priority;

        chunk->queue_timestamp = srv->stats.timestamp;

        srv->stats.waiting_task_count++;
        srv->stats.new_task_count++;
    }
    else if(update < chunk->update_stage)
    {
        chunk->update_stage = update;

        srv->stats.overriden_task_count++;

        success = true;
    }

    bool same_update = chunk->update_stage == update;
    LAB_SDL_UnlockMutex(srv->mutex);
    if(success) LAB_SDL_CondSignal(srv->update);

    return success || same_update;
}























void LAB_GameServer_Lock(LAB_GameServer* srv)
{
    LAB_ASSERT(LAB_IsMainThread());

    LAB_SDL_LockMutex(srv->mutex);
    srv->pause = true;
    LAB_SDL_CondBroadcast(srv->update);


    while(srv->threads_paused != srv->worker_count)
        LAB_SDL_CondWait(srv->paused, srv->mutex);

    LAB_SDL_UnlockMutex(srv->mutex);
}

bool LAB_GameServer_LockTimeout(LAB_GameServer* srv, LAB_Nanos ns)
{
    LAB_ASSERT(LAB_IsMainThread());

    LAB_SDL_LockMutex(srv->mutex);
    srv->pause = true;
    LAB_SDL_CondBroadcast(srv->update);

    LAB_Nanos stoptime = LAB_NanoSeconds() + ns;

    while(srv->threads_paused != srv->worker_count)
    {
        // TODO: timeout too coarse: wake up main thread from another thread
        LAB_SDL_CondWaitTimeout(srv->paused, srv->mutex, 0/*ms*/);
        if(stoptime < LAB_NanoSeconds())
        {
            srv->pause = false;
            LAB_SDL_UnlockMutex(srv->mutex);
            LAB_SDL_CondBroadcast(srv->update);
            return false;
        }
    }

    LAB_SDL_UnlockMutex(srv->mutex);
    return true;
}

void LAB_GameServer_Unlock(LAB_GameServer* srv)
{
    LAB_ASSERT(LAB_IsMainThread());

    LAB_SDL_LockMutex(srv->mutex);
    LAB_ASSERT(srv->pause);
    srv->pause = false;
    LAB_SDL_UnlockMutex(srv->mutex);
    LAB_SDL_CondBroadcast(srv->update);

}



bool LAB_GameServer_Locked_TryLockChunk(LAB_GameServer* srv, LAB_Chunk* chunks[27])
{
    // TODO check if locked

    int i = 0;
    for(; i < 27; ++i)
    {
        if(!chunks[i]) continue;

        if(i == 1+3+9)
        {
            if(chunks[i]->access_mode != 0) break;
            chunks[i]->access_mode = -1;
        }
        else
        {
            if(chunks[i]->access_mode < 0) break;
            chunks[i]->access_mode++;
        }
    }
    if(i == 27) return true;
    for(--i; i >= 0; --i)
    {
        if(!chunks[i]) continue;

        if(i == 1+3+9)
            chunks[i]->access_mode = 0;
        else
            chunks[i]->access_mode--;
    }
    LAB_SDL_CondSignal(srv->finished);
    return false;
}



void LAB_GameServer_Locked_UnlockChunk(LAB_GameServer* srv, LAB_Chunk* chunks[27])
{
    for(int i = 26; i >= 0; --i)
    {
        if(!chunks[i]) continue;

        if(i == 1+3+9)
            chunks[i]->access_mode = 0;
        else
            chunks[i]->access_mode--;
    }
    LAB_SDL_CondSignal(srv->finished);
}







void LAB_GameServer_Lock1Chunk(LAB_GameServer* srv, LAB_Chunk* chunk)
{
    LAB_ASSERT(LAB_IsMainThread()); // prevent chunk from being destroyed
    LAB_SDL_LockMutex(srv->mutex);

    LAB_Chunk* neighbors0[27];
    LAB_GetChunkNeighborsNone(chunk, neighbors0);

    while(!LAB_GameServer_Locked_TryLockChunk(srv, neighbors0))
        LAB_SDL_CondWait(srv->finished, srv->mutex); // << no destroy run

    LAB_SDL_UnlockMutex(srv->mutex);
}

void LAB_GameServer_Unlock1Chunk(LAB_GameServer* srv, LAB_Chunk* chunk)
{
    LAB_ASSERT(LAB_IsMainThread()); // prevent chunk from being destroyed
    LAB_SDL_LockMutex(srv->mutex);

    LAB_Chunk* neighbors0[27];
    LAB_GetChunkNeighborsNone(chunk, neighbors0);

    LAB_GameServer_Locked_UnlockChunk(srv, neighbors0);

    LAB_SDL_UnlockMutex(srv->mutex);
}