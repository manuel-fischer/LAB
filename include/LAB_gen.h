#pragma once

#include "LAB_attr.h"
#include "LAB_chunk.h"
#include "LAB_blocks.h"


typedef struct LAB_LandscapeMask
{
    uint64_t bitset[(16*16*16)/64];
} LAB_LandscapeMask;
#define LAB_LandscapeMask_Create(mask) memset(mask, 0, sizeof *(mask))
#define LAB_LandscapeMask_Create_Expr(mask, refindex, expr) do { \
    (refindex) = 0; \
    for(size_t LAB_LandscapeMask_wp = 0; LAB_LandscapeMask_wp < LAB_LEN((mask)->bitset); ++LAB_LandscapeMask_wp) \
    { \
        uint64_t LAB_LandscapeMask_word = 0; \
        uint64_t LAB_LandscapeMask_shift = 1; \
        for(size_t LAB_LandscapeMask_wi = 0; LAB_LandscapeMask_wi < 64; ++LAB_LandscapeMask_wi) \
        { \
            if(expr) \
                LAB_LandscapeMask_word |= LAB_LandscapeMask_shift; \
            LAB_LandscapeMask_shift <<= 1; \
            ++(refindex); \
        } \
        (mask)->bitset[LAB_LandscapeMask_wp] = LAB_LandscapeMask_word; \
    } \
} while(0)

#define LAB_LandscapeMask_Toggle(mask, index) ((mask)->bitset[(index) >> 6] ^= (uint64_t)1 << ((index) & 63))
#define LAB_LandscapeMask_Clear(mask, index) ((mask)->bitset[(index) >> 6] &= ~((uint64_t)1 << ((index) & 63)))
#define LAB_LandscapeMask_Set(mask, index) ((mask)->bitset[(index) >> 6] |= (uint64_t)1 << ((index) & 63))
#define LAB_LandscapeMask_Get(mask, index) ((mask)->bitset[(index) >> 6] & (uint64_t)1 << ((index) & 63))



typedef uint8_t LAB_PlacementPriority;

typedef struct LAB_PlacementPriorityBuffer
{
    LAB_PlacementPriority buffer[16*16*16];
} LAB_PlacementPriorityBuffer;
#define LAB_PlacementPriorityBuffer_Create(buffer) memset(buffer, 0, sizeof *(buffer))

typedef struct LAB_RelativeChunkPlacer
{
    int ox, oy, oz; // coordinates of the chunk origin in the relative coordinate system
                    // chunk x in [ox, ox+16), etc.
    LAB_Chunk_Blocks* chunk_blocks;
    LAB_PlacementPriorityBuffer* priority;
    LAB_LandscapeMask* landscape;
} LAB_RelativeChunkPlacer;

typedef LAB_RelativeChunkPlacer LAB_Placer;

LAB_INLINE LAB_Placer LAB_Placer_Offset(LAB_Placer* p, int dx, int dy, int dz);
LAB_INLINE bool LAB_Placer_IsInside(LAB_Placer* p, int x, int y, int z, int w, int h, int d);
LAB_INLINE bool LAB_Placer_Translate(LAB_Placer* p, int x, int y, int z, LAB_OUT size_t* index);

LAB_INLINE bool LAB_Placer_GetLandscapeBit(LAB_Placer* p, int x, int y, int z);
LAB_INLINE LAB_BlockID LAB_Placer_GetBlock(LAB_Placer* p, int x, int y, int z);
LAB_INLINE LAB_PlacementPriority LAB_Placer_GetPriority(LAB_Placer* p, int x, int y, int z);
LAB_INLINE bool LAB_Placer_HasPriority(LAB_Placer* p, int x, int y, int z, LAB_PlacementPriority priority);
LAB_INLINE void LAB_Placer_SetBlock(LAB_Placer* p, int x, int y, int z, LAB_BlockID block);
LAB_INLINE void LAB_Placer_SetBlockIfBlock(LAB_Placer* p, int x, int y, int z, LAB_BlockID block, LAB_BlockID replace);
LAB_INLINE void LAB_Placer_SetBlockIfAny(LAB_Placer* p, int x, int y, int z, LAB_BlockID block, uint32_t tags);
LAB_INLINE void LAB_Placer_SetBlockIfAll(LAB_Placer* p, int x, int y, int z, LAB_BlockID block, uint32_t tags);
LAB_INLINE void LAB_Placer_SetBlockWithPriority(LAB_Placer* p, int x, int y, int z, LAB_BlockID block, LAB_PlacementPriority priority);




