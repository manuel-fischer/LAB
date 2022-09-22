#pragma once
#include "LAB_chunk.h"
#include "LAB_direction.h"


typedef struct LAB_BlockNbHood
{
    LAB_Chunk_Blocks* bufs[27];
} LAB_BlockNbHood, LAB_BlockNbHood_Mut;

typedef struct LAB_LightNbHood
{
    LAB_Chunk_Light* bufs[27];
} LAB_LightNbHood, LAB_LightNbHood_Mut;


void LAB_GetChunkNeighborsNone(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27]);
void LAB_GetChunkNeighbors(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27]);

void LAB_BlockNbHood_GetRead(LAB_Chunk*const chunks[27], LAB_BlockNbHood* out);
bool LAB_BlockNbHood_GetWrite(LAB_Chunk*const chunks[27], LAB_BlockNbHood_Mut* out); // return false if allocation failed

void LAB_LightNbHood_GetRead(LAB_Chunk*const chunks[27], LAB_LightNbHood* out);
bool LAB_LightNbHood_GetWrite(LAB_Chunk*const chunks[27], LAB_LightNbHood_Mut* out); // return false if allocation failed


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
void LAB_Neighborhood_Index(int x, int y, int z, int* chunk, int* block)
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

    *chunk = cx+cy+cz;
    *block = ix+iy+iz;
#if 0 /*fun*/
    *block ^= (rand()&rand()&rand()&rand()&rand()&rand()&rand()&0xfff);
#endif
}

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_BlockID _Atomic * LAB_BlockNbHood_RefBlock(LAB_BlockNbHood* n, int x, int y, int z)
{
    int chunk, block;
    LAB_Neighborhood_Index(x, y, z, &chunk, &block);
    return &n->bufs[chunk]->blocks[block];
}


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_LightNode* LAB_LightNbHood_RefLightNode(LAB_LightNbHood* n, int x, int y, int z)
{
    int chunk, block;
    LAB_Neighborhood_Index(x, y, z, &chunk, &block);
    return &n->bufs[chunk]->light[block];
}


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_GetVisualNeighborhoodLight(LAB_LightNbHood* n, int x, int y, int z, LAB_Dir face)
{
    LAB_LightNode* ln = LAB_LightNbHood_RefLightNode(n, x, y, z);

    LAB_Color c = 0;
    LAB_Color max = 0;

    int mask =        1  << (face>>1);
    int bit  = !(face&1) << (face>>1);
    LAB_UNROLL(8)
    for(int i = 0; i < 8; ++i)
    {
        LAB_Color cf = ln->quadrants[i];
        if((i & mask) != bit)
        {
            cf = LAB_MixColor50(cf, 0);
        }
        max = LAB_MaxColor(max, cf);
        c = LAB_AddColor(c, cf >> 2 & 0x3f3f3f3f);
    }

    return max;
}