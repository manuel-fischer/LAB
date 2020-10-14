#pragma once

#include "LAB_attr.h"
#include "LAB_chunk.h"

typedef struct LAB_RelativeChunkPlacer
{
    int ox, oy, oz; // coordinates of the chunk origin in the relative coordinate system
                    // chunk x in [ox, ox+16), etc.
    LAB_Chunk* chunk;
} LAB_RelativeChunkPlacer;

typedef LAB_RelativeChunkPlacer LAB_Placer;


LAB_INLINE bool LAB_Placer_IsInside(LAB_Placer* p, int x, int y, int z, int w, int h, int d);
LAB_INLINE void LAB_Placer_SetBlock(LAB_Placer* p, int x, int y, int z, LAB_Block* block);


LAB_INLINE bool LAB_Placer_IsInside(LAB_Placer* p, int x, int y, int z, int w, int h, int d)
{
    return (x <= p->ox+16 && x+w >= p->ox)
        && (y <= p->oy+16 && y+h >= p->oy)
        && (z <= p->oz+16 && z+d >= p->oz);
}

LAB_INLINE void LAB_Placer_SetBlock(LAB_Placer* p, int x, int y, int z, LAB_Block* block)
{
    unsigned xx = x - p->ox;
    unsigned yy = y - p->oy;
    unsigned zz = z - p->oz;

    if(xx<16 && yy<16 && zz<16)
        p->chunk->blocks[xx|yy<<4|zz<<8] = block;
}

LAB_INLINE void LAB_Placer_SetBlockIfBlock(LAB_Placer* p, int x, int y, int z, LAB_Block* block, LAB_Block* replace)
{
    unsigned xx = x - p->ox;
    unsigned yy = y - p->oy;
    unsigned zz = z - p->oz;

    if(xx<16 && yy<16 && zz<16)
    {
        LAB_Block** b = &p->chunk->blocks[xx|yy<<4|zz<<8];
        if(*b == replace)
            *b = block;
    }
}
