#pragma once
#include "LAB_chunk.h"


LAB_INLINE void LAB_GetChunkNeighbors(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27]);
LAB_INLINE LAB_Chunk* LAB_GetNeighborhoodRef(LAB_Chunk*const neighborhood[27], int x, int y, int z, int* /*out*/ index);
LAB_INLINE LAB_Block* LAB_GetNeighborhoodBlock(LAB_Chunk*const neighborhood[27], int x, int y, int z);



LAB_INLINE
void LAB_GetChunkNeighbors(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27])
{
    LAB_ASSERT(center_chunk);
    LAB_ASSERT(LAB_Chunk_Access(center_chunk));

    #define NEIGHBOR(x, y, z) (chunks[(x)+3*(y)+9*(z)])
    #define NEIGHBOR_FACE(x, y, z, face) LAB_Chunk_Neighbor(NEIGHBOR(x, y, z), face)
    //#define NEIGHBOR_FACE(x, y, z, face)  (NEIGHBOR(x, y, z)?NEIGHBOR(x, y, z)->neighbors[face]:NULL)

    NEIGHBOR(1, 1, 1) = center_chunk;
    LAB_UNROLL(6)
    for(int f = 0; f < 6; ++f)
    {
        NEIGHBOR(1+LAB_OX(f), 1+LAB_OY(f), 1+LAB_OZ(f)) = NEIGHBOR_FACE(1, 1, 1, f);
    }


    int i = 0;
    for(int z = 0; z < 3; ++z)
    for(int y = 0; y < 3; ++y)
    for(int x = 0; x < 3; ++x, ++i)
    {
        LAB_ASSUME(i == x+3*y+9*z);
        if((i&1) && i != 1+3+9)
        {
            if(x == 1)
            {
                if(!(chunks[i] = NEIGHBOR_FACE(1, 1, z, 1<<1^0^y>>1)))
                     chunks[i] = NEIGHBOR_FACE(1, y, 1, 2<<1^0^z>>1);
            }
            else if(y == 1)
            {
                if(!(chunks[i] = NEIGHBOR_FACE(1, 1, z, 0<<1^0^x>>1)))
                     chunks[i] = NEIGHBOR_FACE(x, 1, 1, 2<<1^0^z>>1);
            }
            else
            {
                LAB_ASSUME(z == 1);

                if(!(chunks[i] = NEIGHBOR_FACE(1, y, 1, 0<<1^0^x>>1)))
                     chunks[i] = NEIGHBOR_FACE(x, 1, 1, 1<<1^0^y>>1);
            }
        }
    }

    i = 0;
    for(int z = 0; z < 3; z+=2, i+=6)
    for(int y = 0; y < 3; y+=2, i+=2)
    for(int x = 0; x < 3; x+=2, i+=2)
    {
        LAB_ASSUME(i == x+3*y+9*z);
        if(!(chunks[i] = NEIGHBOR_FACE(1, y, z, 0<<1^0^x>>1)))
        if(!(chunks[i] = NEIGHBOR_FACE(x, 1, z, 1<<1^0^y>>1)))
             chunks[i] = NEIGHBOR_FACE(x, y, 1, 2<<1^0^z>>1);

    }

    #ifndef NDEBUG

        for(int k = 0; k < 27; ++k)
            if(chunks[k]) LAB_ASSERT(LAB_Chunk_Access(chunks[k]));

    #endif

    #undef NEIGHBOR_FACE
    #undef NEIGHBOR
}



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
LAB_Color LAB_GetVisualNeighborhoodLight(LAB_Chunk*const neighborhood[27], int x, int y, int z, int face, LAB_Color default_color)
{
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
    }

    return max;
}