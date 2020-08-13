#include "LAB_gen_overworld.h"
#include "LAB_random.h"
#include "LAB_opt.h"
#include "LAB_noise.h"


#define SMTST(smth, prob, orig) ((smth) > (orig)-(prob)/2 && (smth) < (orig)+(prob)/2)

LAB_HOT
LAB_Chunk* LAB_GenOverworldProc(void* user, LAB_World* world, int x, int y, int z)
{
    LAB_GenOverworld* gen = user;

    LAB_Block* block = y < 0 ? &LAB_BLOCK_STONE : &LAB_BLOCK_AIR;
    LAB_Chunk* chunk = LAB_CreateChunk(block);
    if(!chunk) return NULL;


    uint64_t noise[17*17*17];
    uint32_t smooth[16*16*16];

    if(y >= -2 && y <= -1)
    {
        uint64_t noise[17*17];
        LAB_ChunkNoise2D(noise, gen->seed, x, z);
        uint32_t smooth[16*16];
        LAB_SmoothNoise2D(smooth, noise);

        for(int zz = 0; zz < 16; ++zz)
        for(int xx = 0; xx < 16; ++xx)
        for(int yy = 0; yy < 16; ++yy)
        {
            //printf("%uL\n", noise[xx+17*zz]&15);
            uint64_t height = (15^yy)+16*(y+2);
            uint64_t sheight = smooth[xx|16*zz];//>>(32-5);
            for(int i = 0; i < 0; ++i)
            {
                /*sheight = sheight < 0x80000000
                        ? sheight*sheight
                        : 0x7fffffffffffffff^((0xffffffff^sheight)*(0xffffffff^sheight));
                sheight >>= 31;*/
                sheight*=sheight;
                sheight >>= 32;
            }
            sheight >>= 32-5;

            if(height <= sheight)
                break /*yy*/;

            chunk->blocks[LAB_CHUNK_OFFSET(xx, 15^yy, zz)] = &LAB_BLOCK_AIR;
            //if(rand() & 1)
            //    chunk->blocks[LAB_CHUNK_OFFSET(xx, yy, zz)] = &LAB_BLOCK_GRASS;
            //chunk->blocks[LAB_CHUNK_OFFSET(xx, 15^yy, zz)] = &LAB_BLOCK_GRASS;
            //chunk->blocks[LAB_CHUNK_OFFSET(xx, 15^yy, zz)] = &LAB_BLOCK_COBBLESTONE;
            /*if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;
            if((rand() & 3) == 0) break;*/
        }
    }
    if(y <= -5)
    //if(0)
    {
        // Carve out
        LAB_ChunkNoise3D(noise, gen->seed, x, y, z);
        LAB_SmoothNoise3D(smooth, noise);
        for(int zz = 0; zz < 16*16*16; zz+=16*16)
        {
            for(int yy = 0; yy < 16*16;    yy+=16)
            {
                uint32_t threshold = y == -5
                                   ? 0x08000000ull*(15-yy/16)
                                   : 0x80000000ull;
                for(int xx = 0; xx < 16;       xx++)
                {
                    //if(noise[(xx+yy+zz)&0xccc|0x333] < 0x03000000)
                    //if(smooth[xx+yy+zz] < 0x30000000)     '
                    //printf("%x\n", smooth[xx+yy+zz]);
                    //if(smooth[xx+yy+zz] < threshold/3 && smooth[xx+yy+zz] > 0x08000000)
                    if(smooth[xx+yy+zz] < threshold/2)
                    //if(!SMTST(smooth[xx+yy+zz], 0xffffffff^threshold, 0x80000000))
                    {
                        chunk->blocks[xx+yy+zz] = &LAB_BLOCK_AIR;
                    }
                }
            }
        }

        LAB_Random random;
        LAB_ChunkRandom(&random, gen->seed^0x12345, x, y, z);

        if((LAB_NextRandom(&random)&0x7) < 3)
        {
            for(int i = LAB_NextRandom(&random)&0x15; i > 0; --i)
            {
                LAB_Block*const LIGHTS[4] = { &LAB_BLOCK_BLUE_LIGHT, &LAB_BLOCK_YELLOW_LIGHT, &LAB_BLOCK_GREEN_LIGHT, &LAB_BLOCK_RED_LIGHT };

                LAB_Block* light = LIGHTS[LAB_NextRandom(&random)&3];
                int xx = LAB_NextRandom(&random) & 0xf;
                int zz = LAB_NextRandom(&random) & 0xf;
                int yy;
                for(yy = 0; yy < 16; ++yy)
                    if(chunk->blocks[xx+16*yy+16*16*zz] == &LAB_BLOCK_STONE)
                        break;
                if(yy == 16) continue;

                int h = (LAB_NextRandom(&random)&3)+2;
                for(--yy; yy >= 0 && h > 0; --yy, --h)
                {
                    chunk->blocks[xx+16*yy+16*16*zz] = light;
                }
            }
        }
    }


    LAB_ChunkNoise3D(noise, gen->seed^0x12345, x, y, z);
    LAB_SmoothNoise3D(smooth, noise);
    for(int zz = 0; zz < 16*16*16; zz+=16*16)
    {
        for(int yy = 0; yy < 16*16;    yy+=16)
        {
            for(int xx = 0; xx < 16;       xx++)
            {
                //if(noise[(xx+yy+zz)&0xccc|0x333] < 0x03000000)
                //if(smooth[xx+yy+zz] < 0x30000000)     '
                //printf("%x\n", smooth[xx+yy+zz]);
                uint32_t n = smooth[xx+yy+zz];
                if(SMTST(n, 0x20000000, 0x80000000) && chunk->blocks[xx+yy+zz] == &LAB_BLOCK_STONE)
                {
                    chunk->blocks[xx+yy+zz] = &LAB_BLOCK_MARBLE;
                }
            }
        }
    }


    return chunk;
}
