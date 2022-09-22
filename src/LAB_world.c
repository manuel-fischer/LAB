#include "LAB_world.h"

#include "LAB_memory.h"
#include "LAB_error.h"
#include "LAB_math.h"

#include "LAB_stdinc.h"
#include "LAB_attr.h"
#include "LAB_opt.h"
#include "LAB_debug.h"
#include "LAB_util.h"
#include "LAB_bits.h"
#include "LAB_world_light.h"
#include "LAB_vec_algo.h"

#include "LAB_obj.h"

#include "LAB_game_server.h"

#include <math.h>
#include <stdio.h> // DBG



int LAB_World_Create(LAB_World* world)
{
    LAB_OBJ(LAB_ChunkTBL_Create(&world->chunks),
            LAB_ChunkTBL_Destroy(&world->chunks),

    //LAB_OBJ(LAB_ChunkBufQueue_Create(&world->gen_queue, LAB_MAX_LOAD_CHUNK),
    //        LAB_ChunkBufQueue_Destroy(&world->gen_queue),
            
    return 1;
    );//);
    return 0;
}

void LAB_World_Destroy(LAB_World* world)
{
    //LAB_ChunkBufQueue_Destroy(&world->gen_queue);
    for(size_t i = 0; i < world->chunks.capacity; ++i)
    {
        LAB_World_ChunkEntry* entry = &world->chunks.table[i];
        if(entry->chunk != NULL)
        {
            //if(entry->chunk->view_user && world->chunkunlink)
            //    world->chunkunlink(world->chunkunlink_user, world, entry->chunk, entry->pos.x, entry->pos.y, entry->pos.z);
            LAB_DestroyChunk(entry->chunk);
        }
    }
    LAB_ChunkTBL_Destroy(&world->chunks);
}


#if 0
LAB_STATIC LAB_Chunk* LAB_GenerateNotifyChunk(LAB_World* world, int x, int y, int z)
{
    LAB_ASSERT(false);

    //printf("GEN %3i, %3i, %3i\n", x, y, z);
    LAB_ChunkPos pos = { x, y, z };
    LAB_World_ChunkEntry* entry;
    LAB_Chunk* chunk;
    entry = LAB_ChunkTBL_PutAlloc(&world->chunks, pos);

    // Create slot for chunk
    // Note that it does not really exist, because the value is NULL
    // Entry must be written before anything further happens
    if(LAB_UNLIKELY(entry == NULL)) return NULL;

    if(entry->chunk != NULL) // already generated
    {
        return entry->chunk;
    }

    chunk = LAB_CreateChunk();
    if(LAB_UNLIKELY(chunk == NULL))
    {
        // Because the inserted entry was not changed
        // We can discard it
        LAB_ChunkTBL_Discard(&world->chunks, entry);
        return NULL;
    }
    LAB_Chunk* chunk2 = (*world->chunkgen)(world->chunkgen_user, chunk, x, y, z);
    LAB_ASSERT(chunk == chunk2);

    // Slot gets occupied, because >chunk< is nonzero
    entry->pos = pos;
    entry->chunk = chunk;

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

    // connect neighbors
    for(int face = 0; face < 6; ++face)
    {
        LAB_ChunkPos pos2 = { x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face) };
        LAB_World_ChunkEntry* neighbor = LAB_ChunkTBL_Get(&world->chunks, pos2);
        if(neighbor)
        {
            LAB_Chunk_Connect(chunk, face, neighbor->chunk);
        }
    }

    LAB_UpdateChunk(world, chunk, x, y, z, LAB_CHUNK_UPDATE_BLOCK);

    return chunk;
}
#endif


LAB_STATIC
LAB_Chunk* LAB_GetChunk_Incomplete(LAB_World* world, int x, int y, int z)
{
    LAB_ChunkPos pos = { x, y, z };

    LAB_World_ChunkEntry* entry = LAB_ChunkTBL_Get(&world->chunks, pos);
    if(entry == NULL)
        return NULL;
        
    entry->chunk->age = 0;
    return entry->chunk;
}


LAB_Chunk* LAB_GetChunk(LAB_World* world, int x, int y, int z)
{
    return LAB_Chunk_Access(LAB_GetChunk_Incomplete(world, x, y, z));
}