LAB_INLINE LAB_Placer LAB_Placer_Offset(LAB_Placer* p, int dx, int dy, int dz)
{
    LAB_Placer r;
    r.ox = p->ox - dx;
    r.oy = p->oy - dy;
    r.oz = p->oz - dz;
    r.chunk_blocks = p->chunk_blocks;
    r.priority = p->priority;
    r.landscape = p->landscape;
    return r;
}

LAB_INLINE bool LAB_Placer_IsInside(LAB_Placer* p, int x, int y, int z, int w, int h, int d)
{
    return (x <= p->ox+16 && x+w >= p->ox)
        && (y <= p->oy+16 && y+h >= p->oy)
        && (z <= p->oz+16 && z+d >= p->oz);
}



LAB_INLINE bool LAB_Placer_Translate(LAB_Placer* p, int x, int y, int z, LAB_OUT size_t* index)
{
    unsigned xx = x - p->ox;
    unsigned yy = y - p->oy;
    unsigned zz = z - p->oz;

    *index = xx|yy<<4|zz<<8;
    return xx<16 && yy<16 && zz<16;
}



LAB_INLINE bool LAB_Placer_GetLandscapeBit(LAB_Placer* p, int x, int y, int z)
{
    size_t index;
    return LAB_Placer_Translate(p, x, y, z, &index) && LAB_LandscapeMask_Get(p->landscape, index);
}

LAB_INLINE LAB_BlockID LAB_Placer_GetBlock(LAB_Placer* p, int x, int y, int z)
{
    size_t index;
    if(!LAB_Placer_Translate(p, x, y, z, &index)) return LAB_BID_OUTSIDE;
    return p->chunk_blocks->blocks[index];
}

LAB_INLINE LAB_PlacementPriority LAB_Placer_GetPriority(LAB_Placer* p, int x, int y, int z)
{
    size_t index;
    if(!LAB_Placer_Translate(p, x, y, z, &index)) return (LAB_PlacementPriority)-1;
    return p->priority->buffer[index];
}

LAB_INLINE bool LAB_Placer_HasPriority(LAB_Placer* p, int x, int y, int z, LAB_PlacementPriority priority)
{
    size_t index;
    return LAB_Placer_Translate(p, x, y, z, &index) && p->priority->buffer[index] == priority;
}

LAB_INLINE void LAB_Placer_SetBlock(LAB_Placer* p, int x, int y, int z, LAB_BlockID block)
{
    size_t index;
    if(LAB_Placer_Translate(p, x, y, z, &index))
        p->chunk_blocks->blocks[index] = block;
}

LAB_INLINE void LAB_Placer_SetBlockIfBlock(LAB_Placer* p, int x, int y, int z, LAB_BlockID block, LAB_BlockID replace)
{
    size_t index;
    if(!LAB_Placer_Translate(p, x, y, z, &index)) return;

    LAB_BlockID _Atomic* b = &p->chunk_blocks->blocks[index];
    if(*b == replace)
        *b = block;
}

LAB_INLINE void LAB_Placer_SetBlockIfAny(LAB_Placer* p, int x, int y, int z, LAB_BlockID block, uint32_t tags)
{
    size_t index;
    if(!LAB_Placer_Translate(p, x, y, z, &index)) return;

    LAB_BlockID _Atomic* b = &p->chunk_blocks->blocks[index];
    if(LAB_BlockP(*b)->tags&tags)
        *b = block;
}

LAB_INLINE void LAB_Placer_SetBlockIfAll(LAB_Placer* p, int x, int y, int z, LAB_BlockID block, uint32_t tags)
{
    size_t index;
    if(!LAB_Placer_Translate(p, x, y, z, &index)) return;

    LAB_BlockID _Atomic* b = &p->chunk_blocks->blocks[index];
    if((LAB_BlockP(*b)->tags&tags) == tags)
        *b = block;
}

LAB_INLINE void LAB_Placer_SetBlockWithPriority(LAB_Placer* p, int x, int y, int z, LAB_BlockID block, LAB_PlacementPriority priority)
{
    size_t index;
    if(!LAB_Placer_Translate(p, x, y, z, &index)) return;

    if(p->priority->buffer[index] > priority) return;

    p->chunk_blocks->blocks[index] = block;
    p->priority->buffer[index] = priority;
}