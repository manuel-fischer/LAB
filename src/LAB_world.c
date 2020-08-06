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

void LAB_TickLight(LAB_World* world, LAB_Chunk* chunks[27], int cx, int cy, int cz);

unsigned LAB_ChunkPosHash(LAB_ChunkPos pos)
{
    //return (unsigned)pos.x*257 + (unsigned)pos.y*8191 + (unsigned)pos.y*65537;
    return (unsigned)pos.x*7 + (unsigned)pos.y*13 + (unsigned)pos.y*19;
}

int LAB_ChunkPosComp(LAB_ChunkPos a, LAB_ChunkPos b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}



LAB_World* LAB_CreateWorld()
{
    LAB_World* world = LAB_Calloc(1, sizeof *world); // filled with zeros
    if(LAB_UNLIKELY(world==NULL))
    {
        LAB_SetError("LAB_CreateWorld failed to allocate");
        return NULL;
    }

    LAB_ChunkMap_Construct(&world->chunks);
    return world;
}

void LAB_DestroyWorld(LAB_World* world)
{
    LAB_Free(world);
}


LAB_Chunk* LAB_GetChunk(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags)
{
#if 0
    int chunk_count, chunk_capacity;
    LAB_ChunkEntry* chunks;
    LAB_Chunk* chunk;

    chunk_count = world->chunk_count;
    chunks = world->chunks;

    for(int i = 0; i < chunk_count; ++i)
    {
        if(chunks[i].x == x && chunks[i].y == y && chunks[i].z == z)
            return chunks[i].chunk;
    }

    chunk_capacity = world->chunk_capacity;

    if(flags == LAB_CHUNK_GENERATE)
    {
        if(chunk_count >= chunk_capacity)
        {
            if(chunk_capacity == 0)
                chunk_capacity = 16;
            else
                chunk_capacity *= 2;

            chunks = LAB_ReallocN(chunks, chunk_capacity, sizeof *chunks);
            if(!chunks) return NULL;
            world->chunk_capacity = chunk_capacity;
            world->chunks = chunks;
        }

        if(world->chunkgen == NULL) return NULL;

        chunk = (*world->chunkgen)(world->chunkgen_user, world, x, y, z);
        if(!chunk) return NULL;

        chunks[chunk_count].x = x;
        chunks[chunk_count].y = y;
        chunks[chunk_count].z = z;
        chunks[chunk_count].chunk = chunk;
        world->chunk_count++;

        LAB_NotifyChunk(world, x, y, z);

        return chunk;
    }
    else
    {
        return NULL;
    }
#endif

    LAB_ChunkPos pos = { x, y, z };
    LAB_ChunkMap_Entry* entry = LAB_ChunkMap_Get(&world->chunks, pos);
    if(entry != NULL)
    {
        return entry->value;
    }

    if(flags == LAB_CHUNK_GENERATE)
    {
        LAB_ChunkMap_Entry* entry;
        LAB_Chunk* chunk;
        entry = LAB_ChunkMap_PutKey(&world->chunks, pos);
        // Create slot for chunk
        // Note that it does not really exist, because the value is NULL
        // Entry must be written before anything further happens
        if(LAB_UNLIKELY(entry == NULL)) return NULL;
        chunk = (*world->chunkgen)(world->chunkgen_user, world, x, y, z);
        if(LAB_UNLIKELY(chunk == NULL)) return NULL;
            // Because the inserted entry is not really existing
            // We can silently discard it

        // Slot gets occupied, because >chunk< is nonzero
        entry->value = chunk;

        LAB_NotifyChunk(world, x, y, z);

        return chunk;
    }
    return NULL;
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
        LAB_TickLight(world, chunks, x, y, z);

        (*world->chunkview)(world->chunkview_user, world, x, y, z);
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
    LAB_NotifyChunk(world, cx, cy, cz);
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





void LAB_TickLight(LAB_World* world, LAB_Chunk* chunks[27], int cx, int cy, int cz)
{
    LAB_Chunk* cnk = chunks[1+3+9];
    if(!cnk) return;
    for(int i = 0; i < 16; ++i)
    for(int z = 0; z < 16; ++z)
    for(int y = 0; y < 16; ++y)
    for(int x = 0; x < 16; ++x)
    {
        int off = LAB_CHUNK_OFFSET(x, y, z);

        int lum = 0;
        if(!(cnk->blocks[off]->flags & LAB_BLOCK_SOLID))
        {
            for(int i = 0; i < 6; ++i)
            {
                const int* o = &LAB_offset[i];
                int nlum;

                LAB_Block* block = LAB_GetNeighborhoodBlock(chunks, x+o[0], y+o[1], z+o[2]);
                if(block->flags & LAB_BLOCK_EMISSIVE)
                    nlum = block->lr;
                else
                {
                    nlum = LAB_GetNeighborhoodLight(chunks, x+o[0], y+o[1], z+o[2]);
                    if(i!=3) nlum = nlum-(nlum>>4);
                }
                if(nlum > lum) lum = nlum;
            }
        }
        if(lum < 16) lum = 16;
        if(lum > 255) lum = 255;
        cnk->light[off] = lum;
    }
}