LAB_Chunk* LAB_GenerateChunk(LAB_World* world, int x, int y, int z)
{
    LAB_ChunkPos pos = { x, y, z };
    LAB_World_ChunkEntry* entry = LAB_ChunkTBL_PutAlloc(&world->chunks, pos);
    if(entry == NULL) // TODO Out of memory
        return NULL;

    if(entry->chunk) // chunk generated or generating
        return entry->chunk; //LAB_Chunk_Access(entry->chunk);

    LAB_Chunk* chunk = LAB_CreateChunk(pos);
    if(chunk == NULL) // TODO Out of memory
    {
        LAB_ChunkTBL_Discard(&world->chunks, entry);
        return NULL;
    }

    /*LAB_Chunk** request;
    request = LAB_ChunkBufQueue_PushBack(&world->gen_queue);
    if(request != NULL) {
        *request = chunk;
        
        entry->pos = pos;
        entry->chunk = chunk;
    }
    else
    {
        LAB_DestroyChunk(chunk);
        LAB_ChunkTBL_Discard(&world->chunks, entry);
        entry = NULL;
    }*/

    // connect neighbors
    for(int face = 0; face < 6; ++face)
    {
        LAB_ChunkPos pos2 = { x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face) };
        LAB_World_ChunkEntry* neighbor = LAB_ChunkTBL_Get(&world->chunks, pos2);
        if(neighbor)
            LAB_Chunk_Connect(chunk, face, neighbor->chunk);
    }
    entry->pos = pos;
    entry->chunk = chunk;


    LAB_GameServer_PushChunkTask(world->server, chunk, LAB_CHUNK_STAGE_GENERATE);

    return entry->chunk;
    //return NULL;
}



/*void LAB_UpdateChunk(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update)
{
    LAB_ASSERT(LAB_Chunk_Access(chunk));

    // TODO when block was placed at chunk border
    LAB_Chunk* chunks[27];
    LAB_GetChunkNeighbors(chunk, chunks);

    LAB_CCPS dirty_blocks = chunk->dirty_blocks;


    LAB_Chunk* chunks_light[27];
    for(int i = 0; i < 27; ++i)
        chunks_light[i] = (i != 1+3+9 && chunks[i] && !chunks[i]->light_generated) ? NULL : chunks[i];

    LAB_TickLight(world, chunks_light, x, y, z);


    int bits27 = LAB_CCPS_Neighborhood(dirty_blocks|chunk->relit_blocks);


    int i = 0;
    for(int zz = -1; zz < 2; ++zz)
    for(int yy = -1; yy < 2; ++yy)
    for(int xx = -1; xx < 2; ++xx, ++i)
    {
        if(chunks[i] && (chunks[i]->relit_blocks || (bits27 & 1<<i)))
        {
            if(LAB_LIKELY(world->view != NULL)) 
                (*world->view->chunkview)(world->view_user, world, chunks[i], x+xx, y+yy, z+zz, update);

            chunks[i]->dirty_blocks |= chunks[i]->relit_blocks;
            chunks[i]->relit_blocks = 0;
        }
    }
}*/


/*void LAB_UpdateChunkLater(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update)
{
    chunk->dirty |= update;
}*/


LAB_BlockID LAB_GetBlock(LAB_World* world, int x, int y, int z)
{
    int cx, cy, cz;
    cx = x>>LAB_CHUNK_SHIFT; cy = y>>LAB_CHUNK_SHIFT; cz = z>>LAB_CHUNK_SHIFT;

    LAB_Chunk* chunk = LAB_GetChunk(world, cx, cy, cz);
    if(chunk == NULL) return LAB_BID_OUTSIDE;
    if(chunk->buf_blocks == NULL) return LAB_BID_AIR;
    return chunk->buf_blocks->blocks[LAB_CHUNK_OFFSET(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK)];
}

bool LAB_SetBlock(LAB_World* world, int x, int y, int z, LAB_BlockID block)
{
    // TODO: fix multithreading
    int cx, cy, cz;
    cx = x>>LAB_CHUNK_SHIFT; cy = y>>LAB_CHUNK_SHIFT; cz = z>>LAB_CHUNK_SHIFT;

    LAB_Chunk* chunk = LAB_GetChunk(world, cx, cy, cz);
    if(chunk == NULL) return false;

    LAB_Chunk_Blocks* blocks = LAB_Chunk_Blocks_Write(chunk);
    if(!blocks) return false;

    blocks->blocks[LAB_CHUNK_OFFSET(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK)] = block;
    //LAB_NotifyChunkLater(world, cx, cy, cz);
    chunk->modified = 1;
    chunk->dirty_blocks |= LAB_CCPS_Pos(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK);
    //LAB_UpdateChunkLater(world, chunk, cx, cy, cz, LAB_CHUNK_UPDATE_BLOCK);
    chunk->dirty = true; // TODO remove

    LAB_GameServer_PushChunkTask(world->server, chunk, LAB_CHUNK_STAGE_LIGHT);

    return true;
}

