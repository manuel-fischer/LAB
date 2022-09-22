#include "LAB_chunk_neighborhood.h"

void LAB_GetChunkNeighborsNone(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27])
{
    memset(chunks, 0, sizeof(LAB_Chunk*)*27);
    chunks[1+3+7] = center_chunk;
}


void LAB_GetChunkNeighbors(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27])
{
    LAB_ASSERT(center_chunk);
    //LAB_ASSERT(LAB_Chunk_Access(center_chunk));

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


LAB_STATIC
bool LAB_BlockNbHood_Get(LAB_Chunk*const chunks[27], LAB_BlockNbHood* out, bool write_center)
{
    LAB_ASSERT(chunks[1+3+9]);
    for(int i=0; i<27; ++i)
    {
        if(chunks[i] == NULL)
        {
            LAB_ASSERT(i != 1+3+9);
            out->bufs[i] = &LAB_chunk_empty_blocks_outside;
        }
        else
        {
            if(write_center && i == 1+3+9)
            {
                out->bufs[i] = LAB_Chunk_Blocks_Write(chunks[i]);
                if(!out->bufs[i]) return false;
            }
            else
                out->bufs[i] = LAB_Chunk_Blocks_Read(chunks[i]);
        }
    }
    return true;
}


// TODO: codedup LAB_BlockNbHood_Get
LAB_STATIC
bool LAB_LightNbHood_Get(LAB_Chunk*const chunks[27], LAB_LightNbHood* out, bool write_center)
{
    LAB_ASSERT(chunks[1+3+9]);
    int ay = chunks[1+3+9]->pos.y - 1;
    int i = 0;
    for(int z=0; z<3; ++z)
    for(int y=0; y<3; ++y)
    for(int x=0; x<3; ++x, ++i)
    {
        if(chunks[i] == NULL)
        {
            LAB_ASSERT(i != 1+3+9);
            out->bufs[i]  = LAB_Chunk_Light_Read_ByY(ay+y);
        }
        else
        {
            if(write_center && i == 1+3+9)
            {
                out->bufs[i]  = LAB_Chunk_Light_Write(chunks[i]);
                if(!out->bufs[i]) return false;
            }
            else
                out->bufs[i]  = LAB_Chunk_Light_Read(chunks[i]);
        }
    }
    return true;
}



void LAB_BlockNbHood_GetRead(LAB_Chunk*const chunks[27], LAB_BlockNbHood* out)
{
    bool success = LAB_BlockNbHood_Get(chunks, out, false);
    LAB_ASSERT(success);
}

bool LAB_BlockNbHood_GetWrite(LAB_Chunk*const chunks[27], LAB_BlockNbHood_Mut* out)
{
    return LAB_BlockNbHood_Get(chunks, out, true);
}



void LAB_LightNbHood_GetRead(LAB_Chunk*const chunks[27], LAB_LightNbHood* out)
{
    bool success = LAB_LightNbHood_Get(chunks, out, false);
    LAB_ASSERT(success);
}

bool LAB_LightNbHood_GetWrite(LAB_Chunk*const chunks[27], LAB_LightNbHood_Mut* out)
{
    return LAB_LightNbHood_Get(chunks, out, true);
}