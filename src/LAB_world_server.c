#include "LAB_world_server.h"
#include "LAB_memory.h"
#include "LAB_obj.h"
#include "LAB_world.h"
#include "LAB_opt.h"

static int LAB_WorldServer_ThreadRoutine(void* vsrv);

static void LAB_WorldServer_CollectTasks(LAB_WorldServer* srv);
static void LAB_WorldServer_StartTasks(LAB_WorldServer* srv);

#define HTL_PARAM LAB_WORLD_TASK_QUEUE
#include "HTL/mt_queue.t.c"
#undef HTL_PARAM

#define HTL_PARAM LAB_WORLD_RESULT_QUEUE
#include "HTL/mt_queue.t.c"
#undef HTL_PARAM

enum
{
    LAB_CQ_TERMINATE, // Not counted as pending
    LAB_CQ_GENERATE
};


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
    srv->pending_tasks = 0;
    srv->est_task_count = queue_capacity;
    srv->world = world;
    LAB_OBJ(LAB_WorldTaskQueue_Create(&srv->tasks, queue_capacity),
            LAB_WorldTaskQueue_Destroy(&srv->tasks),
    LAB_OBJ(LAB_DbgAtHalt(LAB_SDL_LockMutexV, srv->tasks.mtx_lock),
            LAB_DbgRemoveHalt(LAB_SDL_LockMutexV, srv->tasks.mtx_lock),
    LAB_OBJ(LAB_WorldResultQueue_Create(&srv->results, queue_capacity),
            LAB_WorldResultQueue_Destroy(&srv->results),
    LAB_OBJ(srv->workers = LAB_MallocN(worker_count, sizeof(*srv->workers)),
            LAB_Free(srv->workers),
    {
        srv->worker_count = worker_count;
        size_t i;
        for(i = 0; i < worker_count; ++i)
        {
            SDL_Thread* t;
            t = SDL_CreateThread(LAB_WorldServer_ThreadRoutine, 
                                "LAB_WorldServer_ThreadRoutine", srv);
            if(t == NULL) break;
            srv->workers[i] = t;
        }
        if(i == worker_count)
            return true;


        // Failed initialization
        for(size_t j = 0; j < i; ++j)
        {
            LAB_WorldTask task = { .action = LAB_CQ_TERMINATE };
            LAB_WorldTaskQueue_WaitPushBack(&srv->tasks, &task);
        }

        for(size_t j = 0; j < i; ++j)
        {
            int return_value;
            SDL_WaitThread(srv->workers[j], &return_value);
            (void)return_value;
        }
    }););););
    return false;
}

/**
 *  stop threads and destroy server
 */
void LAB_WorldServer_Destroy(LAB_WorldServer* srv)
{
    for(size_t j = 0; j < srv->worker_count; ++j)
    {
        LAB_WorldTask task = { .action = LAB_CQ_TERMINATE };
        LAB_WorldTaskQueue_WaitPushBack(&srv->tasks, &task);
    }

    for(size_t j = 0; j < srv->worker_count; ++j)
    {
        int return_value;
        SDL_WaitThread(srv->workers[j], &return_value);
        (void)return_value;
    }
    LAB_Free(srv->workers);
    LAB_WorldResultQueue_Destroy(&srv->results);
    LAB_DbgRemoveHalt(LAB_SDL_LockMutexV, srv->tasks.mtx_lock);
    LAB_WorldTaskQueue_Destroy(&srv->tasks);
}


