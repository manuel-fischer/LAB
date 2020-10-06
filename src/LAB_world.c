#include "LAB_world.h"

#include "LAB_memory.h"
#include "LAB_error.h"

#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_debug.h"
#include "LAB_util.h"
#include "LAB_bits.h"
#include "LAB_world_light.h"

#include <math.h>
#include <stdio.h> // DBG


/*#define HTL_PARAM LAB_CHUNK_MAP
#include "HTL_hashmap.t.c"
#undef HTL_PARAM*/

#define HTL_PARAM LAB_CHUNK_TBL
#include "HTL_hasharray.t.c"
#undef HTL_PARAM

#define HTL_PARAM LAB_CHUNKPOS_QUEUE
#include "HTL_queue.t.c"
#undef HTL_PARAM

/*#define HTL_PARAM LAB_CHUNKPOS2_QUEUE
#include "HTL_queue.t.c"
#undef HTL_PARAM*/

unsigned LAB_ChunkPosHash(LAB_ChunkPos pos)
{
    //return (unsigned)pos.x*257 + (unsigned)pos.y*8191 + (unsigned)pos.y*65537;
    //return (unsigned)pos.x*7 + (unsigned)pos.y*13 + (unsigned)pos.y*19;
    return (unsigned)pos.x
         ^ (unsigned)pos.y << (unsigned)6 ^ pos.y << 4
         ^ (unsigned)pos.z << (unsigned)2 ^ pos.z << 7;
}

int LAB_ChunkPosComp(LAB_ChunkPos a, LAB_ChunkPos b)
{
    //return a.x != b.x || a.y != b.y || a.z != b.z;
    //return (a.x != b.x) | (a.y != b.y) | (a.z != b.z);
    //return memcmp(&a, &b, sizeof a)!=0;
    LAB_ChunkPos tmp_a, tmp_b;
    tmp_a=a; tmp_b=b;
    return memcmp(&tmp_a, &tmp_b, sizeof a)!=0;
}



int LAB_ConstructWorld(LAB_World* world)
{
    // those never fail
    LAB_ChunkTBL_Create(&world->chunks);
    LAB_ChunkPosQueue_Construct(&world->gen_queue);
    //LAB_ChunkPos2Queue_Construct(&world->update_queue);
    return 1;
}

void LAB_DestructWorld(LAB_World* world)
{
    //LAB_ChunkPos2Queue_Destruct(&world->update_queue);
    LAB_ChunkPosQueue_Destruct(&world->gen_queue);
    for(size_t i = 0; i < world->chunks.capacity; ++i)
    {
        LAB_World_ChunkEntry* entry = &world->chunks.table[i];
        if(entry->chunk != NULL)
            LAB_DestroyChunk(entry->chunk);
    }
    LAB_ChunkTBL_Destroy(&world->chunks);
}


static LAB_Chunk* LAB_GenerateNotifyChunk(LAB_World* world, int x, int y, int z)
{
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

    chunk = (*world->chunkgen)(world->chunkgen_user, world, x, y, z);
    if(LAB_UNLIKELY(chunk == NULL))
    {
        // Because the inserted entry was not changed
        // We can discard it
        LAB_ChunkTBL_Discard(&world->chunks, entry);
        return NULL;
    }

    // Slot gets occupied, because >chunk< is nonzero
    entry->pos = pos;
    entry->chunk = chunk;

    LAB_UpdateChunk(world, x, y, z, LAB_CHUNK_UPDATE_BLOCK);

    return chunk;
}


