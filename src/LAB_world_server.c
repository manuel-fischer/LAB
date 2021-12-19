#include "LAB_world_server.h"
#include "LAB_memory.h"
#include "LAB_obj.h"
#include "LAB_world.h"
#include "LAB_world_light.h"
#include "LAB_opt.h"

LAB_STATIC int LAB_WorldServer_ThreadRoutine(void* vsrv);

LAB_STATIC void LAB_WorldServer_RunMainthreadTasks(LAB_WorldServer* srv, LAB_Nanos stop);
LAB_STATIC void LAB_WorldServer_StartTasks(LAB_WorldServer* srv);
LAB_STATIC void LAB_WorldServer_UnloadChunks(LAB_WorldServer* srv);

LAB_STATIC void LAB_WorldServer_SendStop(LAB_WorldServer* srv);

#define HTL_PARAM LAB_WORLD_TASK_QUEUE
#include "HTL/queue.t.c"
#undef HTL_PARAM

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
void LAB_WorldServer_ChunkGenerate_CB(LAB_WorldServer* srv,
                                      LAB_Chunk* chunk, LAB_ChunkPos pos,
                                      void* uparam);

LAB_STATIC
void LAB_WorldServer_ChunkUpdateLight_CB(LAB_WorldServer* srv,
                                         LAB_Chunk* chunk, LAB_ChunkPos pos,
                                         void* uparam);


#define LAB_VIEW_NOTIFY_PARITY LAB_PARITY_MAIN
LAB_STATIC
void LAB_WorldServer_ChunkUpdated_CB(LAB_WorldServer* srv,
                                     LAB_Chunk* chunk, LAB_ChunkPos pos,
                                     void* uparam);

LAB_STATIC
void LAB_WorldServer_ChunkViewMesh_CB(LAB_WorldServer* srv,
                                      LAB_Chunk* chunk, LAB_ChunkPos pos,
                                      void* uparam);


LAB_STATIC
LAB_ChunkCallback LAB_ChunkStageCallback(int update)
{
    switch(update)
    {
        case LAB_CHUNK_STAGE_GENERATE:    return &LAB_WorldServer_ChunkGenerate_CB;
        case LAB_CHUNK_STAGE_LIGHT:       return &LAB_WorldServer_ChunkUpdateLight_CB;
        case LAB_CHUNK_STAGE_VIEW_NOTIFY: return &LAB_WorldServer_ChunkUpdated_CB;
        case LAB_CHUNK_STAGE_VIEW_MESH:   return &LAB_WorldServer_ChunkViewMesh_CB;
        default:                          LAB_ASSERT_FALSE("Invalid chunk stage");
    }
}



LAB_STATIC
int LAB_UpdateParity_From(LAB_ChunkPos p)
{
    return (p.x&1) | (p.y&1) << 1 | (p.z&1) << 2;
}




LAB_STATIC
void LAB_SDL_LockMutexV(void* lock)
{
    SDL_LockMutex(lock);
    LAB_DbgPrintf("Locking %p\n", lock);
}