bool LAB_FillBlocks(LAB_World* world, int x0, int y0, int z0, int x1, int y1, int z1, LAB_BlockID block)
{
    // TODO optimize without repeeking chunks
    //      and effectively change chunk->dirty_blocks
    bool success = true;
    for(int z = z0; z < z1; ++z)
    for(int y = y0; y < y1; ++y)
    for(int x = x0; x < x1; ++x)
    {
        success &= LAB_SetBlock(world, x, y, z, block);
    }
    return success;
}

// hit currently not written
int LAB_TraceBlock(LAB_World* world, int max_distance, float vpos[3], float dir[3], unsigned block_flags,
                   /*out*/ int target[3],/*out*/ int prev[3],/*out*/ float hit[3])
{
    int x, y, z;
    x = LAB_FastFloorF2I(vpos[0]);
    y = LAB_FastFloorF2I(vpos[1]);
    z = LAB_FastFloorF2I(vpos[2]);

    int stepX, stepY, stepZ; // -1 or +1
    stepX = dir[0]<0?-1:+1;
    stepY = dir[1]<0?-1:+1;
    stepZ = dir[2]<0?-1:+1;

    float ivX, ivY, ivZ;
    ivX = fabsf(dir[0]) < 0.00001 ? 100000.f : 1.f / fabsf(dir[0]);
    ivY = fabsf(dir[1]) < 0.00001 ? 100000.f : 1.f / fabsf(dir[1]);
    ivZ = fabsf(dir[2]) < 0.00001 ? 100000.f : 1.f / fabsf(dir[2]);

    // NOTE: same floor rounding behavior as above!
    //       the values x y z could be reused here
    // TODO: possible problem, because number is converted to int and back to float again
    #define MOD1(v) ((v)-LAB_FastFloorF2I(v))
    float tMaxX, tMaxY, tMaxZ;
    if(fabsf(dir[0]) < 0.00001) tMaxX = max_distance; else { tMaxX = MOD1(vpos[0]); { if(dir[0]>0) tMaxX = 1.f-tMaxX; } tMaxX = fabsf(tMaxX * ivX); }
    if(fabsf(dir[1]) < 0.00001) tMaxY = max_distance; else { tMaxY = MOD1(vpos[1]); { if(dir[1]>0) tMaxY = 1.f-tMaxY; } tMaxY = fabsf(tMaxY * ivY); }
    if(fabsf(dir[2]) < 0.00001) tMaxZ = max_distance; else { tMaxZ = MOD1(vpos[2]); { if(dir[2]>0) tMaxZ = 1.f-tMaxZ; } tMaxZ = fabsf(tMaxZ * ivZ); }

    // TODO:
    float tDeltaX, tDeltaY, tDeltaZ;
    tDeltaX = ivX;
    tDeltaY = ivY;
    tDeltaZ = ivZ;


    target[0] = x;
    target[1] = y;
    target[2] = z;

    //     LAB_Vec3Algo_RayVsRect()
//    if(LAB_GetBlock(world, x, y, z, flags)->flags&block_flags
//       /* TODO */)
//    {
//        prev[0] = x;
//        prev[1] = y;
//        prev[2] = z;
//        return 1;
//    }

    // loop
    do
    {
        LAB_Block* b = LAB_GetBlockP(world, x, y, z);
        if(b->flags&block_flags)
        {
            float rect1[3], rect2[3];
            LAB_Vec3_Add(rect1, target, b->bounds[0]);
            LAB_Vec3_Add(rect2, target, b->bounds[1]);

            if(   rect1[0] <= vpos[0] && vpos[0] <= rect2[0]
               && rect1[1] <= vpos[1] && vpos[1] <= rect2[1]
               && rect1[2] <= vpos[2] && vpos[2] <= rect2[2])
            {
                LAB_Vec3_Copy(prev, target);
                return 1;
            }

            float collision_point[3];
            float collision_steps;
            int   collision_face;
            if(LAB_Vec3Algo_RayVsRect(collision_point, &collision_steps, &collision_face,
                                      vpos, dir, rect1, rect2)
               && collision_steps < max_distance)
            {
                prev[0] = target[0] + LAB_OX(collision_face);
                prev[1] = target[1] + LAB_OY(collision_face);
                prev[2] = target[2] + LAB_OZ(collision_face);
                return 1;
            }
        }


        if(tMaxX < tMaxY)
        {
            if(tMaxX < tMaxZ)
                x += stepX, tMaxX += tDeltaX;
            else
                z += stepZ, tMaxZ += tDeltaZ;
        }
        else
        {
            if(tMaxY < tMaxZ)
                y += stepY, tMaxY += tDeltaY;
            else
                z += stepZ, tMaxZ += tDeltaZ;
        }

        target[0] = x;
        target[1] = y;
        target[2] = z;
    }
    while(tMaxX < max_distance || tMaxY < max_distance || tMaxZ < max_distance);

    return 0;
}


