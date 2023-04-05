#pragma once

#include "LAB_stdinc.h"
#include "LAB_block.h"
#include "LAB_chunk_pos.h"
#include "LAB_light_node.h"
#include "LAB_color.h"
#include "LAB_crammed_chunk_pos_set.h"

#include "LAB_chunk_update.h"
#include "LAB_coords.h"

typedef struct LAB_World LAB_World;


#ifdef NDEBUG
#define LAB_FIELD(bits) :bits
#else
#define LAB_FIELD(bits)
#endif


#define LAB_CHUNK_MODIFIED          1

#define LAB_CHUNK_BLOCKS            2
#define LAB_CHUNK_BLOCKS_GENERATED  4
#define LAB_CHUNK_LIGHT             8
#define LAB_CHUNK_LIGHT_GENERATED  16

typedef struct LAB_Chunk_Head
{
    unsigned flags;
    int age;
    struct LAB_Chunk* _neighbors[6];

} LAB_Chunk_Head;

typedef struct LAB_Chunk_Blocks
{
    LAB_BlockID blocks[LAB_CHUNK_LENGTH];
} LAB_Chunk_Blocks;

typedef struct LAB_Chunk_Light
{
    LAB_LightNode light[LAB_CHUNK_LENGTH];
} LAB_Chunk_Light;

extern LAB_Chunk_Blocks LAB_chunk_empty_blocks_air;
extern LAB_Chunk_Blocks LAB_chunk_empty_blocks_outside;
extern LAB_Chunk_Light  LAB_chunk_empty_light_dark;
extern LAB_Chunk_Light  LAB_chunk_empty_light_sunlight;


typedef struct LAB_ChunkStats
{
    size_t executed_updates;

} LAB_ChunkStats;

typedef struct LAB_Chunk
{
    LAB_Chunk_Blocks* buf_blocks; // corresponds to AIR if NULL
    LAB_Chunk_Light*  buf_light;  // corresponds to complete darkness if NULL

    LAB_ChunkPos pos;

    atomic_bool modified;
         //light_generated,
         //empty, // usually set to 0, it is set to 1, if all blocks are air, TODO: this is tested, when the
                // the chunk is added to the list
                // this bit is only used for optimizations, it might not be set if all blocks are air
                // (when changed to this after generation)
                // alternative: air-counter, decreases if air got replaced by another block, increases if other block is replaced by air
                // 0 -> definitely empty
                // 1 -> might be empty, don't care
         //generated, // chunk is completely generated and in a stable state
         //pseudo; // chunk does not exist, this is a dummy chunk
         //sky_light; // when empty, this marks that the chunk only has skylight
                    // but at every block

    //_Atomic unsigned int dirty; 
    atomic_bool dirty; // TODO remove

    _Atomic LAB_CrammedChunkPosSet dirty_blocks; // used for updating light, set by world
    _Atomic LAB_CrammedChunkPosSet relit_blocks; // wich blocks are relit.
    LAB_CrammedChunkPosSet active_blocks;

    _Atomic int dirty_neighbors;
    _Atomic int relit_neighbors;
    int active_neighbors;


    atomic_bool generated;
    atomic_bool light_generated;


    int age;
    struct LAB_Chunk*_Atomic _neighbors[6];
    void* view_user; // managed by the view, non owning pointer, set to NULL when

    // used by the world server to avoid accessing the same chunk from different
    // threads, note that usually chunks with the same parity do not collide,
    // only if it is the same chunk

    // locked by the server mutex
//    bool is_accessed; // TODO remove
    int access_mode; // -1: write, 0: available, positive: read count

    LAB_ChunkStats stats;

    //atomic_size_t enqueue_count; // number of entries in the world task queue
    // if queue_prev is NULL, the element is not enqueued
    struct LAB_Chunk** queue_prev,* queue_next;
    //size_t reenqueue;

    // Not initialized
    size_t queue_timestamp;
    unsigned int update_stage;
    int update_priority;


} LAB_Chunk;

LAB_STATIC_ASSUME(ATOMIC_POINTER_LOCK_FREE == 2, "Pointers should always be lock free");
LAB_STATIC_ASSUME(sizeof(LAB_Chunk*) == sizeof(LAB_Chunk*_Atomic), "Atomic pointers should have the same size as normal pointers");

typedef bool (LAB_ChunkGenerator)(void* user, LAB_Chunk* chunk, int x, int y, int z);


