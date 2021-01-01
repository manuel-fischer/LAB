#pragma once

#include "LAB_crammed_chunk_pos_set.h"
#include "LAB_chunk.h"
#include "LAB_stdinc.h"
#include "LAB_debug.h"

#include "LAB_check.h"
#include "LAB_opt.h"


typedef struct LAB_ChunkPos
{
    int16_t x, y, z;
    LAB_PAD(int16_t);
    //int32_t x, y, z;
    //LAB_PAD(int32_t);
} LAB_ChunkPos;
LAB_CHECK_STRUCT_SIZE(LAB_ChunkPos);

LAB_ALWAYS_INLINE
LAB_INLINE unsigned LAB_ChunkPosHash(LAB_ChunkPos pos)
{
    //return (unsigned)pos.x*257 + (unsigned)pos.y*8191 + (unsigned)pos.y*65537;
    //return (unsigned)pos.x*7 + (unsigned)pos.y*13 + (unsigned)pos.y*19;
    return (unsigned)pos.x
         ^ (unsigned)pos.y << (unsigned)6 ^ pos.y << 4
         ^ (unsigned)pos.z << (unsigned)2 ^ pos.z << 7;
}

LAB_ALWAYS_INLINE
LAB_INLINE int LAB_ChunkPosComp(LAB_ChunkPos a, LAB_ChunkPos b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
    //return (a.x != b.x) | (a.y != b.y) | (a.z != b.z);
    //return memcmp(&a, &b, sizeof a)!=0;
    /*LAB_ChunkPos tmp_a, tmp_b;
    tmp_a=a; tmp_b=b;
    return memcmp(&tmp_a, &tmp_b, sizeof a)!=0;*/
}


#define LAB_MAX_CHUNK_AGE 256
//#define LAB_MAX_LOAD_CHUNK 16
#define LAB_MAX_LOAD_CHUNK 256

#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)+1)
#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)+2)



#if 0
/*#define LAB_CHUNK_MAP_NAME                 LAB_ChunkMap
#define LAB_CHUNK_MAP_KEY_TYPE             LAB_ChunkPos
#define LAB_CHUNK_MAP_VALUE_TYPE           LAB_Chunk*
#define LAB_CHUNK_MAP_HASH_FUNC            LAB_ChunkPosHash
#define LAB_CHUNK_MAP_COMP_FUNC            LAB_ChunkPosComp
#define LAB_CHUNK_MAP_CALLOC               LAB_Calloc
#define LAB_CHUNK_MAP_FREE                 LAB_Free
#define LAB_CHUNK_MAP_LOAD_NUM             3
#define LAB_CHUNK_MAP_LOAD_DEN             4
#define LAB_CHUNK_MAP_GROW_FACTOR          2
#define LAB_CHUNK_MAP_INITIAL_CAPACITY     16
#define LAB_CHUNK_MAP_NULL_REPR            (entry->value == NULL)

#define HTL_PARAM LAB_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM*/


#else


typedef struct LAB_World_ChunkEntry
{
    LAB_ChunkPos pos;
    LAB_Chunk* chunk;
} LAB_World_ChunkEntry;

#define LAB_CHUNK_TBL_NAME             LAB_ChunkTBL
#define LAB_CHUNK_TBL_KEY_TYPE         LAB_ChunkPos
#define LAB_CHUNK_TBL_ENTRY_TYPE       LAB_World_ChunkEntry
#define LAB_CHUNK_TBL_KEY_FUNC(e)      ((e)->pos)
#define LAB_CHUNK_TBL_HASH_FUNC(k)     LAB_ChunkPosHash(k)
#define LAB_CHUNK_TBL_COMP_FUNC(k1,k2) LAB_ChunkPosComp(k1, k2)

#define LAB_CHUNK_TBL_EMPTY_FUNC(e)    ((e)->chunk == NULL)

#define LAB_CHUNK_TBL_CALLOC           LAB_Calloc
#define LAB_CHUNK_TBL_FREE             LAB_Free

#define LAB_CHUNK_TBL_LOAD_NUM         3
#define LAB_CHUNK_TBL_LOAD_DEN         4
#define LAB_CHUNK_TBL_GROW_FACTOR      2
#define LAB_CHUNK_TBL_INITIAL_CAPACITY 16

#define LAB_CHUNK_TBL_CACHE_LAST       1


#define HTL_PARAM LAB_CHUNK_TBL
#include "HTL_hasharray.t.h"
#undef HTL_PARAM
#endif


