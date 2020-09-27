#include "LAB_world.h"

#include "LAB_memory.h"
#include "LAB_error.h"

#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_debug.h"
#include "LAB_util.h"
#include "LAB_bits.h"

#include <math.h>


#define HTL_PARAM LAB_CHUNK_MAP
#include "HTL_hashmap.t.c"
#undef HTL_PARAM

#define HTL_PARAM LAB_CHUNKPOS_QUEUE
#include "HTL_queue.t.c"
#undef HTL_PARAM

#define HTL_PARAM LAB_CHUNKPOS2_QUEUE
#include "HTL_queue.t.c"
#undef HTL_PARAM

/**
 *  Return face bitset of faces of the chunk cube that were touched
 */
int LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27], int cx, int cy, int cz);

unsigned LAB_ChunkPosHash(LAB_ChunkPos pos)
{
    //return (unsigned)pos.x*257 + (unsigned)pos.y*8191 + (unsigned)pos.y*65537;
    //return (unsigned)pos.x*7 + (unsigned)pos.y*13 + (unsigned)pos.y*19;
    return pos.x
         ^ pos.y << 6 ^ pos.y << 4
         ^ pos.z << 2 ^ pos.z << 7;
}

int LAB_ChunkPosComp(LAB_ChunkPos a, LAB_ChunkPos b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}



int LAB_ConstructWorld(LAB_World* world)
{
    // those never fail
    LAB_ChunkMap_Construct(&world->chunks);
    LAB_ChunkPosQueue_Construct(&world->gen_queue);
    LAB_ChunkPos2Queue_Construct(&world->update_queue);
    return 1;
}

void LAB_DestructWorld(LAB_World* world)
{
    LAB_ChunkPos2Queue_Destruct(&world->update_queue);
    LAB_ChunkPosQueue_Destruct(&world->gen_queue);
    for(size_t i = 0; i < world->chunks.capacity; ++i)
    {
        LAB_ChunkMap_Entry* entry = &world->chunks.table[i];
        if(entry->value)
            LAB_DestroyChunk(entry->value);
    }
    LAB_ChunkMap_Destruct(&world->chunks);
}


static LAB_Chunk* LAB_GenerateNotifyChunk(LAB_World* world, int x, int y, int z)
{
    //printf("GEN %3i, %3i, %3i\n", x, y, z);
    LAB_ChunkPos pos = { x, y, z };
    LAB_ChunkMap_Entry* entry;
    LAB_Chunk* chunk;
    entry = LAB_ChunkMap_PutKey(&world->chunks, pos);

    // Create slot for chunk
    // Note that it does not really exist, because the value is NULL
    // Entry must be written before anything further happens
    if(LAB_UNLIKELY(entry == NULL)) return NULL;

    if(entry->value != NULL) // already generated
    {
        return entry->value;
    }

    chunk = (*world->chunkgen)(world->chunkgen_user, world, x, y, z);
    if(LAB_UNLIKELY(chunk == NULL)) return NULL;
        // Because the inserted entry is not really existing
        // We can silently discard it

    // Slot gets occupied, because >chunk< is nonzero
    entry->value = chunk;

    LAB_UpdateChunk(world, x, y, z, LAB_CHUNK_UPDATE_BLOCK);

    return chunk;
}


