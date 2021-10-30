#pragma once

#include "LAB_crammed_chunk_pos_set.h"
#include "LAB_chunk.h"
#include "LAB_stdinc.h"
#include "LAB_debug.h"

#include "LAB_check.h"
#include "LAB_opt.h"
#include "LAB_perf_info.h"
#include "LAB_htl_config.h"

#include "LAB_builtin_blocks.h"

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

//#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)+1)
//#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)+2)
#define LAB_PRELOAD_CHUNK(chunk_dist) ((chunk_dist)+3)
#define LAB_KEEP_CHUNK(chunk_dist)    ((chunk_dist)*2+3)



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

#define HTL_PARAM LAB_CHUNK_TBL
#include "HTL_hasharray.t.h"
#undef HTL_PARAM




#define LAB_CHUNKPOS_QUEUE_NAME     LAB_ChunkPosQueue
#define LAB_CHUNKPOS_QUEUE_TYPE     LAB_ChunkPos

#define HTL_PARAM LAB_CHUNKPOS_QUEUE
#include "HTL_queue.t.h"
#undef HTL_PARAM



typedef struct LAB_World LAB_World;

typedef void(LAB_IView_ChunkViewer)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z, LAB_ChunkUpdate update);
typedef bool(LAB_IView_ChunkKeeper)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
typedef void(LAB_IView_ChunkUnlinker)(void* user, LAB_World* world, LAB_Chunk* chunk, int x, int y, int z);
typedef void(LAB_IView_Position)(void* user, LAB_World* world, LAB_OUT double xyz[3]);

typedef struct LAB_IView
{
    LAB_IView_ChunkViewer* chunkview;
    LAB_IView_ChunkKeeper* chunkkeep;
    LAB_IView_ChunkUnlinker* chunkunlink;
    LAB_IView_Position* position;
} LAB_IView;

/*typedef struct LAB_ChunkEntry
{
    int x, y, z;
    LAB_Chunk* chunk;
} LAB_ChunkEntry;*/

typedef struct LAB_World
{
    LAB_ChunkGenerator* chunkgen;
    void*               chunkgen_user;


    const LAB_IView* view;
    void*            view_user;

    LAB_ChunkTBL chunks;

    LAB_ChunkPosQueue gen_queue;
    //LAB_ChunkPos2Queue update_queue;

    size_t max_gen,
           max_update;

    //LAB_LightUpdateQueue light_queue;

    LAB_PerfInfo* perf_info;
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
//LAB_INLINE LAB_Color LAB_GetNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, int face, LAB_Color default_color);
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

/*LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_GetNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, int quadrant, LAB_Color default_color)
{
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);

    //if(LAB_UNLIKELY(chunk == NULL)) return LAB_RGB(255, 255, 255);
    if(chunk == NULL) return default_color;
#if LAB_DIRECTIONAL_LIGHT == 0
    return chunk->light[block_index];
#else
    return chunk->light[block_index].quadrants[face];
#endif
}*/

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_GetVisualNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, int face, LAB_Color default_color)
{
#if LAB_DIRECTIONAL_LIGHT == 0
    return LAB_GetNeighborhoodLight(neighborhood, x, y, z, face, default_color);
#else
#if 0
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

#if 1
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
    if(chunk == NULL) return default_color;


    LAB_Color c = 0;
    LAB_Color max = 0;

    int mask =        1  << (face>>1);
    int bit  = !(face&1) << (face>>1);
    LAB_UNROLL(8)
    for(int i = 0; i < 8; ++i)
    {
        LAB_Color cf = chunk->light[block_index].quadrants[i];
        if((i & mask) != bit)
        {
            cf = LAB_MixColor50(cf, 0);
        }
        max = LAB_MaxColor(max, cf);
        c = LAB_AddColor(c, cf >> 2 & 0x3f3f3f3f);
        //c = LAB_AddColor(c, cf >> 1 & 0x7f7f7f7f);
    }

    return max; //c;// LAB_MinColor(c, max);
#else
    int block_index;
    LAB_Chunk* chunk;
    chunk = LAB_GetNeighborhoodRef(neighborhood, x, y, z, &block_index);
    if(chunk == NULL) return default_color;

    int a =        1  <<  (face>>1);
    int d = !(face&1) <<  (face>>1);
    int u =        1  << ((face>>1) == 0);
    int v = 7-u-a; //7 ^ u ^ ax;

    const LAB_Color* q = chunk->light[block_index].quadrants;

    LAB_Color c0 = LAB_MaxColor(q[d     ], q[d +u  ]);
    LAB_Color c1 = LAB_MaxColor(q[d +u  ], q[d +u+v]);
    LAB_Color c2 = LAB_MaxColor(q[d +u+v], q[d   +v]);
    LAB_Color c3 = LAB_MaxColor(q[d   +v], q[d     ]);

    return LAB_MixColor4x25(c0, c1, c2, c3);

#endif
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
