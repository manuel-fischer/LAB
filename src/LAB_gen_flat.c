#include "LAB_gen_flat.h"

bool LAB_GenFlatProc(void* user, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_GenFlat* gen_flat = user;

    LAB_BlockID block = y < 0 ? gen_flat->block : LAB_BID_AIR;
    LAB_Chunk_FillGenerate(chunk, block);

    #if 0
    if(y == -1)
    {
        for(int zz = 0; zz < 16; ++zz)
        for(int xx = 0; xx < 16; ++xx)
        for(int yy = 0; yy < 16; ++yy)
        {
            //if(rand() & 1)
            //    chunk->blocks[LAB_CHUNK_OFFSET(xx, yy, zz)] = &LAB_BLOCK_GRASS;
            //chunk->blocks[LAB_CHUNK_OFFSET(xx, 15^yy, zz)] = &LAB_BLOCK_GRASS;
            chunk->blocks[LAB_CHUNK_OFFSET(xx, 15^yy, zz)] = &LAB_BLOCK_COBBLESTONE;
            /*if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;*/
        }
    }
    #endif
    return true;
}