#define LAB_CHUNKPOS_QUEUE_NAME     LAB_ChunkPosQueue
#define LAB_CHUNKPOS_QUEUE_TYPE     LAB_ChunkPos
#define LAB_CHUNKPOS_QUEUE_CAPACITY LAB_MAX_LOAD_CHUNK

#define HTL_PARAM LAB_CHUNKPOS_QUEUE
#include "HTL_queue.t.h"
#undef HTL_PARAM

/*
#define LAB_CHUNKPOS2_QUEUE_NAME     LAB_ChunkPos2Queue
#define LAB_CHUNKPOS2_QUEUE_TYPE     LAB_ChunkPos
#define LAB_CHUNKPOS2_QUEUE_CAPACITY 128

#define HTL_PARAM LAB_CHUNKPOS2_QUEUE
#include "HTL_queue.t.h"
#undef HTL_PARAM
*/


/*typedef struct LAB_LightUpdate
{
    int x, y, z;
} LAB_LightUpdate;

#define LAB_LIGHT_UPDATE_QUEUE_NAME LAB_LightUpdateQueue
#define LAB_LIGHT_UPDATE_QUEUE_TYPE LAB_LightUpdate
#define LAB_LIGHT_UPDATE_QUEUE_CAPACITY (16*16*16*4)

#define HTL_PARAM LAB_LIGHT_UPDATE_QUEUE
#include "HTL_queue.t.h"
#undef HTL_PARAM*/


typedef struct LAB_World LAB_World;

typedef void(LAB_ChunkViewer)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
typedef bool(LAB_ChunkKeeper)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
typedef void(LAB_ChunkUnlinker)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);


/*typedef struct LAB_ChunkEntry
{
    int x, y, z;
    LAB_Chunk* chunk;
} LAB_ChunkEntry;*/

typedef struct LAB_World
{
    LAB_ChunkGenerator* chunkgen;
    void*               chunkgen_user;


    LAB_ChunkViewer* chunkview;
    void*            chunkview_user;

    LAB_ChunkKeeper* chunkkeep;
    void*            chunkkeep_user;

    LAB_ChunkUnlinker* chunkunlink;
    void*              chunkunlink_user;

    LAB_ChunkTBL chunks;

    LAB_ChunkPosQueue gen_queue;
    //LAB_ChunkPos2Queue update_queue;

    size_t max_gen,
           max_update;

    //LAB_LightUpdateQueue light_queue;
} LAB_World;


typedef unsigned LAB_ChunkPeekType;
#define LAB_CHUNK_EXISTING       0
#define LAB_CHUNK_GENERATE       1
#define LAB_CHUNK_GENERATE_LATER 2




/**
 *  Init world, return 0 on error
 */
int  LAB_ConstructWorld(LAB_World* world);
/**
 *  Destruct world
 */
void LAB_DestructWorld(LAB_World* world);


/**
 *  Get chunk, create the chunk, when not existing, depending on flags
 *  Return chunk or NULL if chunk was not generated
 */
LAB_Chunk* LAB_GetChunk(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags);

/**
 *
 */
LAB_DEPRECATED("use LAB_GetChunkNeighbors instead")
void LAB_GetChunkNeighborhood(LAB_World* world, LAB_Chunk* chunks[27], int x, int y, int z, LAB_ChunkPeekType flags);

void LAB_GetChunkNeighbors(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27]);


/**
 *  Refresh a chunk by notifying the view
 */