int LAB_WorldServer_ThreadRoutine(void* vsrv)
{
    LAB_WorldServer* srv = vsrv;

    while(true)
    {
        LAB_WorldTask task;
        LAB_WorldTaskQueue_WaitPopFront(&srv->tasks, &task);

        switch(task.action)
        {
            case LAB_CQ_TERMINATE:
            {
                return 0;
            }
            case LAB_CQ_GENERATE:
            {
                //LAB_DbgPrintf("GEN %i %i %i %p\n", task.chunk.pos.x, task.chunk.pos.y, task.chunk.pos.z, task.chunk.chunk);
                LAB_ASSERT(!task.chunk.chunk->generated);

                srv->world->chunkgen(
                    srv->world->chunkgen_user,
                    task.chunk.chunk,
                    task.chunk.pos.x, task.chunk.pos.y, task.chunk.pos.z
                );


                LAB_WorldResult result;
                result.result = 0;
                result.chunk.pos   = task.chunk.pos;
                result.chunk.chunk = task.chunk.chunk;
                LAB_WorldResultQueue_WaitPushBack(&srv->results, &result);
            }
        }
    }
}



void LAB_WorldServer_Tick(LAB_WorldServer* srv)
{
    uint64_t nanos = LAB_NanoSeconds();

    LAB_WorldServer_CollectTasks(srv);

    if(LAB_NanoSeconds()-nanos < 1000*1000) // 1 ms
    {
        srv->est_task_count = srv->est_task_count*33/32+10;
        //printf("More amount\n");
    }
    else
    {
        //printf("Half amount\n");
        srv->est_task_count = srv->est_task_count*10/16-10;
        if(srv->est_task_count < 10)
            srv->est_task_count = 10;
    }

    LAB_WorldServer_StartTasks(srv);
}

void LAB_WorldServer_TickV(void* vsrv)
{
    LAB_WorldServer_Tick((LAB_WorldServer*)vsrv);
}




LAB_STATIC
void LAB_WorldServer_CollectTasks(LAB_WorldServer* srv)
{
    struct 
    {
        LAB_ChunkPos pos;
        LAB_Chunk* chunk;
    }* updates;
    updates = LAB_MallocN(srv->pending_tasks, sizeof(*updates));
    LAB_ASSERT(updates);

    // Collect results
    for(size_t t = 0; t < srv->pending_tasks; ++t)
    {
        LAB_WorldResult result;
        LAB_WorldResultQueue_WaitPopFront(&srv->results, &result);


        LAB_Chunk* chunk = result.chunk.chunk;
        LAB_ASSERT(!chunk->generated);

        chunk->generated = 1;

        //update empty bit
        chunk->empty = 1;
        for(int i = 0; i < 16*16*16; ++i)
        {
            if(chunk->blocks[i] != &LAB_BLOCK_AIR)
            {
                chunk->empty = 0;
                break;
            }
        }


        updates[t].pos   = result.chunk.pos;
        updates[t].chunk = result.chunk.chunk;
    }



    // send updates
    for(size_t t = 0; t < srv->pending_tasks; ++t)
    {
        LAB_ASSERT(updates[t].chunk->generated);
        LAB_ASSERT(!updates[t].chunk->light_generated);
        LAB_UpdateChunk(srv->world, updates[t].chunk, updates[t].pos.x, updates[t].pos.y, updates[t].pos.z, LAB_CHUNK_UPDATE_BLOCK);
    }
    srv->pending_tasks = 0;
    LAB_Free(updates);
}


LAB_STATIC
void LAB_WorldServer_StartTasks(LAB_WorldServer* srv)
{
    while(!LAB_ChunkPosQueue_IsEmpty(&srv->world->gen_queue) 
       && !LAB_WorldTaskQueue_IsFull(&srv->tasks)
       && srv->pending_tasks < srv->est_task_count)
    {
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
            {
                LAB_Chunk_Connect(chunk, face, neighbor->chunk);
            }
        }



        LAB_WorldTask task = {
            .action = LAB_CQ_GENERATE, 
            .chunk = { .chunk = chunk, .pos = *pos }
        };
        LAB_WorldTaskQueue_WaitPushBack(&srv->tasks, &task);
        LAB_ChunkPosQueue_PopFront(&srv->world->gen_queue);
        ++srv->pending_tasks;
    }
}