typedef struct LAB_UpdatePQ_Entry { int table_index; int distance; } LAB_UpdatePQ_Entry;
#define LAB_UPDATE_PQ_NAME          LAB_UpdatePQ
#define LAB_UPDATE_PQ_TYPE          struct LAB_UpdatePQ_Entry
#define LAB_UPDATE_PQ_PRIO_TYPE     int
#define LAB_UPDATE_PQ_PRIO(e)       ((e)->distance)
#define LAB_UPDATE_PQ_PRIO_HIGHER_THAN(a, b) ((a) < (b))

#define HTL_PARAM LAB_UPDATE_PQ
#include "HTL/prio_queue.t.h"
#include "HTL/prio_queue.t.c"
#undef HTL_PARAM

#if 0
LAB_STATIC void LAB_World_UpdateChunks(LAB_World* world, uint32_t delta_ms, uint64_t nanos)
{
    double view_pos[3];
    LAB_ASSERT(world->view);
    world->view->position(world->view_user, world, view_pos);
    int px = LAB_Sar(LAB_FastFloorF2I(view_pos[0]), LAB_CHUNK_SHIFT);
    int py = LAB_Sar(LAB_FastFloorF2I(view_pos[1]), LAB_CHUNK_SHIFT);
    int pz = LAB_Sar(LAB_FastFloorF2I(view_pos[2]), LAB_CHUNK_SHIFT);



    size_t queue_capacity = world->cfg.max_update ? world->cfg.max_update : 256;
    queue_capacity = 30;
    LAB_UpdatePQ q;
    bool success = LAB_UpdatePQ_Create(&q, queue_capacity);
    if(!success) { /* TODO */ }

    for(size_t i = 0; i < world->chunks.capacity; ++i)
    {
        LAB_World_ChunkEntry* e = &world->chunks.table[i];
        LAB_Chunk* chunk = e->chunk;
        if(LAB_Chunk_Access(chunk) && chunk->dirty)
        {
            int distance = (px-e->pos.x)*(px-e->pos.x)
                         + (py-e->pos.y)*(py-e->pos.y)
                         + (pz-e->pos.z)*(pz-e->pos.z);

            // shift elements to the right and find insertion position
            LAB_UpdatePQ_Entry* qe = LAB_UpdatePQ_Push(&q, distance);
            if(qe) { qe->table_index = i; qe->distance = distance; }
        }
    }

    while(!LAB_UpdatePQ_IsEmpty(&q))
    {
        int i = LAB_UpdatePQ_Front(&q)->table_index;
        LAB_UpdatePQ_PopFront(&q);

        LAB_Chunk* chunk = world->chunks.table[i].chunk;
        LAB_ASSERT(LAB_Chunk_Access(chunk));
        LAB_ChunkUpdate update = chunk->dirty;
        chunk->dirty = 0;
        LAB_ChunkPos* pos = &world->chunks.table[i].pos;
        LAB_ASSERT(chunk->generated);
        LAB_UpdateChunk(world, chunk, pos->x, pos->y, pos->z, update);
        if(LAB_NanoSeconds() - nanos > 2500*1000) break; // 2.5 ms
    }
    LAB_UpdatePQ_Destroy(&q);
}
#endif


void LAB_WorldTick(LAB_World* world)
{
    double view_pos[3];
    LAB_ASSERT(world->view);
    world->view->position(world->view_user, world, view_pos);
    world->px = LAB_Sar(LAB_FastFloorF2I(view_pos[0]), LAB_CHUNK_SHIFT);
    world->py = LAB_Sar(LAB_FastFloorF2I(view_pos[1]), LAB_CHUNK_SHIFT);
    world->pz = LAB_Sar(LAB_FastFloorF2I(view_pos[2]), LAB_CHUNK_SHIFT);
}