LAB_Chunk* LAB_GetChunk(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags)
{
    LAB_ChunkPos pos = { x, y, z };
    if(world->last_entry && memcmp(&world->last_entry->key, &pos, sizeof pos)==0)
    {
        world->last_entry->value->age = 0;
        return world->last_entry->value;
    }
    LAB_ChunkMap_Entry* entry = LAB_ChunkMap_Get(&world->chunks, pos);
    if(entry != NULL)
    {
        world->last_entry = entry;
        entry->value->age = 0;
        return entry->value;
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
        // TODO: ENABLE LATER
        for(int face_itr=faces; face_itr; face_itr &= face_itr-1)
        {
            int face = LAB_Ctz(face_itr);
            const int* o = LAB_offset[face];
            LAB_UpdateChunkLater(world, x+o[0], y+o[1], z+o[2], LAB_CHUNK_UPDATE_LIGHT);
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
        LAB_Chunk* chunk = world->chunks.table[i].value;
        if(chunk && chunk->dirty)
        {
            LAB_ChunkUpdate update = chunk->dirty;
            chunk->dirty = 0;
            LAB_ChunkPos* pos = &world->chunks.table[i].key;
            LAB_UpdateChunk(world, pos->x, pos->y, pos->z, update);
            if(--rest_update == 0) return;
        }
    }

    // Unload chunks
    for(size_t i = 0; i < world->chunks.capacity; ++i)
    {
        LAB_ChunkMap_Entry* entry = &world->chunks.table[i];
        LAB_Chunk* chunk = entry->value;
        if(chunk)
        {
            chunk->age++;
            if(chunk->age >= LAB_MAX_CHUNK_AGE && !chunk->modified)
            {
                int cx, cy, cz;
                cx = entry->key.x;
                cy = entry->key.y;
                cz = entry->key.z;
                bool keep = world->chunkkeep(world->chunkkeep_user, world, cx, cy, cz);
                if(!keep)
                {
                    // Only entries after this entry are changed, another entry
                    // might be moved into this entry, the array itself is not
                    // reallocated when removing entries
                    //printf("Unload chunk %i %i %i\n", cx, cy, cz);
                    LAB_DestroyChunk(chunk);
                    LAB_ChunkMap_RemoveEntry(&world->chunks, &world->chunks.table[i]);
                    --i; // repeat index
                }
            }
        }
    }
}


static LAB_Color LAB_CalcLight(LAB_World* world, LAB_Chunk*const chunks[27], int x, int y, int z, LAB_Color default_color)
{
    LAB_Color lum;
    //lum = LAB_RGB(16, 16, 16);
    //lum = LAB_GetNeighborhoodBlock(chunks, x,y,z)->lum;
    LAB_Chunk* cnk = chunks[1+3+9];
    int off = LAB_CHUNK_OFFSET(x, y, z);
    if(!(cnk->blocks[off]->flags & LAB_BLOCK_OPAQUE))
    {
        lum = LAB_RGB(16, 16, 16);
        for(int i = 0; i < 6; ++i)
        {
            const int* o = LAB_offset[i];
            int nlum;

            LAB_Block* block = LAB_GetNeighborhoodBlock(chunks, x+o[0], y+o[1], z+o[2]);
            if(block->flags & LAB_BLOCK_EMISSIVE)
                nlum = block->lum;
            else
            {
                nlum = LAB_GetNeighborhoodLight(chunks, x+o[0], y+o[1], z+o[2], default_color);
                if(i!=3 || (nlum&0xffffff) != 0xffffff)
                    nlum = nlum - (nlum>>2 & 0x3f3f3f);
            }
            lum = LAB_MaxColor(lum, nlum);
        }
    }
    else
    {
        lum = LAB_GetNeighborhoodBlock(chunks, x,y,z)->lum;
    }
    return lum;
}

/**
 *  Return format: ?SNUDEW in binary
 *  ?: Any light changed
 *  S, N, U, D, E, W: Neighboring chunk needs to be updated
 */
LAB_HOT
static int LAB_CheckLight(LAB_World* world, LAB_Chunk*const chunks[27], LAB_Color default_color)
{
    int changed = 0;

    LAB_Chunk* cnk = chunks[1+3+9];
    for(int z = 0; z < 16; ++z)
    for(int y =15; y >= 0; --y)
    for(int x = 0; x < 16; ++x)
    {
        int off = LAB_CHUNK_OFFSET(x, y, z);

        if(cnk->light[off] != LAB_CalcLight(world, chunks, x, y, z, default_color))
        {
            if(x==0)  changed |=  1;
            if(x==15) changed |=  2;
            if(y==0)  changed |=  4;
            if(y==15) changed |=  8;
            if(z==0)  changed |= 16;
            if(z==15) changed |= 32;
            changed |= 64;
        }
    }
    return changed;
}

LAB_HOT                                           // TODO: |--------------------| not used
int LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27], int cx, int cy, int cz)
{
    LAB_Chunk* cnk = chunks[1+3+9];
    if(!cnk) return 0;

    //LAB_Color default_color = cy <= -5 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);
    LAB_Color default_color = cy <  -2 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);

    int faces_changed = LAB_CheckLight(world, chunks, default_color);
    if(!faces_changed) return 0;
    faces_changed &= 63; // remove change bit

    memset(cnk->light, 0, sizeof cnk->light);

    int changed = 1;
    int change_count = 0;
    while(changed)
    {
        changed = 0;
        for(int z = 0; z < 16; ++z)
        for(int y =15; y >= 0; --y)
        for(int x = 0; x < 16; ++x)
        {
            int off = LAB_CHUNK_OFFSET(x, y, z);

            LAB_Color lum;
            //lum = LAB_RGB(16, 16, 16);
            //lum = LAB_GetNeighborhoodBlock(chunks, x,y,z)->lum;
            if(!(cnk->blocks[off]->flags & LAB_BLOCK_OPAQUE))
            {
                lum = LAB_RGB(16, 16, 16);
                for(int i = 0; i < 6; ++i)
                {
                    const int* o = LAB_offset[i];
                    int nlum;

                    LAB_Block* block = LAB_GetNeighborhoodBlock(chunks, x+o[0], y+o[1], z+o[2]);
                    if(block->flags & LAB_BLOCK_EMISSIVE)
                        nlum = block->lum;
                    else
                    {
                        nlum = LAB_GetNeighborhoodLight(chunks, x+o[0], y+o[1], z+o[2], default_color);
                        if(i!=3 || (nlum&0xffffff) != 0xffffff)
                            nlum = nlum - (nlum>>2 & 0x3f3f3f);
                    }
                    lum = LAB_MaxColor(lum, nlum);
                }
            }
            else
            {
                lum = LAB_GetNeighborhoodBlock(chunks, x,y,z)->lum;
            }
            if(cnk->light[off] != lum)
            {
                cnk->light[off] = lum;
                if(x==0)  faces_changed |=  1;
                if(x==15) faces_changed |=  2;
                if(y==0)  faces_changed |=  4;
                if(y==15) faces_changed |=  8;
                if(z==0)  faces_changed |= 16;
                if(z==15) faces_changed |= 32;
                changed = 1;
            }
        }
        change_count++;
    }
    return faces_changed;
}













