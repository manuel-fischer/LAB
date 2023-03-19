#include "LAB_chunk_neighborhood.h"

void LAB_GetChunkNeighborsNone(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27])
{
    LAB_ASSERT(center_chunk);
    memset(chunks, 0, sizeof(LAB_Chunk*)*27);
    chunks[LAB_NB_CENTER] = center_chunk;
}


void LAB_GetChunkNeighbors(LAB_Chunk* center_chunk, LAB_Chunk* chunks[27])
{
    // determine chunk neighborhood of size 3x3x3 based on links to direct neighbors

    LAB_ASSERT(center_chunk);
    //LAB_ASSERT(LAB_Chunk_Access(center_chunk));

    //memset(chunks, 0, sizeof(*chunks)*27); // TODO: remove

    #if 0
    memset(chunks, 0xff, sizeof(*chunks)*27);
    #define CHK(chunk) (LAB_ASSERT((chunk)!=((void*)(ptrdiff_t)-1)), chunk)
    #else
    #define CHK(chunk) (chunk)
    #endif

    #define NEIGHBOR(x, y, z) (chunks[LAB_NB_I0(x,y,z)])
    #define NEIGHBOR_FACE(x, y, z, face) LAB_Chunk_Neighbor(CHK(NEIGHBOR(x, y, z)), face)
    //#define NEIGHBOR_FACE(x, y, z, face) (printf("%i: %i %i %i\n", __LINE__, x, y, z), LAB_Chunk_Neighbor(CHK(NEIGHBOR(x, y, z)), face, what_required))
    //#define NEIGHBOR_FACE(x, y, z, face)  (NEIGHBOR(x, y, z)?NEIGHBOR(x, y, z)->neighbors[face]:NULL)

    chunks[LAB_NB_CENTER] = center_chunk;
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
        LAB_ASSERT(i < 27);
        LAB_ASSERT(i == LAB_NB_I0(x,y,z));
        if((i&1) && i != LAB_NB_CENTER)
        {
            if(x == 1)
            {
                LAB_ASSERT(y!=1 && z!=1);
                if(!(chunks[i] = NEIGHBOR_FACE(1, 1, z, LAB_DIR_Y(y!=0))))
                     chunks[i] = NEIGHBOR_FACE(1, y, 1, LAB_DIR_Z(z!=0));
            }
            else if(y == 1)
            {
                LAB_ASSERT(x!=1 && z!=1);
                if(!(chunks[i] = NEIGHBOR_FACE(1, 1, z, LAB_DIR_X(x!=0))))
                     chunks[i] = NEIGHBOR_FACE(x, 1, 1, LAB_DIR_Z(z!=0));
            }
            else
            {
                // TODO: does this assertion confuse GCC?
                //LAB_ASSERT(z == 1);

                LAB_ASSERT(x!=1 && y!=1);
                if(!(chunks[i] = NEIGHBOR_FACE(1, y, 1, LAB_DIR_X(x!=0))))
                     chunks[i] = NEIGHBOR_FACE(x, 1, 1, LAB_DIR_Y(y!=0));
            }
        }
    }

    i = 0;
    for(int z = 0; z < 3; z+=2, i+=6) // step 2+2+6 = 10
    for(int y = 0; y < 3; y+=2, i+=2) // step 2+2 = 4
    for(int x = 0; x < 3; x+=2, i+=2) // step 2
    {
        LAB_ASSERT(i < 27);
        LAB_ASSUME(i == LAB_NB_I0(x,y,z));
        if(!(chunks[i] = NEIGHBOR_FACE(1, y, z, LAB_DIR_X(x!=0))))
        if(!(chunks[i] = NEIGHBOR_FACE(x, 1, z, LAB_DIR_Y(y!=0))))
             chunks[i] = NEIGHBOR_FACE(x, y, 1, LAB_DIR_Z(z!=0));

    }

    #if !defined NDEBUG && 0

        for(int k = 0; k < 27; ++k)
            if(chunks[k]) LAB_ASSERT(LAB_Chunk_Access(chunks[k], what_required));

        for(int k = 0; k < 27; ++k)
        for(int j = k+1; j < 27; ++j)
            if(chunks[k] && chunks[j])
                LAB_ASSERT(chunks[k] != chunks[j]);

        for(int k = 0; k < 27; ++k)
        {
            LAB_Pos16 delta = LAB_NbHood_DeltaPos(k);
            LAB_ChunkPos p = LAB_Pos16_Add(center_chunk->pos, delta);
            if(chunks[k])
                LAB_ASSERT_FMT(LAB_ChunkPosComp(chunks[k]->pos, p) == 0,
                    "center:   %2i %2i %2i\n"
                    "actual:   %2i %2i %2i\n"
                    "expected: %2i %2i %2i\n"
                    "delta:    %2i %2i %2i",
                    
                    center_chunk->pos.x, center_chunk->pos.y, center_chunk->pos.z,
                    chunks[k]->pos.x, chunks[k]->pos.y, chunks[k]->pos.z,
                    p.x, p.y, p.z,
                    delta.x, delta.y, delta.z);
        }

    #endif

    #undef NEIGHBOR_FACE
    #undef NEIGHBOR
    #undef CHK

    LAB_ASSERT(chunks[LAB_NB_CENTER]);
}


LAB_STATIC
bool LAB_BlockNbHood_Get(LAB_Chunk*const chunks[27], LAB_BlockNbHood* out, bool write_center)
{
    LAB_ASSERT(chunks[LAB_NB_CENTER]);
    for(int i=0; i<27; ++i)
    {
        if(chunks[i] == NULL)
        {
            LAB_ASSERT(i != LAB_NB_CENTER);
            out->bufs[i] = &LAB_chunk_empty_blocks_outside;
        }
        else
        {
            if(write_center && i == LAB_NB_CENTER)
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
    LAB_ASSERT(chunks[LAB_NB_CENTER]);
    int ay = chunks[LAB_NB_CENTER]->pos.y - 1;
    int i = 0;
    for(int z=0; z<3; ++z)
    for(int y=0; y<3; ++y)
    for(int x=0; x<3; ++x, ++i)
    {
        if(chunks[i] == NULL)
        {
            LAB_ASSERT(i != LAB_NB_CENTER);
            out->bufs[i]  = LAB_Chunk_Light_Read_ByY(ay+y);
        }
        else
        {
            if(write_center && i == LAB_NB_CENTER)
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