//void LAB_NotifyChunk(LAB_World* world, int x, int y, int z);
void LAB_UpdateChunk(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
void LAB_UpdateChunkAt(LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update);
//void LAB_NotifyChunkLater(LAB_World* world, int x, int y, int z/*, LAB_ChunkPeekType flags*/);
void LAB_UpdateChunkLater(LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
void LAB_UpdateChunkLaterAt(LAB_World* world, int x, int y, int z, LAB_ChunkUpdate update);

LAB_Block* LAB_GetBlock(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags);
void LAB_SetBlock(LAB_World* world, int x, int y, int z, LAB_ChunkPeekType flags, LAB_Block* block);
void LAB_FillBlocks(LAB_World* world, int x0, int y0, int z0, int x1, int y1, int z1, LAB_ChunkPeekType flags, LAB_Block* block);


// dir should be an unit vector
int LAB_TraceBlock(LAB_World* world, int max_distance, float vpos[3], float dir[3], LAB_ChunkPeekType flags, unsigned block_flags,
                   /*out*/ int target[3],/*out*/ int prev[3],/*out*/ float hit[3]);


void LAB_WorldTick(LAB_World* world, uint32_t delta_ms);



LAB_INLINE LAB_Chunk* LAB_GetNeighborhoodRef(LAB_Chunk*const neighborhood[27], int x, int y, int z, int* /*out*/ index);
LAB_INLINE LAB_Block* LAB_GetNeighborhoodBlock(LAB_Chunk*const neighborhood[27], int x, int y, int z);
LAB_INLINE LAB_Color LAB_GetNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, int face, LAB_Color default_color);
LAB_INLINE uint32_t LAB_World_PeekFlags3x3(LAB_Chunk* chunk, int x, int y, int z, unsigned flag);








/**
 *  the origin of x,y,z is at (0,0,0) of the chunk at [1+3+3*3]
 */
LAB_ALWAYS_INLINE LAB_INLINE
LAB_Chunk* LAB_GetNeighborhoodRef(LAB_Chunk*const neighborhood[27], int x, int y, int z, int* /*out*/ index)
{
    LAB_ASSUME(x >= -16 && x < 32);
    LAB_ASSUME(y >= -16 && y < 32);
    LAB_ASSUME(z >= -16 && z < 32);

    int cx, cy, cz,  ix, iy, iz;

    cx = (x+16) >> LAB_CHUNK_SHIFT;
    ix = LAB_CHUNK_X(x & LAB_CHUNK_MASK);

    cy = 3*((y+16) >> LAB_CHUNK_SHIFT);
    iy = LAB_CHUNK_Y(y & LAB_CHUNK_MASK);

    cz = 3*3*((z+16) >> LAB_CHUNK_SHIFT);
    iz = LAB_CHUNK_Z(z & LAB_CHUNK_MASK);

    *index = ix+iy+iz;
    return neighborhood[cx+cy+cz];
}

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Block* LAB_GetNeighborhoodBlock(LAB_Chunk*const neighborhood[27], int x, int y, int z)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
    //if(LAB_UNLIKELY(chunk == NULL)) return &LAB_BLOCK_OUTSIDE;
    if(chunk == NULL) return &LAB_BLOCK_OUTSIDE;
    return chunk->blocks[block_index];
}

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_GetNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, int face, LAB_Color default_color)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);

    //if(LAB_UNLIKELY(chunk == NULL)) return LAB_RGB(255, 255, 255);
    if(chunk == NULL) return default_color;
#if LAB_DIRECTIONAL_LIGHT == 0
    return chunk->light[block_index];
#else
    return chunk->light[block_index].faces[face];
#endif
}

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_GetVisualNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, int face, LAB_Color default_color)
{
#if LAB_DIRECTIONAL_LIGHT == 0
    return LAB_GetNeighborhoodLight(neighborhood, x, y, z, face, default_color);
#else
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);

    //if(LAB_UNLIKELY(chunk == NULL)) return LAB_RGB(255, 255, 255);
    if(chunk == NULL) return default_color;

    /**
     *    1
     *  2 3 4
     *    5
    **/
    LAB_Color c = 0;

    int j = 0;
    for(int i = 0; i < 6; ++i)
    {
        if(i != (face^1))
        {
            LAB_Color cf = chunk->light[block_index].faces[i];
            if(i == face)
            {
                //cf = LAB_SubColor(cf, 0x0f0f0f0f);
                cf = LAB_MulColor2_Saturate(cf);
                cf = LAB_MulColor2_Saturate(cf);
            }
            c = LAB_MaxColor(c, cf);
        }
    }

    return c;
#endif
}


/*LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
bool LAB_SetNeighborhoodLight(LAB_Chunk* neighborhood[27], int x, int y, int z, int face, LAB_Color color)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);

    if(chunk == NULL) return 0;
    chunk->light[block_index].faces[face] = color;
    return 1;
}*/

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
uint32_t LAB_World_PeekFlags3x3(LAB_Chunk* chunk, int x, int y, int z, unsigned flag)
{
    uint32_t bitset = 0, curbit = 1;
    for(int zz = z-1; zz <= z+1; ++zz)
    for(int yy = y-1; yy <= y+1; ++yy)
    for(int xx = x-1; xx <= x+1; ++xx)
    {
        if(xx >= 0 && xx < 16 &&
           yy >= 0 && yy < 16 &&
           zz >= 0 && zz < 16)
        {
            if(chunk->blocks[xx|yy<<4|zz<<8]->flags & flag)
                bitset |= curbit;
        }
        curbit <<= 1;
    }
    return bitset;
}