bool LAB_WorldServer_Create(LAB_WorldServer* srv,
                            LAB_World* world,
                            size_t queue_capacity,
                            size_t worker_count)
{
    srv->world = world;
    srv->worker_count = worker_count;

    srv->parity = LAB_PARITY_000;
    srv->threads_parity = worker_count;
    srv->rest_parity = LAB_PARITY_TASK_COUNT;
    srv->stop = false;
    srv->pause = false;
    srv->threads_paused = 0;
    srv->update_pointer = 0;

    srv->completed_cycles = 0;
    srv->completed_cycles_thrd = 0;
    srv->completed_mainthread = 0;

    srv->task_count = 0;

    for(size_t i = 0; i < LAB_PARITY_COUNT; ++i)
    for(size_t j = 0; j < LAB_CHUNK_PRIORITIES; ++j)
        LAB_ChunkQueue_Create(&srv->tasks[i][j]);

    /*LAB_OBJ_FOR(p, LAB_PARITY_COUNT*LAB_CHUNK_PRIORITIES,
                LAB_ChunkQueue_Create(&(*srv->tasks)[p]),
                LAB_ChunkQueue_Destroy(&(*srv->tasks)[p]),*/
    LAB_OBJ_SDL(srv->mutex = SDL_CreateMutex(),
                SDL_DestroyMutex(srv->mutex),
    LAB_OBJ(LAB_DbgAtHalt(LAB_SDL_LockMutexV, srv->mutex),
            LAB_DbgRemoveHalt(LAB_SDL_LockMutexV, srv->mutex),
    LAB_OBJ_SDL(srv->update = SDL_CreateCond(),
                SDL_DestroyCond(srv->update),
    LAB_OBJ_SDL(srv->paused = SDL_CreateCond(),
                SDL_DestroyCond(srv->paused),
    LAB_OBJ(srv->workers = LAB_MallocN(worker_count, sizeof(*srv->workers)),
            LAB_Free(srv->workers),
    LAB_OBJ_FOR2(i, worker_count,
                 srv->workers[i] = SDL_CreateThread(LAB_WorldServer_ThreadRoutine,
                                                   "LAB_WorldServer_ThreadRoutine", srv),
                 LAB_WorldServer_SendStop(srv),
                 SDL_WaitThread(srv->workers[i], NULL),
                 
    {
            return true;
    }););););););
    return false;
}

/**
 *  stop threads and destroy server
 */
void LAB_WorldServer_Destroy(LAB_WorldServer* srv)
{
    LAB_WorldServer_SendStop(srv);

    for(size_t j = 0; j < srv->worker_count; ++j)
        SDL_WaitThread(srv->workers[j], NULL);

    LAB_Free(srv->workers);
    LAB_DbgRemoveHalt(LAB_SDL_LockMutexV, srv->mutex);

    SDL_DestroyCond(srv->paused);
    SDL_DestroyCond(srv->update);
    SDL_DestroyMutex(srv->mutex);

    //for(size_t i = 0; i < LAB_PARITY_COUNT; ++i)
    //for(size_t j = 0; j < LAB_CHUNK_PRIORITIES; ++j)
    //    LAB_ChunkQueue_Destroy(&srv->tasks[i][j]);
}


void LAB_WorldServer_SendStop(LAB_WorldServer* srv)
{
    SDL_LockMutex(srv->mutex);
    srv->stop = true;
    SDL_UnlockMutex(srv->mutex);
    SDL_CondBroadcast(srv->update);
}


