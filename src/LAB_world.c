#include "LAB_world.h"

#include "LAB_memory.h"
#include "LAB_error.h"

#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_util.h"

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

int LAB_TickLight(LAB_World* world, LAB_Chunk* chunks[27], int cx, int cy, int cz);

unsigned LAB_ChunkPosHash(LAB_ChunkPos pos)
{
    //return (unsigned)pos.x*257 + (unsigned)pos.y*8191 + (unsigned)pos.y*65537;
    return (unsigned)pos.x*7 + (unsigned)pos.y*13 + (unsigned)pos.y*19;
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

    LAB_NotifyChunk(world, x, y, z);

    return chunk;
}


LAB_Chunk* LAB_GetChunk(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags)
{
    LAB_ChunkPos pos = { x, y, z };
    LAB_ChunkMap_Entry* entry = LAB_ChunkMap_Get(&world->chunks, pos);
    if(entry != NULL)
    {
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



void LAB_NotifyChunk(LAB_World* world, int x, int y, int z)
{
    if(LAB_LIKELY(world->chunkview != NULL))
    {
        LAB_Chunk* chunks[27];
        LAB_GetChunkNeighborhood(world, chunks, x, y, z, LAB_CHUNK_EXISTING);
        for(int j = 0; j < 16; ++j)
        {
            int borders = LAB_TickLight(world, chunks, x, y, z);
            int borders2 = 0;
            if(borders)
            {
                #if 1
                for(int zz = -1; zz <= 1; ++zz)
                for(int yy = -1; yy <= 1; ++yy)
                for(int xx = -1; xx <= 1; ++xx)
                {
                    if(!xx && !yy & !zz) continue;
                    LAB_GetChunkNeighborhood(world, chunks, x+xx, y+yy, z+zz, LAB_CHUNK_EXISTING);
                    borders2 |= LAB_TickLight(world, chunks, x+xx, y+yy, z+zz);
                    (*world->chunkview)(world->chunkview_user, world, x, y, z);
                }
                #else
                for(int i = 0; i < 6; ++i)
                {
                    const int* o = LAB_offset[i];
                    //LAB_NotifyChunkLater(world, x+o[0], y+o[1], z+o[2]);
                    LAB_GetChunkNeighborhood(world, chunks, x+o[0], y+o[1], z+o[2], LAB_CHUNK_EXISTING);
                    LAB_TickLight(world, chunks, x+o[0], y+o[1], z+o[2]);
                    //(*world->chunkview)(world->chunkview_user, world, x+o[0], y+o[1], z+o[2]);
                }
                #endif
            }
            if(!borders2 && !borders) break;
            //break;
        }

        (*world->chunkview)(world->chunkview_user, world, x, y, z);
    }
}


static int LAB_NotifyChunkLater_Comp(void* ctx, LAB_ChunkPos* a)
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
    LAB_NotifyChunkLater(world, cx, cy, cz);
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
    // TODO more efficient
    float p[3];
    p[0] = vpos[0];
    p[1] = vpos[1];
    p[2] = vpos[2];

    prev[0] = (int)floor(p[0]);
    prev[1] = (int)floor(p[1]);
    prev[2] = (int)floor(p[2]);

    for(int i = 0; i < max_distance*16; ++i)
    {
        p[0] += dir[0]/16;
        p[1] += dir[1]/16;
        p[2] += dir[2]/16;
        target[0] = (int)floor(p[0]);
        target[1] = (int)floor(p[1]);
        target[2] = (int)floor(p[2]);
        if(LAB_GetBlock(world, target[0], target[1], target[2], flags)->flags&block_flags)
        {
            return 1;
        }
        prev[0] = target[0];
        prev[1] = target[1];
        prev[2] = target[2];
    }
    return 0;
}




void LAB_WorldTick(LAB_World* world)
{
    while(!LAB_ChunkPosQueue_IsEmpty(&world->gen_queue))
    {
        LAB_ChunkPos* pos;
        pos = LAB_ChunkPosQueue_Front(&world->gen_queue);
        LAB_GenerateNotifyChunk(world, pos->x, pos->y, pos->z);
        LAB_ChunkPosQueue_PopFront(&world->gen_queue);
    }

    while(!LAB_ChunkPos2Queue_IsEmpty(&world->update_queue))
    {
        LAB_ChunkPos* pos;
        pos = LAB_ChunkPos2Queue_Front(&world->update_queue);
        LAB_NotifyChunk(world, pos->x, pos->y, pos->z);
        LAB_ChunkPos2Queue_PopFront(&world->update_queue);
    }
}


LAB_HOT
int LAB_TickLight(LAB_World* world, LAB_Chunk* chunks[27], int cx, int cy, int cz)
{
    LAB_Chunk* cnk = chunks[1+3+9];
    if(!cnk) return 0;
    #if 1
    memset(cnk->light, 0xff, sizeof cnk->light);
    return 0;
    #endif
    memset(cnk->light, 0, sizeof cnk->light);

    LAB_Color default_color = cy <= -5 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);



    /*for(int z = 0; z < 16; ++z)
    for(int y = 0; y < 16; ++y)
    for(int x = 0; x < 16; ++x)
    {
        int off = LAB_CHUNK_OFFSET(x, y, z);
        if(!(cnk->blocks[off]->flags & LAB_BLOCK_OPAQUE))
        {

        }
        else
        {
            cnk->light[off] = 0;
        }
    }*/

    //return;
    //for(int n = 0; n < 16; ++n)
    int changed = 1;
    int change_count = -1;
    while(changed)
    {
        changed = 0;
        for(int z = 0; z < 16; ++z)
        for(int y =15; y >= 0; --y)
        for(int x = 0; x < 16; ++x)
        {
            int off = LAB_CHUNK_OFFSET(x, y, z);

            //int lum = 0;
            LAB_Color lum;
            lum = LAB_RGB(16, 16, 16);
            if(!(cnk->blocks[off]->flags & LAB_BLOCK_OPAQUE))
            {
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
                        if(i!=3)
                            nlum = nlum - (nlum>>2 & 0x3f3f3f);
                    }
                    lum = LAB_MaxColor(lum, nlum);
                }
            }
            if(cnk->light[off] != lum)
            {
                cnk->light[off] = lum;
                changed = 1;
            }
        }
        change_count++;
    }
    return change_count>0;
}













