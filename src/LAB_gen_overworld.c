#include "LAB_gen_overworld.h"
#include "LAB_random.h"


#define DEF_NOISE_FUNC(me, rec)                                                          \
LAB_HOT LAB_INLINE                                                                       \
void LAB_SmoothNoiseRec##me(LAB_OUT uint32_t smooth[16*16],                       \
                                   LAB_IN uint64_t noise[17*17],                         \
                                   int x0, int y0, int x1, int y1,                       \
                                   uint32_t n00, uint32_t n01,                           \
                                   uint32_t n10, uint32_t n11)                           \
{                                                                                        \
    if(x0+1 == x1)                                                                       \
    {                                                                                    \
        smooth[x0|y0<<4] = ((uint64_t)n00+(uint64_t)n01+(uint64_t)n10+(uint64_t)n11) / 4;\
    }                                                                                    \
    else                                                                                 \
    {                                                                                    \
        int xc, yc;                                                                      \
        uint64_t r0c, rc0, rcc, rc1, r1c;                                                \
        uint32_t n0c, nc0, ncc, nc1, n1c;                                                \
                                                                                         \
        xc = (x0+x1) >> 1;                                                               \
        yc = (y0+y1) >> 1;                                                               \
                                                                                         \
        r0c = noise[xc+17*y0] & 0xffffffff;                                              \
        rc0 = noise[x0+17*yc] & 0xffffffff;                                              \
        rcc = noise[xc+17*yc] & 0xffffffff;                                              \
        rc1 = noise[x1+17*yc] & 0xffffffff;                                              \
        r1c = noise[xc+17*y1] & 0xffffffff;                                              \
                                                                                         \
                                                                                         \
        n0c = ((uint64_t)n00*(0x100000000ll-r0c)+(uint64_t)n01*r0c)>>32;                 \
        nc0 = ((uint64_t)n00*(0x100000000ll-rc0)+(uint64_t)n10*rc0)>>32;                 \
                                                                                         \
        nc1 = ((uint64_t)n01*(0x100000000ll-rc1)+(uint64_t)n11*rc1)>>32;                 \
        n1c = ((uint64_t)n10*(0x100000000ll-r1c)+(uint64_t)n11*r1c)>>32;                 \
                                                                                         \
        /*ncc = ((((uint64_t)nc0*(0x100000000ll-rcc)+(uint64_t)nc1*rcc)>>32)             \
            + (((uint64_t)n0c*(0x100000000ll-rcc)+(uint64_t)n1c*rcc)>>32)                \
        ) >> 1;*/                                                                        \
                                                                                         \
        ncc = ((((uint64_t)n00*(0x100000000ll-rcc)+(uint64_t)n11*rcc)>>32)               \
            + (((uint64_t)n01*(0x100000000ll-rcc)+(uint64_t)n10*rcc)>>32)                \
        ) >> 1;                                                                          \
                                                                                         \
                                                                                         \
        LAB_SmoothNoiseRec##rec(smooth, noise, x0, y0, xc, yc, n00, n0c, nc0, ncc);      \
        LAB_SmoothNoiseRec##rec(smooth, noise, xc, y0, x1, yc, n0c, n01, ncc, nc1);      \
        LAB_SmoothNoiseRec##rec(smooth, noise, x0, yc, xc, y1, nc0, ncc, n10, n1c);      \
        LAB_SmoothNoiseRec##rec(smooth, noise, xc, yc, x1, y1, ncc, nc1, n1c, n11);      \
    }                                                                                    \
}

void LAB_SmoothNoiseRec0(LAB_OUT uint32_t smooth[16*16],
                         LAB_IN uint64_t noise[17*17],
                         int x0, int y0, int x1, int y1,
                         uint32_t n00, uint32_t n01,
                         uint32_t n10, uint32_t n11) {}

DEF_NOISE_FUNC(1, 0)
DEF_NOISE_FUNC(2, 1)
DEF_NOISE_FUNC(4, 2)
DEF_NOISE_FUNC(8, 4)
DEF_NOISE_FUNC(16, 8)


LAB_HOT LAB_INLINE
static void LAB_SmoothNoise(LAB_OUT uint32_t smooth[16*16],
                     LAB_IN uint64_t noise[17*17])
{
    uint32_t n00, n01, n10, n11;
    n00 = noise[0];
    n01 = noise[16];
    n10 = noise[16*17];
    n11 = noise[16*17+16];
    LAB_SmoothNoiseRec16(smooth, noise, 0, 0, 16, 16, n00, n01, n10, n11);
}



LAB_HOT
LAB_Chunk* LAB_GenOverworldProc(void* user, LAB_World* world, int x, int y, int z)
{
    LAB_GenOverworld* gen = user;

    LAB_Block* block = y < 0 ? &LAB_BLOCK_STONE : &LAB_BLOCK_AIR;
    LAB_Chunk* chunk = LAB_CreateChunk(block);
    if(!chunk) return NULL;

    uint64_t noise[17*17];
    LAB_ChunkNoise2D(noise, gen->seed, x, z);
    uint32_t smooth[16*16];
    LAB_SmoothNoise(smooth, noise);


    if(y >= -2 && y <= -1)
    {
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
    return chunk;
}
