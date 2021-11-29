#pragma once

#include "LAB_block.h"
#include "LAB_light_node.h"
#include "LAB_color.h"
#include "LAB_crammed_chunk_pos_set.h"

typedef struct LAB_World LAB_World;

enum LAB_ChunkUpdate_Enum
{
    LAB_CHUNK_UPDATE_LIGHT = 1, // light changed, usually by neighboring chunk
    LAB_CHUNK_UPDATE_BLOCK_ = 2, // block changed, usually in combination with light
    LAB_CHUNK_UPDATE_BLOCK = 2+1,
    LAB_CHUNK_UPDATE_LOCAL = 2+0*4, // the chunk itself has changed -> higher priority
                                // when only the neighboring chunk had changed, the update
                                // is not propagated (TODO)
};
typedef int LAB_ChunkUpdate;

#define LAB_CHUNK_SHIFT  4
#define LAB_CHUNK_SIZE   (1 << LAB_CHUNK_SHIFT)
#define LAB_CHUNK_LENGTH (LAB_CHUNK_SIZE*LAB_CHUNK_SIZE*LAB_CHUNK_SIZE)
#define LAB_CHUNK_MASK   (LAB_CHUNK_SIZE - 1)


#define LAB_CHUNK_X(x) (x)
#define LAB_CHUNK_Y(y) (LAB_CHUNK_SIZE*(y))
#define LAB_CHUNK_Z(z) (LAB_CHUNK_SIZE*LAB_CHUNK_SIZE*(z))
#define LAB_CHUNK_OFFSET(x, y, z) ((x) + LAB_CHUNK_SIZE*((y) + LAB_CHUNK_SIZE*(z)))

#ifdef NDEBUG
#define LAB_FIELD(bits) :bits
#else
#define LAB_FIELD(bits)
#endif

typedef struct LAB_Chunk
{
    LAB_Block*     blocks[LAB_CHUNK_LENGTH];
    LAB_LightNode  light[LAB_CHUNK_LENGTH];
    unsigned int dirty LAB_FIELD(8),
                 modified LAB_FIELD(1),
                 light_generated LAB_FIELD(1),
                 empty LAB_FIELD(1), // usually set to 0, it is set to 1, if all blocks are air, TODO: this is tested, when the
                          // the chunk is added to the list
                          // this bit is only used for optimizations, it might not be set if all blocks are air
                          // (when changed to this after generation)
                          // alternative: air-counter, decreases if air got replaced by another block, increases if other block is replaced by air
                          // 0 -> definitely empty
                          // 1 -> might be empty, don't care
                 generated LAB_FIELD(1), // chunk is completely generated and in a stable state
                 pseudo LAB_FIELD(1); // chunk does not exist, this is a dummy chunk
    LAB_CrammedChunkPosSet dirty_blocks; // used for updating light, set by world
    LAB_CrammedChunkPosSet relit_blocks; // wich blocks are relit.
    int age;
    struct LAB_Chunk* _neighbors[6];
    void* view_user; // managed by the view, non owning pointer, set to NULL when
} LAB_Chunk;

typedef LAB_Chunk* (LAB_ChunkGenerator)(void* user, LAB_Chunk* chunk, int x, int y, int z);

/**
 *  Create chunk filled with fill_block,
 *  Neighbors are all NULL
 *  Return NULL on failure
 */
LAB_Chunk* LAB_CreateChunk(void);

/**
 *  Unlink neighbors and free chunk
 */
void LAB_DestroyChunk(LAB_Chunk* chunk);


void LAB_FillChunk(LAB_Chunk* chunk, LAB_Block* fill_block);


LAB_INLINE
LAB_Chunk* LAB_Chunk_Access(LAB_Chunk* chunk)
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