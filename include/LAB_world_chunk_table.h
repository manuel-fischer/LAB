#pragma once

// The world datastructure, that are observers & low level modifiers

#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_chunk.h"
#include "LAB_htl_config.h"
#include "LAB_chunk_pos.h"





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




/*#define LAB_CHUNKPOS_QUEUE_NAME     LAB_ChunkPosQueue
#define LAB_CHUNKPOS_QUEUE_TYPE     LAB_ChunkPos

#define HTL_PARAM LAB_CHUNKPOS_QUEUE
    #include "HTL/queue.t.h"
#undef HTL_PARAM*/

#define LAB_CHUNK_BUF_QUEUE_NAME     LAB_ChunkBufQueue
#define LAB_CHUNK_BUF_QUEUE_TYPE     LAB_Chunk*

#define HTL_PARAM LAB_CHUNK_BUF_QUEUE
    #include "HTL/queue.t.h"
#undef HTL_PARAM