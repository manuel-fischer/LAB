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
#include "LAB_direction.h"

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


LAB_STATIC
LAB_Chunk* LAB_GetChunk_AtPos(LAB_World* world, int x, int y, int z)
{
    LAB_ASSERT(LAB_IsMainThread());
    int cx, cy, cz;
    cx = x>>LAB_CHUNK_SHIFT; cy = y>>LAB_CHUNK_SHIFT; cz = z>>LAB_CHUNK_SHIFT;
    return LAB_GetChunk(world, cx, cy, cz);
}


LAB_BlockID LAB_GetBlock(LAB_World* world, int x, int y, int z)
{
    // TODO: more efficient multithreading
    LAB_Chunk* chunk = LAB_GetChunk_AtPos(world, x, y, z);
    if(chunk == NULL) return LAB_BID_OUTSIDE;


    LAB_BlockID b = LAB_BID_AIR;
    size_t index = LAB_CHUNK_OFFSET(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK);

    LAB_GameServer_Lock1Chunk(world->server, chunk);
    if(chunk->buf_blocks != NULL)
        b = chunk->buf_blocks->blocks[index];
    LAB_GameServer_Unlock1Chunk(world->server, chunk);


    return b;
}

LAB_BlockID LAB_GetBlock_FromMainThread(LAB_World* world, int x, int y, int z)
{
    // TODO: more efficient multithreading
    LAB_Chunk* chunk = LAB_GetChunk_AtPos(world, x, y, z);
    if(chunk == NULL) return LAB_BID_OUTSIDE;


    LAB_BlockID b = LAB_BID_AIR;
    size_t index = LAB_CHUNK_OFFSET(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK);

    if(chunk->buf_blocks != NULL)
        b = chunk->buf_blocks->blocks[index];

    return b;
}

LAB_STATIC
bool LAB_SetBlockIndex_Locked(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_BlockID block)
{
    LAB_Chunk_Blocks* blocks = LAB_Chunk_Blocks_Write(chunk);
    if(!blocks) return false;

    size_t index = LAB_CHUNK_OFFSET(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK);

    blocks->blocks[index] = block;
    //LAB_NotifyChunkLater(world, cx, cy, cz);
    chunk->modified = 1;
    chunk->dirty_blocks |= LAB_CCPS_Pos(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK);
    //LAB_UpdateChunkLater(world, chunk, cx, cy, cz, LAB_CHUNK_UPDATE_BLOCK);
    chunk->dirty = true; // TODO remove

    LAB_GameServer_PushChunkTask(world->server, chunk, LAB_CHUNK_STAGE_LIGHT);
    return true;
}

bool LAB_SetBlock(LAB_World* world, int x, int y, int z, LAB_BlockID block)
{
    LAB_ASSERT(LAB_IsMainThread());

    // TODO: more efficient multithreading
    LAB_Chunk* chunk = LAB_GetChunk_AtPos(world, x, y, z);
    if(chunk == NULL) return false;


    LAB_GameServer_Lock1Chunk(world->server, chunk);
    bool success = LAB_SetBlockIndex_Locked(world, chunk, x, y, z, block);
    LAB_GameServer_Unlock1Chunk(world->server, chunk);

    return success;
}

/*bool LAB_FillBlocks(LAB_World* world, int x0, int y0, int z0, int x1, int y1, int z1, LAB_BlockID block)
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
}*/

// TODO: write routine per chunk
// new routine that traces chunks, with similar overall behavior
LAB_TraceBlock_Result LAB_TraceBlock(LAB_World* world, float max_distance, LAB_Vec3F vpos, LAB_Vec3F dir, unsigned block_flags)
{
    LAB_ASSERT(LAB_IsMainThread());

    LAB_TraceBlock_Result result;

    LAB_Vec3I ipos = LAB_Vec3F2I_FastFloor(vpos);

    LAB_Vec3I step; // -1 or +1
    step.x = dir.x<0?-1:+1;
    step.y = dir.y<0?-1:+1;
    step.z = dir.z<0?-1:+1;

    float ivX, ivY, ivZ;
    ivX = fabsf(dir.x) < 0.00001 ? 100000.f : 1.f / fabsf(dir.x);
    ivY = fabsf(dir.y) < 0.00001 ? 100000.f : 1.f / fabsf(dir.y);
    ivZ = fabsf(dir.z) < 0.00001 ? 100000.f : 1.f / fabsf(dir.z);

    // NOTE: same floor rounding behavior as above!
    //       the values x y z could be reused here
    // TODO: possible problem, because number is converted to int and back to float again
    #define MOD1(v) ((v)-LAB_FastFloorF2I(v))

    LAB_Vec3F tMax;
    if(fabsf(dir.x) < 0.00001) tMax.x = max_distance; else { tMax.x = MOD1(vpos.x); { if(dir.x>0) tMax.x = 1.f-tMax.x; } tMax.x = fabsf(tMax.x * ivX); }
    if(fabsf(dir.y) < 0.00001) tMax.y = max_distance; else { tMax.y = MOD1(vpos.y); { if(dir.y>0) tMax.y = 1.f-tMax.y; } tMax.y = fabsf(tMax.y * ivY); }
    if(fabsf(dir.z) < 0.00001) tMax.z = max_distance; else { tMax.z = MOD1(vpos.z); { if(dir.z>0) tMax.z = 1.f-tMax.z; } tMax.z = fabsf(tMax.z * ivZ); }

    // TODO:
    LAB_Vec3F tDelta = { ivX, ivY, ivZ };

    // loop
    do
    {
        LAB_Block* b = LAB_GetBlockP_FromMainThread(world, ipos.x, ipos.y, ipos.z);
        if(b->flags&block_flags)
        {
            LAB_Box3F b_box = (LAB_Box3F) {
                LAB_Vec3F_FromArray(b->bounds[0]),
                LAB_Vec3F_FromArray(b->bounds[1]),
            };
            LAB_Box3F box = LAB_Box3F_Add(b_box, LAB_Vec3I2F(ipos));

            if(LAB_Box3F_Contains_Inc(box, vpos)) // only for first block
            {
                result.hit_block = ipos;
                result.prev_block = ipos;
                result.hit_point = vpos;
                return (result.has_hit = true, result);
            }

            LAB_Vec3Algo_RayVsBox_Hit hit;
            if((hit = LAB_Vec3Algo_RayVsBox(vpos, dir, box)).has_hit
               && hit.collision_steps < max_distance)
            {
                result.hit_block = ipos;
                result.prev_block = LAB_Vec3I_Add(ipos, LAB_Vec3I_FromDirIndex(hit.collision_face));
                result.hit_point = hit.collision_point;
                return (result.has_hit = true, result);
            }
        }

        int idx = LAB_Vec3F_MinIndex(tMax);
        *LAB_Vec3I_Ref(&ipos, idx) += LAB_Vec3I_Get(step,   idx);
        *LAB_Vec3F_Ref(&tMax, idx) += LAB_Vec3F_Get(tDelta, idx);
    }
    while(LAB_Vec3F_GetMin(tMax) < max_distance);

    return (result.has_hit = false, result);
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

void LAB_WorldTick(LAB_World* world)
{
    LAB_ASSERT(world->view);

    LAB_Vec3D view_pos = world->view->position(world->view_user, world);
    LAB_Vec3I_Unpack(&world->px, &world->py, &world->pz, LAB_Pos3D2Chunk(view_pos));
}