int LAB_WorldServer_ThreadRoutine(void* vsrv)
{
    LAB_WorldServer* srv = vsrv;

    LAB_UpdateParity own_parity = srv->parity;
    bool has_finished_parity = false;

    SDL_LockMutex(srv->mutex);
    while(true)
    {

        if(srv->pause)
        {
            srv->threads_paused++;
            SDL_CondSignal(srv->paused);
            while(srv->pause && !srv->stop)
                SDL_CondWait(srv->update, srv->mutex);

            srv->threads_paused--;
        }

        if(srv->stop)
        {
            SDL_UnlockMutex(srv->mutex);
            return 0;
        }

        LAB_ChunkQueue* qs;


        // switch to new parity
        if(has_finished_parity && srv->parity != own_parity)
        {
            own_parity = srv->parity;
            has_finished_parity = false;
        }

        qs = srv->tasks[own_parity];

        // check if parity has more tasks
        if(!has_finished_parity)
        {
            if(LAB_ChunkQueues_AllEmpty(qs)) srv->rest_parity = 0;

            if(!srv->rest_parity)
            {
                --srv->threads_parity;

                if(srv->threads_parity == 0)
                {
                    // No thread works on the current parity
                    // go on to the next parity

                    srv->threads_parity = srv->worker_count;
                    srv->rest_parity = LAB_PARITY_TASK_COUNT;

                    // Next parity
                    srv->parity++;
                    if(srv->parity&8) srv->completed_cycles_thrd++;
                    srv->parity&=7;

                    own_parity = srv->parity;
                    // keep has_finished_parity = false;                

                    SDL_CondBroadcast(srv->update);

                    continue;
                }
                else
                {
                    has_finished_parity = true;
                }
            }
        }


        if(has_finished_parity)
        {
            qs = srv->tasks[LAB_PARITY_NONE];
        }


        if(LAB_ChunkQueues_AllEmpty(qs))
        {
            SDL_CondWait(srv->update, srv->mutex);
            //SDL_UnlockMutex(srv->mutex);
            //SDL_LockMutex(srv->mutex);
            continue;
        }
        
        
        int priority = LAB_ChunkQueues_NextQueue(qs);
        LAB_Chunk* chunk = LAB_ChunkQueue_PopFront(&qs[priority]);
        //LAB_DbgPrintf("Dequeued\n");

        if(chunk->is_accessed)
        {
            // because we popped from the stack
            // at least the space for one element is available
            // such that this is safe
            LAB_ChunkQueue_PushBack(&qs[priority], chunk);
            //LAB_DbgPrintf("Reenqued\n");
            SDL_UnlockMutex(srv->mutex);
            // TODO wait
            SDL_LockMutex(srv->mutex);
            continue;
        }
        int update_stage = chunk->update_stage;
        chunk->update_stage = 0;
        chunk->is_accessed = true;
        SDL_UnlockMutex(srv->mutex);

        LAB_ChunkCallback cb = LAB_ChunkStageCallback(update_stage);
        cb(srv, chunk, chunk->pos, NULL);

        SDL_LockMutex(srv->mutex);
        //LAB_DbgPrintf("Finished\n");
        
        //task.chunk->enqueue_count--;

        chunk->is_accessed = false;

        srv->task_count--;
    }
}



void LAB_WorldServer_Tick(LAB_WorldServer* srv)
{
    uint64_t stoptime = LAB_NanoSeconds() + 1000*1000;

    if(LAB_WorldServer_LockTimeout(srv, 1000*1000))
    {
        LAB_WorldServer_RunMainthreadTasks(srv, stoptime);
        LAB_WorldServer_UnloadChunks(srv);

        LAB_WorldServer_Unlock(srv);
    }

    LAB_WorldServer_StartTasks(srv);

    SDL_LockMutex(srv->mutex);
    srv->completed_cycles = srv->completed_cycles_thrd;
    srv->completed_cycles_thrd = 0;
    SDL_UnlockMutex(srv->mutex);
    srv->timestamp++;
}

void LAB_WorldServer_TickV(void* vsrv)
{
    LAB_WorldServer_Tick((LAB_WorldServer*)vsrv);
}




LAB_STATIC
void LAB_WorldServer_RunMainthreadTasks(LAB_WorldServer* srv, LAB_Nanos stop)
{
    // Collect results
    #ifndef NDEBUG
    {
        size_t task_count = 0;
        for(int i = 0; i < LAB_PARITY_COUNT; ++i)
        for(int j = 0; j < LAB_CHUNK_PRIORITIES; ++j)
        {
            LAB_ChunkQueue_DbgValidate(&srv->tasks[i][j]);
            task_count += srv->tasks[i][j].dbg_size;
        }
        LAB_ASSERT(srv->task_count == task_count);
    }
    #endif

    srv->completed_mainthread = 0;
    while(true)
    {
        LAB_ChunkQueue* qs = srv->tasks[LAB_PARITY_MAIN];

        if(LAB_ChunkQueues_AllEmpty(qs) || LAB_NanoSeconds() >= stop)
        {
            return;
        }
        //LAB_DbgPrintf("SPIN\n");
        
        LAB_Chunk* chunk = LAB_ChunkQueues_PopFront(qs);
        int update_stage = chunk->update_stage;
        chunk->update_stage = 0;
        LAB_ASSERT(!chunk->is_accessed);

        //SDL_UnlockMutex(srv->mutex);

        LAB_ChunkCallback cb = LAB_ChunkStageCallback(update_stage);
        cb(srv, chunk, chunk->pos, NULL);

        //SDL_LockMutex(srv->mutex);

        LAB_ASSERT(!chunk->is_accessed);
        srv->completed_mainthread++;
        srv->task_count--;
    }
}


