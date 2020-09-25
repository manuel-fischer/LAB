#include "LAB_gen_overworld.h"
#include "LAB_random.h"
#include "LAB_opt.h"
#include "LAB_noise.h"

#include "LAB_simplex_noise.h"
#include <math.h>

#define SMTST(smth, prob, orig) ((smth) > (orig)-(prob)/2 && (smth) < (orig)+(prob)/2)

LAB_HOT
LAB_Chunk* LAB_GenOverworldProc(void* user, LAB_World* world, int x, int y, int z)
{
    LAB_GenOverworld* gen = user;

    LAB_Block* block = y < 0 ? &LAB_BLOCK_STONE : &LAB_BLOCK_AIR;
    LAB_Chunk* chunk = LAB_CreateChunk(block);
    if(!chunk) return NULL;


    static uint64_t noise[17*17*17];
    static uint32_t smooth[16*16*16];

    LAB_Random random;
    LAB_ChunkRandom(&random, gen->seed^0x12345, x, y, z);

    if(y >= -2 && y <= -1)
    {
        //LAB_ChunkNoise2D(noise, gen->seed, x, z);
        //LAB_SmoothNoise2D(smooth, noise);

        for(int zz = 0; zz < 16; ++zz)
        for(int xx = 0; xx < 16; ++xx)
        for(int yy = 0; yy < 16; ++yy)
        {
            //printf("%uL\n", noise[xx+17*zz]&15);
            uint64_t height = (15^yy)+16*(y+2);          // Range [0, 32)
            //uint64_t sheight = smooth[xx|16*zz]>>(32-5); // Range [0, 32)
            double xi = 16*x|xx;
            double zi = 16*z|zz;

            #define ML 0.001
            //#define ML 0.01
            #define MS 0.03
            double large = (LAB_SimplexNoise2D(xi*0.001, zi*0.001)+1)*0.5;
            #define fade(t) ((t)*(t)*(t)*((t)*((t)*6-15)+10))
            #define sqr1(t) ((t)*(2-t))
            #define sqr2(t) ((t)*(t))
            double small = 0.70*(LAB_SimplexNoise2D(xi*MS*1, zi*MS*1)+1)*0.5
                         + 0.20*(LAB_SimplexNoise2D(xi*MS*2, zi*MS*2)+1)*0.5
                         + 0.10*(LAB_SimplexNoise2D(xi*MS*4, zi*MS*4)+1)*0.5;
            //small = sqr1(sqr1(sqr1(small)));
            double displacement = (large*large)*(large*large)*small;
            displacement = sqr1(sqr1(sqr1(displacement)));
            //displacement = fade(fade(displacement));
            displacement = fade(displacement);
            double base  = 0.50*(LAB_SimplexNoise2D(xi*ML*2+100, zi*ML*2+100)+1)*0.5
                         + 0.50*(LAB_SimplexNoise2D(xi*ML*4+100, zi*ML*4+100)+1)*0.5;
            double n = 0.50*displacement
                     + 0.50*base*base*base;
            //n = sqr1(sqr1(n));
            //double n = large*small;
            uint64_t sheight = (int)floor(n*31.); // Range [0, 32)

            LAB_Block* b;

            if(height == sheight)
                b = &LAB_BLOCK_GRASS;
            else if(height <= sheight)
            {
                //if(rng.random() >= (2*height-(sheight>>))/(16+8))
                uint64_t fact = 0x100000000ll/(32+16);
                if((~LAB_NextRandom(&random)>>32) >= (2u*(32-height)-(32-sheight))*fact)
                    b = &LAB_BLOCK_DIRT;
                else
                    continue; // keep stone
            }
            else
                b = &LAB_BLOCK_AIR;

            chunk->blocks[LAB_CHUNK_OFFSET(xx, 15^yy, zz)] = b;
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
    //#define A -5
    #define A -1
    #ifdef A
    if(y <= A)
    #else
    if(y <= 0)
    #endif
    //if(0)
    {
        // Carve out
        //LAB_ChunkNoise3D(noise, gen->seed, x, y, z);
        //LAB_SmoothNoise3D(smooth, noise);
        for(int zz = 0; zz < 16; ++zz)
        {
            for(int yy = 0; yy < 16; ++yy)
            {
                #ifdef A
                /*uint32_t threshold = y == A
                                   ? 0x08000000ull*(15-yy)
                                   : 0x80000000ull;*/
                #else
                uint32_t threshold = 0x80000000ull;
                #endif
                for(int xx = 0; xx < 16; ++xx)
                {
                    //if(noise[(xx+yy+zz)&0xccc|0x333] < 0x03000000)
                    //if(smooth[xx+yy+zz] < 0x30000000)     '
                    //printf("%x\n", smooth[xx+yy+zz]);
                    //if(smooth[xx+yy+zz] < threshold/3 && smooth[xx+yy+zz] > 0x08000000)
                    //if(smooth[xx+yy+zz] < threshold/2)
                    double xi = x*16|xx;
                    double yi = y*16|yy;
                    double zi = z*16|zz;
                    #if 0
                    #define CM (1./32.)                    //double d = (LAB_SimplexNoise3D(xi*CM*1, yi*CM*1, zi*CM*1)+1)*0.5;
                    double d = 0.50*(LAB_SimplexNoise3D(xi*CM*1, yi*CM*1, zi*CM*1)+1)*0.5
                             + 0.30*(LAB_SimplexNoise3D(xi*CM*2, yi*CM*2, zi*CM*2)+1)*0.5
                             + 0.20*(LAB_SimplexNoise3D(xi*CM*4, yi*CM*4, zi*CM*4)+1)*0.5;
                    #else
                    //#define CM (1./128.)
                    //#define DM (1./32.)
                    #if 0
                    double d1 = 0.50*(LAB_SimplexNoise3D(xi*DM*1, yi*CM*1, zi*CM*1)+1)*0.5
                              + 0.30*(LAB_SimplexNoise3D(xi*DM*2, yi*CM*2, zi*CM*2)+1)*0.5
                              + 0.20*(LAB_SimplexNoise3D(xi*DM*4, yi*CM*4, zi*CM*4)+1)*0.5;
                    double d2 = 0.50*(LAB_SimplexNoise3D(xi*CM*1, yi*CM*1, zi*DM*1)+1)*0.5
                              + 0.30*(LAB_SimplexNoise3D(xi*CM*2, yi*CM*2, zi*DM*2)+1)*0.5
                              + 0.20*(LAB_SimplexNoise3D(xi*CM*4, yi*CM*4, zi*DM*4)+1)*0.5;
                    #endif
                    #if 0
                    //#define KM (1./20.)
                    //#define KM (1./3.)
                    #define KM (1./5.)
                    double ox = 0; //LAB_SimplexNoise2D(xi*KM, zi*KM)*0.03;
                    double oz = 0; //LAB_SimplexNoise2D(xi*KM, zi*KM+100)*0.03;

                    double d0 = (LAB_SimplexNoise3D(xi*CM+ox, 2*yi*CM, zi*CM+oz)+1)*0.5;

                    double d1 = (LAB_SimplexNoise3D(xi*DM+ox, 2*yi*CM, zi*CM+oz)+1)*0.5;
                    double d2 = (LAB_SimplexNoise3D(xi*CM+ox, 2*yi*CM, zi*DM+oz)+1)*0.5;
                    double d3 = (LAB_SimplexNoise3D(xi*CM+ox, 2*yi*DM, zi*CM+oz)+1)*0.5;
                    //double d = (d1+d2)*0.5;
                    //double d = sqr2((d1+d2+d3)*(1./3.));
                    double d = d1*(1-d3) + d2*d3+d0;
                    //double d = d1*d2;
                    if(d < 0.55)
                    #endif

                    #define CM (1./128.)
                    #define DM (1./32.)
                    #define KM (1./10.)
                    double ox = LAB_SimplexNoise2D(xi*KM, zi*KM)*0.03;
                    double oz = LAB_SimplexNoise2D(xi*KM, zi*KM+100)*0.03;

                    double d0 = LAB_SimplexNoise3D(xi*CM+ox, 2*yi*CM, zi*CM+oz);

                    double d1 = LAB_SimplexNoise3D(xi*DM+ox, 2*yi*CM, zi*CM+oz);
                    double d2 = LAB_SimplexNoise3D(xi*CM+ox, 2*yi*CM, zi*DM+oz);
                   // double d3 = LAB_SimplexNoise3D(xi*CM+ox, 2*yi*DM, zi*CM+oz);
                    double d = d1*d1*d2*d2+d0*d0*2;//0.5;
                    //double d = d1*d1*d2*d2+(d0+1)*0.5;
                    //double d = d1*d1*d2*d2*(d3+1)*0.5+(d0*d0)*0.0005;

                    //double treshold = 0.25-0.25/(double)(yi*yi+1);
                    //double treshold = 1-1/(double)(yi*yi*0.00001+1);
                    double treshold = 1-1/(double)(yi*yi*0.001+20)*20;
                    if(d < treshold*0.2/**(0.5+0.5*(d3+1)*0.5)*/)
                    #endif
                    //if(d < (float)threshold/(float)((long long)256*256*256*256)/2)
                    //if(d < 0.125)
                    //if(d < 0.125)
                    //if(!SMTST(smooth[xx+yy+zz], 0xffffffff^threshold, 0x80000000))
                    {
                        chunk->blocks[xx|yy<<4|zz<<8] = &LAB_BLOCK_AIR;
                    }
                }
            }
        }

        if((LAB_NextRandom(&random)&7) < 3)
        {
            for(int i = LAB_NextRandom(&random)&15; i > 0; --i)
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
