#pragma once

#include <stdbool.h>
#include "LAB_chunk_queue.h"

typedef struct LAB_WorldServer
{
    LAB_World* world;
    LAB_ChunkQueue q;
    SDL_Thread** workers;
    size_t worker_count;
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