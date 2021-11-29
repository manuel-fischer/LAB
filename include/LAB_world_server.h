#pragma once

#include <stdbool.h>
#include "LAB_htl_config.h"
#include "LAB_world.h"

typedef struct LAB_WorldTask
{
    int action;
    union
    {
        struct
        {
            LAB_Chunk* chunk;
            LAB_ChunkPos pos;
        } chunk;
    };
    
} LAB_WorldTask;

#define LAB_WORLD_TASK_QUEUE_TYPE LAB_WorldTask
#define LAB_WORLD_TASK_QUEUE_NAME LAB_WorldTaskQueue

#define HTL_PARAM LAB_WORLD_TASK_QUEUE
#include "HTL/mt_queue.t.h"
#undef HTL_PARAM





typedef struct LAB_WorldResult
{
    int result;
    union
    {
        struct
        {
            LAB_Chunk* chunk;
            LAB_ChunkPos pos;
        } chunk;
    };

} LAB_WorldResult;

#define LAB_WORLD_RESULT_QUEUE_TYPE LAB_WorldResult
#define LAB_WORLD_RESULT_QUEUE_NAME LAB_WorldResultQueue

#define HTL_PARAM LAB_WORLD_RESULT_QUEUE
#include "HTL/mt_queue.t.h"
#undef HTL_PARAM



typedef struct LAB_WorldServer
{
    LAB_World* world;
    LAB_WorldTaskQueue tasks;
    LAB_WorldResultQueue results;
    SDL_Thread** workers;
    size_t worker_count;

    size_t pending_tasks;
    size_t est_task_count;
} LAB_WorldServer;

/**
 *  create server and start threads
 */
bool LAB_WorldServer_Create(LAB_WorldServer* srv,
                            LAB_World* world,
                            size_t queue_capacity,
                            size_t worker_count);

/**
 *  stop threads and destroy server
 */
void LAB_WorldServer_Destroy(LAB_WorldServer* srv);

void LAB_WorldServer_Tick(LAB_WorldServer* srv);
void LAB_WorldServer_TickV(void* vsrv); 