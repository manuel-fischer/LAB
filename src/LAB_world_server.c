#include "LAB_world_server.h"
#include "LAB_memory.h"
#include "LAB_obj.h"
#include "LAB_world.h"

static int LAB_WorldServer_ChunkHandleThread(void* vsrv);

enum
{
    LAB_CQ_TERMINATE,
    LAB_CQ_GENERATE
};


bool LAB_WorldServer_Create(LAB_WorldServer* srv,
                            LAB_World* world,
                            size_t queue_capacity,
                            size_t worker_count)
{
    srv->world = world;
    LAB_OBJ(LAB_ChunkQueue_Create(&srv->q, queue_capacity),
            LAB_ChunkQueue_Destroy(&srv->q),
    LAB_OBJ(srv->workers = LAB_MallocN(sizeof(*srv->workers), worker_count),
            LAB_Free(srv->workers),
    {
        srv->worker_count = worker_count;
        int i;
        for(i = 0; i < worker_count; ++i)
        {
            SDL_Thread* t;
            t = SDL_CreateThread(LAB_WorldServer_ChunkHandleThread, 
                                "LAB_WorldServer_ChunkHandleThread", srv);
            if(t == NULL) break;
            srv->workers[i] = t;
        }
        if(i == worker_count)
            return true;

        for(int j = 0; j < i; ++j)
        {
            LAB_ChunkQueue_Push(&srv->q, NULL, LAB_CQ_TERMINATE);
        }

        for(int j = 0; j < i; ++j)
        {
            int return_value;
            SDL_WaitThread(srv->workers[j], &return_value);
            (void)return_value;
        }
    }););
    return false;
}

/**
 *  stop threads and destroy server
 */
void LAB_WorldServer_Destroy(LAB_WorldServer* srv)
{
    for(int j = 0; j < srv->worker_count; ++j)
    {
        LAB_ChunkQueue_Push(&srv->q, NULL, LAB_CQ_TERMINATE);
    }

    for(int j = 0; j < srv->worker_count; ++j)
    {
        int return_value;
        SDL_WaitThread(srv->workers[j], &return_value);
        (void)return_value;
    }
}


int LAB_WorldServer_ChunkHandleThread(void* vsrv)
{
    LAB_WorldServer* srv = vsrv;

    LAB_Chunk* cnk;
    int action;
    while(true)
    {
        LAB_ChunkQueue_Pop(&srv->q, &cnk, &action);

        switch(action)
        {
            case LAB_CQ_TERMINATE:
            {
                return 0;
            }
            case LAB_CQ_GENERATE:
            {
                //srv->world->chunkgen
            }
        }
    }
}