LAB_STATIC
void LAB_WorldServer_StartTasks(LAB_WorldServer* srv)
{
    srv->max_age = 0;

    LAB_Nanos stoptime = LAB_NanoSeconds() + 1000*1000;
    {
        size_t i0 = srv->update_pointer;
        size_t i = i0;
        LAB_ChunkTBL* tbl = &srv->world->chunks;
        //for(size_t i = 0; i < tbl->capacity; ++i)
        if(tbl->size)
        for(;;)
        {

            LAB_World_ChunkEntry* e = &tbl->table[i];
            if(LAB_ChunkTBL_IsEntry(tbl, e))
            {
                // Adjust chunk priority
                LAB_Chunk* chunk = e->chunk;
                SDL_LockMutex(srv->mutex);
                if(chunk->queue_prev)
                {
                    size_t age = srv->timestamp - chunk->queue_timestamp;
                    if(age > srv->max_age) srv->max_age = age;

                    int parity = chunk->update_parity;
                    if(parity < 8)
                    {
                        int priority = LAB_ChunkPriority(srv->world, chunk);
                        int cpriority = chunk->update_priority;
                        if(priority < cpriority || priority > cpriority)
                        {
                            LAB_ChunkQueue_Splice(&srv->tasks[parity][cpriority], chunk);
                            LAB_ChunkQueue_PushBack(&srv->tasks[parity][priority], chunk);
                            chunk->update_priority = priority;
                        }
                    }
                }
                SDL_UnlockMutex(srv->mutex);

                // Update chunk
                if(LAB_Chunk_Access(e->chunk) && e->chunk->dirty)
                {
                    if(LAB_WorldServer_PushChunkTask(srv, LAB_PARITY_FROM_POS, e->chunk, LAB_CHUNK_STAGE_LIGHT))
                    {
                        e->chunk->dirty = 0;
                    }
                }

                //
            }

            ++i;
            if(i >= tbl->capacity) i = 0;
            if(i == i0) break;
            if(stoptime < LAB_NanoSeconds()) break;
        }
        srv->update_pointer = i;
    }
    stoptime += 1000*1000;

    int rest = 20;
    while(!LAB_ChunkPosQueue_IsEmpty(&srv->world->gen_queue))
    {
        if(!--rest) break;
        LAB_ChunkPos* pos;
        pos = LAB_ChunkPosQueue_Front(&srv->world->gen_queue);

        LAB_World_ChunkEntry* entry;
        entry = LAB_ChunkTBL_Get(&srv->world->chunks, *pos);

        LAB_ASSERT(entry != NULL);
        LAB_Chunk* chunk = entry->chunk;
        
        LAB_ASSERT(chunk);
        LAB_ASSERT(!chunk->generated);

        int x = pos->x, y = pos->y, z = pos->z;
        // connect neighbors
        for(int face = 0; face < 6; ++face)
        {
            LAB_ChunkPos pos2 = { x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face) };
            LAB_World_ChunkEntry* neighbor = LAB_ChunkTBL_Get(&srv->world->chunks, pos2);
            if(neighbor)
                LAB_Chunk_Connect(chunk, face, neighbor->chunk);
        }


        if(LAB_WorldServer_PushChunkTask(srv, LAB_PARITY_NONE, chunk, LAB_CHUNK_STAGE_GENERATE))
        {
            LAB_ChunkPosQueue_PopFront(&srv->world->gen_queue);
        }
        else
        {
            LAB_ASSERT_FALSE("Chunk already enqueued");
        }

        if(stoptime < LAB_NanoSeconds())
            break;
    }
}