LAB_Chunk* LAB_GetChunk(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags)
{
    LAB_ChunkPos pos = { x, y, z };
/*    if(world->last_entry && memcmp(&world->last_entry->key, &pos, sizeof pos)==0)
    {
        world->last_entry->value->age = 0;
        return world->last_entry->value;
    }*/
    LAB_World_ChunkEntry* entry = LAB_ChunkTBL_Get(&world->chunks, pos);
    if(entry != NULL)
    {
//        world->last_entry = entry;
        entry->chunk->age = 0;
        return entry->chunk;
    }

    if(flags == LAB_CHUNK_GENERATE)
    {
        return LAB_GenerateNotifyChunk(world, x, y, z);
    }
    else if(flags == LAB_CHUNK_GENERATE_LATER)
    {
        //printf("Enqueue %i, %i, %i\n", x, y, z);
        LAB_ChunkPos* request;
        request = LAB_ChunkPosQueue_PushBack(&world->gen_queue);
        if(request != NULL) {
            request->x = x;
            request->y = y;
            request->z = z;
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}

void LAB_GetChunkNeighborhood(LAB_World* world, LAB_Chunk* /*out*/ chunks[27], int x, int y, int z, LAB_ChunkPeekType flags)
{
    LAB_Chunk** itr = chunks;
    for(int iz = -1; iz <= 1; ++iz)
    for(int iy = -1; iy <= 1; ++iy)
    for(int ix = -1; ix <= 1; ++ix)
    {
        *itr = LAB_GetChunk(world, x+ix, y+iy, z+iz, flags);
        itr++;
    }
}



void LAB_UpdateChunk(LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update)
{
    // TODO when block was placed at chunk border
    if(LAB_LIKELY(world->chunkview != NULL))
    {
        LAB_Chunk* chunks[27];
        LAB_GetChunkNeighborhood(world, chunks, x, y, z, LAB_CHUNK_EXISTING);
        int faces = LAB_TickLight(world, chunks, x, y, z);
        for(int face_itr=faces&63; face_itr; face_itr &= face_itr-1)
        {
            int face = LAB_Ctz(face_itr);
            LAB_UpdateChunkLater(world, x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face), LAB_CHUNK_UPDATE_LIGHT);
        }
        if(faces&128)
        {
            LAB_ASSUME(chunks[1+3+9]);
            printf("UPDATE AGAIN");
            chunks[1+3+9]->dirty |= LAB_CHUNK_UPDATE_LIGHT; // TODO: |LAB_CHUNK_UPDATE_LOCAL;
        }

        (*world->chunkview)(world->chunkview_user, world, x, y, z, update);
    }
}


/*static int LAB_NotifyChunkLater_Comp(void* ctx, LAB_ChunkPos* a)
{
    LAB_ChunkPos* b = ctx;
    return LAB_ChunkPosComp(*a, *b);
}

void LAB_NotifyChunkLater(LAB_World* world, int x, int y, int z)
{
    LAB_ChunkPos pos = {x, y, z};
    if(LAB_ChunkPos2Queue_Find(&world->update_queue, LAB_NotifyChunkLater_Comp, &pos))
        return;

    LAB_ChunkPos* entry = LAB_ChunkPos2Queue_PushBack(&world->update_queue);
    if(entry != NULL) {
        entry->x = x;
        entry->y = y;
        entry->z = z;
    }
}*/


void LAB_UpdateChunkLater(LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update)
{
    LAB_Chunk* chunk = LAB_GetChunk(world, x, y, z, LAB_CHUNK_EXISTING);
    //if(chunk) chunk->dirty = 1;
    if(chunk) chunk->dirty |= update;
}


LAB_Block* LAB_GetBlock(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags)
{
    int cx, cy, cz;
    cx = x>>LAB_CHUNK_SHIFT; cy = y>>LAB_CHUNK_SHIFT; cz = z>>LAB_CHUNK_SHIFT;

    LAB_Chunk* chunk = LAB_GetChunk(world, cx, cy, cz, flags);
    if(chunk == NULL) return &LAB_BLOCK_OUTSIDE;
    return chunk->blocks[LAB_CHUNK_OFFSET(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK)];
}

void LAB_SetBlock(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags, LAB_Block* block)
{
    int cx, cy, cz;
    cx = x>>LAB_CHUNK_SHIFT; cy = y>>LAB_CHUNK_SHIFT; cz = z>>LAB_CHUNK_SHIFT;

    LAB_Chunk* chunk = LAB_GetChunk(world, cx, cy, cz, flags);
    if(chunk == NULL) return;
    chunk->blocks[LAB_CHUNK_OFFSET(x&LAB_CHUNK_MASK, y&LAB_CHUNK_MASK, z&LAB_CHUNK_MASK)] = block;
    //LAB_NotifyChunkLater(world, cx, cy, cz);
    chunk->modified = 1;
    LAB_UpdateChunkLater(world, cx, cy, cz, LAB_CHUNK_UPDATE_BLOCK);
}

void LAB_FillBlocks(LAB_World* world, int x0, int y0, int z0, int x1, int y1, int z1, LAB_ChunkPeekType flags, LAB_Block* block)
{
    // TODO optimize without repeeking chunks
    for(int z = z0; z < z1; ++z)
    for(int y = y0; y < y1; ++y)
    for(int x = x0; x < x1; ++x)
    {
        LAB_SetBlock(world, x, y, z, flags, block);
    }
}

// hit currently not written
int LAB_TraceBlock(LAB_World* world, int max_distance, float vpos[3], float dir[3], LAB_ChunkPeekType flags, unsigned block_flags,
                   /*out*/ int target[3],/*out*/ int prev[3],/*out*/ float hit[3])
{
    #if 0
    // TODO more efficient
    float p[3];
    p[0] = vpos[0];
    p[1] = vpos[1];
    p[2] = vpos[2];

    prev[0] = (int)floorf(p[0]);
    prev[1] = (int)floorf(p[1]);
    prev[2] = (int)floorf(p[2]);

    if(LAB_GetBlock(world, prev[0], prev[1], prev[2], flags)->flags&block_flags)
    {
        target[0] = prev[0];
        target[1] = prev[1];
        target[2] = prev[2];
        return 1;
    }
    for(int i = 0; i < max_distance*16; ++i)
    {
        p[0] += dir[0]/16;
        p[1] += dir[1]/16;
        p[2] += dir[2]/16;
        target[0] = (int)floorf(p[0]);
        target[1] = (int)floorf(p[1]);
        target[2] = (int)floorf(p[2]);
        if(LAB_GetBlock(world, target[0], target[1], target[2], flags)->flags&block_flags)
        {
            return 1;
        }
        prev[0] = target[0];
        prev[1] = target[1];
        prev[2] = target[2];
    }
    return 0;
    #else
    int x, y, z;
    x = (int)floorf(vpos[0]);
    y = (int)floorf(vpos[1]);
    z = (int)floorf(vpos[2]);

    int stepX, stepY, stepZ; // -1 or +1
    stepX = dir[0]<0?-1:+1;
    stepY = dir[1]<0?-1:+1;
    stepZ = dir[2]<0?-1:+1;

    float ivX, ivY, ivZ;
    ivX = fabsf(dir[0]) < 0.00001 ? 100000.f : 1.f / fabsf(dir[0]);
    ivY = fabsf(dir[1]) < 0.00001 ? 100000.f : 1.f / fabsf(dir[1]);
    ivZ = fabsf(dir[2]) < 0.00001 ? 100000.f : 1.f / fabsf(dir[2]);

    #define MOD1(v) ((v)-floorf(v))
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


    if(LAB_GetBlock(world, x, y, z, flags)->flags&block_flags)
    {
        prev[0] = x;
        prev[1] = y;
        prev[2] = z;
        return 1;
    }

    // loop
    while(tMaxX < max_distance || tMaxY < max_distance || tMaxZ < max_distance)
    {
        prev[0] = x;
        prev[1] = y;
        prev[2] = z;

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

        if(LAB_GetBlock(world, x, y, z, flags)->flags&block_flags)
        {
            return 1;
        }
    }

    return 0;
    #endif
}



void LAB_WorldTick(LAB_World* world, uint32_t delta_ms)
{
    size_t rest_gen = world->max_gen;
    while(!LAB_ChunkPosQueue_IsEmpty(&world->gen_queue))
    {
        LAB_ChunkPos* pos;
        pos = LAB_ChunkPosQueue_Front(&world->gen_queue);
        LAB_GenerateNotifyChunk(world, pos->x, pos->y, pos->z);
        LAB_ChunkPosQueue_PopFront(&world->gen_queue);
        if(--rest_gen == 0) break;
    }

    size_t rest_update = world->max_update;
    for(size_t i = 0; i < world->chunks.capacity; ++i)
    {
        LAB_Chunk* chunk = world->chunks.table[i].chunk;
        if(chunk && chunk->dirty)
        {
            LAB_ChunkUpdate update = chunk->dirty;
            chunk->dirty = 0;
            LAB_ChunkPos* pos = &world->chunks.table[i].pos;
            LAB_UpdateChunk(world, pos->x, pos->y, pos->z, update);
            if(--rest_update == 0) return;
        }
    }

    // Unload chunks
    for(size_t i = 0; i < world->chunks.capacity; ++i)
    {
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
                bool keep = world->chunkkeep(world->chunkkeep_user, world, cx, cy, cz); // DBG
                if(!keep)
                {
                    // Only entries after this entry are changed, another entry
                    // might be moved into this entry, the array itself is not
                    // reallocated when removing entries
                    //printf("Unload chunk %i %i %i\n", cx, cy, cz);
                    LAB_DestroyChunk(chunk);
                    LAB_ChunkTBL_RemoveEntry(&world->chunks, &world->chunks.table[i]);
                    --i; // repeat index
                }
            }
        }
    }
}
