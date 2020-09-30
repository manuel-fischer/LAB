#include "LAB_gen_overworld.h"
#include "LAB_random.h"
#include "LAB_opt.h"
#include "LAB_noise.h"

#include "LAB_gen_overworld_structures.h"

#define LAB_MAX_STRUCTURE_SIZE 1 /*in chunks*/

#include "LAB_simplex_noise.h"
#include <math.h>

#define SMTST(smth, prob, orig) ((smth) > (orig)-(prob)/2 && (smth) < (orig)+(prob)/2)

#define LAB_GEN_DIRT_SALT        0x12345
#define LAB_GEN_UNDERGROUND_SALT 0x54321
#define LAB_GEN_CRYSTAL_SALT     0x98765

//static void LAB_Gen_Surface(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
static void LAB_Gen_Surface_Shape(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
static int  LAB_Gen_Surface_Shape_Func(LAB_GenOverworld* gen, int x, int z);     // height
static int  LAB_Gen_Surface_Populate(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
static void LAB_Gen_Surface_Populate_Func(LAB_GenOverworld* gen, LAB_Placer* p, int cx, int cy, int cz);
static void LAB_Gen_Cave(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
static void LAB_Gen_Cave_Carve(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
static bool LAB_Gen_Cave_Carve_Func(LAB_GenOverworld* gen, int x, int y, int z); // block is cave
static void LAB_Gen_Cave_Crystals(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
static void LAB_Gen_Cave_RockVariety(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);





LAB_HOT
LAB_Chunk* LAB_GenOverworldProc(void* user, /*unused*/LAB_World* world_, int x, int y, int z)
{
    LAB_GenOverworld* gen = user;

    LAB_Block* block = y < 0 ? &LAB_BLOCK_STONE : &LAB_BLOCK_AIR;
    LAB_Chunk* chunk = LAB_CreateChunk(block);
    if(!chunk) return NULL;

    //LAB_Gen_Surface(gen, chunk, x, y, z);

    LAB_Gen_Surface_Shape(gen, chunk, x, y, z);
    LAB_Gen_Cave(gen, chunk, x, y, z);
    LAB_Gen_Surface_Populate(gen, chunk, x, y, z);

    return chunk;
}


/*tatic void LAB_Gen_Surface(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Gen_Surface_Shape(gen, chunk, x, y, z);
    LAB_Gen_Surface_Populate(gen, chunk, x, y, z);
}*/

static void LAB_Gen_Surface_Shape(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, gen->seed^LAB_GEN_DIRT_SALT, x, y, z);

    if(y >= -2 && y <= -1)
    {
        for(int zz = 0; zz < 16; ++zz)
        for(int xx = 0; xx < 16; ++xx)
        for(int yy =15; yy >= 0; --yy)
        {

            int xi = 16*x|xx;
            int yi = 16*y|yy;
            int zi = 16*z|zz;

            int sheight = LAB_Gen_Surface_Shape_Func(gen, xi, zi);

            LAB_Block* b;

            if(yi == sheight)
                b = &LAB_BLOCK_GRASS;
            else if(yi <= sheight)
            {
                uint64_t fact = 0x100000000ll/(32+16);
                if((~LAB_NextRandom(&random)>>32) >= (2u*(-yi)-(-sheight))*fact)
                    b = &LAB_BLOCK_DIRT;
                else
                    continue; // keep stone
            }
            else
                b = &LAB_BLOCK_AIR;

            chunk->blocks[LAB_CHUNK_OFFSET(xx, yy, zz)] = b;
        }
    }
}


static int LAB_Gen_Surface_Shape_Func(LAB_GenOverworld* gen, int xi, int zi)
{
    #define ML 0.001
    #define MS 0.03
    double large = (LAB_SimplexNoise2D(xi*0.001, zi*0.001)+1)*0.5;
    #define fade(t) ((t)*(t)*(t)*((t)*((t)*6-15)+10))
    #define sqr1(t) ((t)*(2-t))
    #define sqr2(t) ((t)*(t))
    double small = 0.70*(LAB_SimplexNoise2D(xi*MS*1, zi*MS*1)+1)*0.5
                 + 0.20*(LAB_SimplexNoise2D(xi*MS*2, zi*MS*2)+1)*0.5
                 + 0.10*(LAB_SimplexNoise2D(xi*MS*4, zi*MS*4)+1)*0.5;
    double displacement = (large*large)*(large*large)*small;
    displacement = sqr1(sqr1(sqr1(displacement)));
    displacement = fade(displacement);
    double base  = 0.50*(LAB_SimplexNoise2D(xi*ML*2+100, zi*ML*2+100)+1)*0.5
                 + 0.50*(LAB_SimplexNoise2D(xi*ML*4+100, zi*ML*4+100)+1)*0.5;
    double n = 0.50*displacement
             + 0.50*base*base*base;
    return (int)floor(n*31.)-32; // Range [0, 32)
}




static int  LAB_Gen_Surface_Populate(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz)
{
    for(int z = -LAB_MAX_STRUCTURE_SIZE; z <= LAB_MAX_STRUCTURE_SIZE; ++z)
    for(int y = -LAB_MAX_STRUCTURE_SIZE; y <= LAB_MAX_STRUCTURE_SIZE; ++y)
    for(int x = -LAB_MAX_STRUCTURE_SIZE; x <= LAB_MAX_STRUCTURE_SIZE; ++x)
    {
        LAB_Placer p;
        p.chunk = chunk;
        p.ox = -16*x;
        p.oy = -16*y;
        p.oz = -16*z;
        LAB_Gen_Surface_Populate_Func(gen, &p, cx+x, cy+y, cz+z);
    }
}

static void LAB_Gen_Surface_Populate_Func(LAB_GenOverworld* gen, LAB_Placer* p, int cx, int cy, int cz)
{
    LAB_Random rnd;
    LAB_ChunkRandom(&rnd, gen->seed^0x13579, cx, cy, cz);
    int count = LAB_NextRandom(&rnd)&7;
    for(int i = 0; i < count; ++i)
    {
        int xz = LAB_NextRandom(&rnd);
        int x = xz    & 15;
        int z = xz>>4 & 15;

        // absolute
        int ay = 1+LAB_Gen_Surface_Shape_Func(gen, x|cx<<4, z|cz<<4);
        if(ay >> 4 == cy)
        {
            LAB_Placer p2;
            p2.chunk = p->chunk;
            p2.ox = p->ox - x;
            p2.oy = p->oy - (ay&15);
            p2.oz = p->oz - z;

            LAB_Gen_Overworld_Tree(&p2, &rnd);
        }
    }
}













static void LAB_Gen_Cave(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    if(y <= -1)
    {
        // Underground Generation
        LAB_Gen_Cave_Carve(gen, chunk, x, y, z);
        LAB_Gen_Cave_Crystals(gen, chunk, x, y, z);
        LAB_Gen_Cave_RockVariety(gen, chunk, x, y, z);
    }
}


static void LAB_Gen_Cave_Carve(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    // Carve out caves
    for(int zz = 0; zz < 16; ++zz)
    for(int yy = 0; yy < 16; ++yy)
    for(int xx = 0; xx < 16; ++xx)
    {
        double xi = x*16|xx;
        double yi = y*16|yy;
        double zi = z*16|zz;
        if(LAB_Gen_Cave_Carve_Func(gen, xi, yi, zi))
            chunk->blocks[xx|yy<<4|zz<<8] = &LAB_BLOCK_AIR;
    }
}

static bool LAB_Gen_Cave_Carve_Func(LAB_GenOverworld* gen, int xi, int yi, int zi)
{

    #define CM (1./128.)
    #define DM (1./32.)
    #define KM (1./10.)
    double ox = LAB_SimplexNoise2D(xi*KM, zi*KM)*0.03;
    double oz = LAB_SimplexNoise2D(xi*KM, zi*KM+100)*0.03;

    double d0 = LAB_SimplexNoise3D(xi*CM+ox, 2*yi*CM, zi*CM+oz);

    double d1 = LAB_SimplexNoise3D(xi*DM+ox, 2*yi*CM, zi*CM+oz);
    double d2 = LAB_SimplexNoise3D(xi*CM+ox, 2*yi*CM, zi*DM+oz);

    double d = d1*d1*d2*d2+d0*d0*2;

    double treshold = 1-1/(double)(abs(yi)*32*0.001+20)*20;
    return d < treshold*0.2;
}

static void LAB_Gen_Cave_Crystals(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, gen->seed^LAB_GEN_CRYSTAL_SALT, x, y, z);

    // Less probability that crystals generate at the top
    int amount = LAB_MIN(-2*y, 100);

    if((LAB_NextRandom(&random)&255) < amount)
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


static void LAB_Gen_Cave_RockVariety(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    static uint64_t noise[17*17*17];
    static uint32_t smooth[16*16*16];

    LAB_ChunkNoise3D(noise, gen->seed^0x12345, x, y, z);
    LAB_SmoothNoise3D(smooth, noise);
    for(int zz = 0; zz < 16*16*16; zz+=16*16)
    for(int yy = 0; yy < 16*16;    yy+=16)
    for(int xx = 0; xx < 16;       xx++)
    {
        uint32_t n = smooth[xx|yy|zz];
        if(SMTST(n, 0x20000000, 0x80000000) && chunk->blocks[xx|yy|zz] == &LAB_BLOCK_STONE)
        {
            chunk->blocks[xx|yy|zz] = &LAB_BLOCK_MARBLE;
        }
    }
}