LAB_STATIC
void LAB_WorldServer_UnloadChunks(LAB_WorldServer* srv)
{
    LAB_Nanos stoptime = LAB_NanoSeconds() + 100*1000; // 100 us

    //LAB_WorldServer_Lock(srv);
    LAB_World* world = srv->world;


    for(size_t i = 0; i < world->chunks.capacity; ++i)
    {
        // TODO: remove chunkkeep hook, instead add callback to get
        //       position(s) and distance(s), multiple to be considered, if there are multiple views
        LAB_World_ChunkEntry* entry = &world->chunks.table[i];
        LAB_Chunk* chunk = entry->chunk;
        if(LAB_Chunk_Access(chunk))
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
                    if(chunk->queue_prev)
                    {
                        LAB_ChunkQueue_Splice(&srv->tasks[chunk->update_parity][chunk->update_priority], chunk);
                        srv->task_count--;
                    }

                    // Only entries after this entry are changed, another entry
                    // might be moved into this entry, the array itself is not
                    // reallocated when removing entries
                    //printf("Unload chunk %i %i %i\n", cx, cy, cz);
                    if(chunk->view_user) world->view->chunkunlink(world->view_user, world, chunk, cx, cy, cz);
                    LAB_DestroyChunk(chunk);
                    LAB_ChunkTBL_RemoveEntry(&world->chunks, &world->chunks.table[i]);
                    --i; // repeat index
                }
            }
            if(stoptime < LAB_NanoSeconds())
                break;
        }
    }
}











bool LAB_WorldServer_PushChunkTask(LAB_WorldServer* srv,
                                   int /*LAB_UpdateParity*/ parity,
                                   LAB_Chunk* chunk,
                                   unsigned int update)
{

    if(parity == LAB_PARITY_FROM_POS) parity = LAB_UpdateParity_From(chunk->pos);
    
    int priority = LAB_ChunkPriority(srv->world, chunk);

    LAB_ChunkQueue* q = &srv->tasks[parity][priority];

    SDL_LockMutex(srv->mutex);
    bool success = LAB_ChunkQueue_PushBack(q, chunk);
    if(success)
    {
        chunk->update_stage = update;
        chunk->update_parity = parity;
        chunk->update_priority = priority;

        chunk->queue_timestamp = srv->timestamp;

        srv->task_count++;
    }
    else if(update < chunk->update_stage)
    {
        LAB_ASSERT(chunk->generated);

        if(parity != chunk->update_parity)
        {
            LAB_ChunkQueue_Splice(&srv->tasks[chunk->update_parity][chunk->update_priority], chunk);
            LAB_ChunkQueue_PushBack(&srv->tasks[parity][priority], chunk);
            chunk->update_parity = parity;
            chunk->update_priority = priority;
        }

        chunk->update_stage = update;

        success = true;
    }
    bool same_update = chunk->update_stage == update;
    //if(!success && !same_update) LAB_DbgPrintf("Already enqueued: cur:%i new:%i\n", chunk->update_stage, update);
    SDL_UnlockMutex(srv->mutex);
    if(success && parity != LAB_PARITY_MAIN) SDL_CondSignal(srv->update);

    //LAB_ASSERT(success);
    //if(!success) LAB_DbgPrintf("Already enqueued: cur:%i new:%i\n", chunk->update_stage, update);
    //else ;//LAB_DbgPrintf("Successfully enqueued\n");
    return success || same_update;
}









