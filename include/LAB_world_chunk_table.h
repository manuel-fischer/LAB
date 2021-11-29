#pragma once

// The world datastructure, that are observers & low level modifiers

#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_chunk.h"
#include "LAB_htl_config.h"

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
#include "HTL/hasharray.t.h"
#undef HTL_PARAM




#define LAB_CHUNKPOS_QUEUE_NAME     LAB_ChunkPosQueue
#define LAB_CHUNKPOS_QUEUE_TYPE     LAB_ChunkPos

#define HTL_PARAM LAB_CHUNKPOS_QUEUE
#include "HTL/queue.t.h"
#undef HTL_PARAM