void LAB_InitEmptyChunks(void);


/**
 *  Create uninitialized chunk
 *  Neighbors are all NULL
 *  Return NULL on failure
 */
LAB_Chunk* LAB_CreateChunk(LAB_ChunkPos pos);

/**
 *  Unlink neighbors and free chunk
 */
void LAB_DestroyChunk(LAB_Chunk* chunk);

void LAB_UnlinkChunk(LAB_Chunk* chunk);
void LAB_DestroyChunk_Unlinked(LAB_Chunk* chunk);

void LAB_Chunk_Blocks_Fill(LAB_Chunk_Blocks* blocks, LAB_BlockID fill_block);

LAB_INLINE
void LAB_Chunk_FillGenerate(LAB_Chunk* chunk, LAB_BlockID fill_block)
{
    LAB_ASSERT(!chunk->generated);
    LAB_ASSERT(chunk->buf_blocks);
    LAB_Chunk_Blocks_Fill(chunk->buf_blocks, fill_block);
}


// Multi threaded access
LAB_INLINE
LAB_Chunk* LAB_Chunk_Access(LAB_Chunk* chunk/*, LAB_ChunkFlags flags*/)
{
    return chunk && chunk->generated ? chunk : NULL;
}


LAB_INLINE
LAB_Chunk* LAB_Chunk_Neighbor(LAB_Chunk* chunk, int face)
{
    return chunk ? LAB_Chunk_Access(chunk->_neighbors[face]) : NULL;
}

LAB_INLINE
void LAB_Chunk_Connect(LAB_Chunk* a, int face, LAB_Chunk* b)
{
    LAB_ASSERT(a);
    LAB_ASSERT(b);
    LAB_ASSERT(!a->_neighbors[face  ] || a->_neighbors[face  ] == b);
    LAB_ASSERT(!b->_neighbors[face^1] || b->_neighbors[face^1] == a);

    a->_neighbors[face  ] = b;
    b->_neighbors[face^1] = a;
}


void LAB_Chunk_LockRead(LAB_Chunk* chunk);
void LAB_Chunk_UnlockRead(LAB_Chunk* chunk);
void LAB_Chunk_LockWrite(LAB_Chunk* chunk);
void LAB_Chunk_UnlockWrite(LAB_Chunk* chunk);

// write in the center, read in the neighbors
void LAB_Chunk_LockNeighbors(LAB_Chunk* chunks[27]);
void LAB_Chunk_UnlockNeighbors(LAB_Chunk* chunks[27]);

LAB_Chunk_Blocks* LAB_Chunk_Blocks_Read(LAB_Chunk* chunk); // Does not modify chunk, return value should not be modified
LAB_Chunk_Blocks* LAB_Chunk_Blocks_Write(LAB_Chunk* chunk); // Modifies chunk, can fail
void LAB_Chunk_Blocks_Optimize(LAB_Chunk* chunk);
LAB_INLINE bool LAB_Chunk_IsEmpty(LAB_Chunk* chunk) { return chunk->buf_blocks == NULL; }

LAB_Chunk_Light* LAB_Chunk_Light_Read(LAB_Chunk* chunk); // Does not modify chunk, return value should not be modified
LAB_Chunk_Light* LAB_Chunk_Light_Write(LAB_Chunk* chunk); // Modifies chunk, can fail
void LAB_Chunk_Light_Optimize(LAB_Chunk* chunk);
LAB_Chunk_Light* LAB_Chunk_Light_Read_ByY(int y);

typedef enum LAB_LightFill
{
    LAB_LIGHT_FILL_DARK,
    LAB_LIGHT_FILL_SUNLIGHT,
    LAB_LIGHT_FILL_DATA
} LAB_LightFill;

LAB_INLINE LAB_LightFill LAB_Chunk_GetLightFill(LAB_Chunk* chunk)
{
    LAB_Chunk_Light* l = chunk->buf_light;
    return l == NULL ? LAB_LIGHT_FILL_DARK
         : l == &LAB_chunk_empty_light_dark ? LAB_LIGHT_FILL_DARK
         : l == &LAB_chunk_empty_light_sunlight ? LAB_LIGHT_FILL_SUNLIGHT
         : LAB_LIGHT_FILL_DATA;
}

// light: either &LAB_chunk_empty_* or allocated
void LAB_Chunk_SetLightBuf(LAB_Chunk* chunk, LAB_Chunk_Light* light);