LAB_STATIC
void LAB_WorldServer_ChunkGenerate_CB(LAB_WorldServer* srv,
                                      LAB_Chunk* chunk, LAB_ChunkPos pos,
                                      void* uparam)
{
    //LAB_DbgPrintf("GEN %i %i %i %p\n", task.chunk.pos.x, task.chunk.pos.y, task.chunk.pos.z, task.chunk.chunk);
    LAB_ASSERT(!chunk->generated);

    srv->world->chunkgen(
        srv->world->chunkgen_user,
        chunk,
        pos.x, pos.y, pos.z
    );


    //update empty bit
    chunk->empty = true;
    for(int i = 0; i < 16*16*16; ++i)
    {
        if(chunk->blocks[i] != &LAB_BLOCK_AIR)
        {
            chunk->empty = false;
            break;
        }
    }

    


    atomic_store_explicit(&chunk->generated, true, memory_order_release);

    if(!LAB_WorldServer_PushChunkTask(srv, LAB_PARITY_FROM_POS, chunk, LAB_CHUNK_STAGE_LIGHT))
        LAB_DbgPrintf("Generate->Light: Chunk queue full\n");



    /*LAB_Chunk* chunks[27];
    LAB_GetChunkNeighbors(chunk, chunks);

    int i = 0;
    for(int zz = -1; zz < 2; ++zz)
    for(int yy = -1; yy < 2; ++yy)
    for(int xx = -1; xx < 2; ++xx, ++i)
    {
        if(i != 1+3+9 &&
            chunks[i] && 
            chunks[i]->light_generated
        )
        {
            if(!LAB_WorldServer_PushChunkTask(srv, LAB_MESH_PARITY, chunks[i], LAB_CHUNK_STAGE_VIEW_NOTIFY))
                ; //LAB_DbgPrintf("Light->Light: Chunk queue full\n");
        }
    }*/
}



LAB_STATIC
void LAB_WorldServer_ChunkUpdateLight_CB(LAB_WorldServer* srv,
                                         LAB_Chunk* chunk, LAB_ChunkPos pos,
                                         void* uparam)
{
    LAB_ASSERT(LAB_Chunk_Access(chunk));

    LAB_Chunk* chunks[27];
    LAB_GetChunkNeighbors(chunk, chunks);

    LAB_CCPS dirty_blocks = atomic_exchange(&chunk->dirty_blocks, 0);
    chunk->relit_blocks = 0;

    for(int i = 0; i < 27; ++i)
        if((i != 1+3+9 && chunks[i] && !chunks[i]->light_generated))
            chunks[i] = NULL;

    LAB_TickLight(srv->world, chunks, pos.x, pos.y, pos.z);

    LAB_ASSERT(chunk->light_generated);

    LAB_CCPS relit_blocks = atomic_exchange(&chunk->relit_blocks, 0);
    //if(!relit_blocks) LAB_DbgPrintf("NOTHING RELIT\n");
    //if(!dirty_blocks) LAB_DbgPrintf("NOTHING DIRTY\n");

    int dirty_neighbors = atomic_exchange(&chunk->dirty_neighbors, 0);
    int relit_neighbors = atomic_exchange(&chunk->relit_neighbors, 0);

    //chunk->dirty = 0;

    int dirty_bits27 = LAB_CCPS_Neighborhood(dirty_blocks);
    int relit_bits27 = LAB_CCPS_Neighborhood(relit_blocks);
    int bits27 = dirty_bits27 | relit_bits27;
    
    LAB_ASSERT(!!dirty_bits27 == !!dirty_blocks);
    LAB_ASSERT(!!relit_bits27 == !!relit_blocks);

    //if(bits27 & 1 << (1+3+9))
    if(dirty_blocks || relit_blocks || dirty_neighbors || relit_neighbors)
    {
        if(!LAB_WorldServer_PushChunkTask(srv, LAB_VIEW_NOTIFY_PARITY, chunk, LAB_CHUNK_STAGE_VIEW_NOTIFY))
        { /*LAB_DbgPrintf("Light->Mesh: Chunk queue full\n");*/ }
    }

    //bits27 &= 1<<(1+3+9) | 1<<(  3+9) | 1<<(1 + 9) | 1<<(1+3   )
    //                     | 1<<(2+3+9) | 1<<(1+6+9) | 1<<(1+3+18);

    int i = 0;
    for(int zz = -1; zz < 2; ++zz)
    for(int yy = -1; yy < 2; ++yy)
    for(int xx = -1; xx < 2; ++xx, ++i)
    {
        if(i != 1+3+9 &&
            chunks[i] && 
            chunks[i]->light_generated)
        {
            if(bits27 & 1<<i)
            {
                if(dirty_bits27 & 1<<i) chunks[i]->dirty_neighbors |= 1<<(26-i);
                if(relit_bits27 & 1<<i) chunks[i]->relit_neighbors |= 1<<(26-i);


                if(!LAB_WorldServer_PushChunkTask(srv, LAB_PARITY_FROM_POS, chunks[i], LAB_CHUNK_STAGE_LIGHT))
                { /* LAB_DbgPrintf("Light->Light: Chunk queue full\n"); */ }
            }
            //else
            //{
            //    if(!LAB_WorldServer_PushChunkTask(srv, LAB_MESH_PARITY, chunks[i], LAB_CHUNK_STAGE_VIEW_NOTIFY))
            //    { /* LAB_DbgPrintf("Light->Light: Chunk queue full\n"); */ }
            //}
        }
    }
}



LAB_STATIC
void LAB_WorldServer_ChunkUpdated_CB(LAB_WorldServer* srv,
                                     LAB_Chunk* chunk, LAB_ChunkPos pos,
                                     void* uparam)
{
    LAB_ASSERT(chunk->generated);
    LAB_ASSERT(chunk->light_generated);

    (*srv->world->view->chunkview)(srv->world->view_user, srv->world, chunk, pos.x, pos.y, pos.z, LAB_CHUNK_UPDATE_BLOCK);
}

LAB_STATIC
void LAB_WorldServer_ChunkViewMesh_CB(LAB_WorldServer* srv,
                                      LAB_Chunk* chunk, LAB_ChunkPos pos,
                                      void* uparam)
{
    LAB_ASSERT(chunk->generated);
    LAB_ASSERT(chunk->light_generated);

    (*srv->world->view->chunkmesh)(srv->world->view_user, srv->world, chunk);
}



void LAB_WorldServer_Lock(LAB_WorldServer* srv)
{
    SDL_LockMutex(srv->mutex);
    srv->pause = true;
    SDL_CondBroadcast(srv->update);


    while(srv->threads_paused != srv->worker_count)
        SDL_CondWait(srv->paused, srv->mutex);

    SDL_UnlockMutex(srv->mutex);
}

bool LAB_WorldServer_LockTimeout(LAB_WorldServer* srv, LAB_Nanos ns)
{
    SDL_LockMutex(srv->mutex);
    srv->pause = true;
    SDL_CondBroadcast(srv->update);

    LAB_Nanos stoptime = LAB_NanoSeconds() + ns;

    while(srv->threads_paused != srv->worker_count)
    {
        // TODO: timeout to coarse: wake up main thread from another thread
        SDL_CondWaitTimeout(srv->paused, srv->mutex, 1/*ms*/);
        if(stoptime < LAB_NanoSeconds())
        {
            srv->pause = false;
            SDL_UnlockMutex(srv->mutex);
            SDL_CondBroadcast(srv->update);
            return false;
        }
    }

    SDL_UnlockMutex(srv->mutex);
    return true;
}

void LAB_WorldServer_Unlock(LAB_WorldServer* srv)
{
    SDL_LockMutex(srv->mutex);
    LAB_ASSERT(srv->pause);
    srv->pause = false;
    SDL_UnlockMutex(srv->mutex);
    SDL_CondBroadcast(srv->update);

}