#include "LAB_gen_overworld.h"
#include "LAB_random.h"
#include "LAB_opt.h"
//#include "LAB_noise.h"
#include "LAB_bits.h"

#include "LAB_gen_overworld_structures.h"

#define LAB_MAX_STRUCTURE_SIZE 1 /*in chunks*/

#include "LAB_simplex_noise.h"
#include "LAB_math.h"

#define SMTST(smth, prob, orig) ((smth) > (orig)-(prob)/2 && (smth) < (orig)+(prob)/2)

#define LAB_GEN_DIRT_SALT        0x12345
#define LAB_GEN_UNDERGROUND_SALT 0x54321
#define LAB_GEN_CRYSTAL_SALT     0x98765


//LAB_STATIC void LAB_Gen_Surface(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Surface_Shape(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC int  LAB_Gen_Surface_Shape_Func(LAB_GenOverworld* gen, int x, int z);     // height
LAB_STATIC void LAB_Gen_Surface_PopulateLayer(LAB_GenOverworld* gen, LAB_Chunk* chunk, const LAB_StructureLayer* lyr, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Surface_PopulateLayer_Func(LAB_GenOverworld* gen, LAB_Placer* p, const LAB_StructureLayer* lyr, int cx, int cy, int cz);
#if 0
LAB_STATIC void LAB_Gen_Surface_Populate(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Surface_Populate_Func(LAB_GenOverworld* gen, LAB_Placer* p, int cx, int cy, int cz);
#endif
LAB_STATIC void LAB_Gen_Cave(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Cave_Carve(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC bool LAB_Gen_Cave_Carve_Func(LAB_GenOverworld* gen, int x, int y, int z); // block is cave
LAB_STATIC int  LAB_Gen_Cave_Carve_Ceiling(LAB_GenOverworld* gen, int x, int y, int z, int y_dist);
LAB_STATIC int  LAB_Gen_Cave_Carve_Floor(LAB_GenOverworld* gen, int x, int y, int z, int y_dist);
LAB_STATIC void LAB_Gen_Cave_Crystals(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Cave_RockVariety(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);

LAB_STATIC void LAB_Gen_Cave_PopulateLayer(LAB_GenOverworld* gen, LAB_Chunk* chunk, const LAB_StructureLayer* lyr, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Cave_PopulateLayer_Func(LAB_GenOverworld* gen, LAB_Placer* p, const LAB_StructureLayer* lyr, int cx, int cy, int cz);


#define LAB_SURFACE_FACTOR (4)
//#define LAB_SURFACE_FACTOR (8)
//#define LAB_SURFACE_FACTOR (1)
#define LAB_SURFACE_MIN_CY (-2)
//#define LAB_SURFACE_MAX_CY (0)
//#define LAB_SURFACE_MAX_CY (5)
#define LAB_SURFACE_MAX_CY (10)
//#define LAB_SURFACE_MAX_G_CY (100)
#define LAB_SURFACE_MAX_G_CY LAB_SURFACE_MAX_CY
#define LAB_SURFACE_MIN_Y (16*LAB_SURFACE_MIN_CY)
#define LAB_SURFACE_MAX_Y (16*LAB_SURFACE_MAX_CY)




LAB_HOT
LAB_Chunk* LAB_GenOverworldProc(void* user, /*unused*/LAB_World* world_, int x, int y, int z)
{
    LAB_GenOverworld* gen = user;

    LAB_Block* block = y < LAB_SURFACE_MAX_G_CY ? &LAB_BLOCK_STONE : &LAB_BLOCK_AIR;
    LAB_Chunk* chunk = LAB_CreateChunk(block);
    if(!chunk) return NULL;

    //LAB_Gen_Surface(gen, chunk, x, y, z);

    LAB_Gen_Surface_Shape(gen, chunk, x, y, z);
    //return chunk; // DBG
    LAB_Gen_Cave(gen, chunk, x, y, z);
    //LAB_Gen_Surface_Populate(gen, chunk, x, y, z);
    for(size_t i = 0; i < overworld_layers_count; ++i)
        LAB_Gen_Surface_PopulateLayer(gen, chunk, &overworld_layers[i], x, y, z);

    return chunk;
}


/*LAB_STATIC void LAB_Gen_Surface(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Gen_Surface_Shape(gen, chunk, x, y, z);
    LAB_Gen_Surface_Populate(gen, chunk, x, y, z);
}*/


// TODO cache that stores minimum and maximum values for a given cx and cz chunk coordinate
//      - thread local
//      - special function that finalizes cache,
//        - when all height values have been calculated, that is when a chunk has been generated
LAB_STATIC void LAB_Gen_Surface_Shape(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, gen->seed^LAB_GEN_DIRT_SALT, x, y, z);

    //                         v  for dirt generation below the surface
    if(y >= LAB_SURFACE_MIN_CY-1 && y < LAB_SURFACE_MAX_G_CY)
    {
        for(int zz = 0; zz < 16; ++zz)
        for(int xx = 0; xx < 16; ++xx)
        {
            int xi = 16*x|xx;
            int zi = 16*z|zz;
            int sheight = LAB_Gen_Surface_Shape_Func(gen, xi, zi);
            for(int yy = 15; yy >= 0; --yy)
            {
                int yi = 16*y|yy;


                LAB_Block* b = &LAB_BLOCK_AIR;

                //if(sheight < (int)(LAB_NextRandom(&random)&15) + 20)
                if(sheight < (int)(LAB_NextRandom(&random)&31) + 40)
                {
                    if(yi == sheight)
                        b = &LAB_BLOCK_GRASS;
                    else if(yi <= sheight)
                    {
                        //uint64_t fact = 0x100000000ll/(32+16);
                        //if((~LAB_NextRandom(&random)>>32) >= (2u*(-yi)-(-sheight))*fact)
                        if(yi >= sheight-(int)(LAB_NextRandom(&random)&7)-1)
                        //if(yi >= sheight-2)
                            b = &LAB_BLOCK_DIRT;
                        else
                            continue; // keep stone
                    }
                }
                else if(yi <= sheight)
                    continue; // keep stone

                chunk->blocks[LAB_CHUNK_OFFSET(xx, yy, zz)] = b;
            }
        }
    }
}


LAB_STATIC int LAB_Gen_Surface_Shape_Func(LAB_GenOverworld* gen, int xi, int zi)
{
    //#define ML 0.001
    //#define MS 0.03
    #define ML 0.0006
    #define MS 0.007
    //#define MS 0.001


    // smooth step between 0 and 1
    #define fade(t) ((t)*(t)*(t)*((t)*((t)*6-15)+10))
    // square function with peak at 1: sqr1(x) = -sqr2(1-x)
    #define sqr1(t) ((t)*(2-t))
    // square function
    #define sqr2(t) ((t)*(t))


    double x = xi, z = zi;
    // dx, dy in range [-1, 1]
    double dx = LAB_SimplexNoise2D(x*0.03, z*0.1 + 10000);
    double dz = LAB_SimplexNoise2D(x*0.1+10000, z*0.03);
    x+=dx*2;
    z+=dz*2;

    // range [0, 1]
    #define ridge_noise(x, z) (1-fabs(LAB_SimplexNoise2D((x), (z))))
    #define peak_noise(x, z) (sqrt(1-(fabs(LAB_SimplexNoise2D((x), (z)))*fabs(LAB_SimplexNoise2D(10000+(x), (z))))))

    // make the noise function pointer but preserve height of the points
    // by reducing heights, when the length of the gradient of the noise function
    // gets bigger
    double peak_noise2_noise, peak_noise2_dx, peak_noise2_dz;
    #define deriv_h 0.1
    #define peak_noise2(x, z) ( \
        (peak_noise2_noise = LAB_SimplexNoise2D((x), (z))), \
        (peak_noise2_dx = (peak_noise2_noise - LAB_SimplexNoise2D((x)+deriv_h, (z))))/deriv_h, \
        (peak_noise2_dz = (peak_noise2_noise - LAB_SimplexNoise2D((x), (z)+deriv_h)))/deriv_h, \
        (peak_noise2_dx = (sqr2(peak_noise2_dx)+sqr2(peak_noise2_dz))), \
        (peak_noise2_noise+1)*sqrt((1-peak_noise2_dx))-1, \
    )
    #define peak_noise3(x, z) ( \
        (peak_noise2_noise = LAB_SimplexNoise2D(0.5*(x), 0.5*(z))), \
        (peak_noise2_dx = (peak_noise2_noise - LAB_SimplexNoise2D(0.5*(x)+deriv_h, 0.5*(z))))/deriv_h, \
        (peak_noise2_dz = (peak_noise2_noise - LAB_SimplexNoise2D(0.5*(x), 0.5*(z)+deriv_h)))/deriv_h, \
        (peak_noise2_dx = (sqr2(peak_noise2_dx)+sqr2(peak_noise2_dz))), \
        1-sqrt(peak_noise2_dx) \
    )

    #define peak_noise4(x, z) sqr2((peak_noise3((x), (z))+peak_noise3((x), (z)+10000))*0.5)

    // large in range [0, 1]
    double large = (LAB_SimplexNoise2D(x*0.001, z*0.001)+1)*0.5;
    //double large = peak_noise(x*0.001, z*0.001);
    //double large = peak_noise4(x*0.0003, z*0.0003);
    // small in range [0, 1]
    double small = 0.70*(LAB_SimplexNoise2D(x*MS*1, z*MS*1)+1)*0.5
                 + 0.20*(LAB_SimplexNoise2D(x*MS*2, z*MS*2)+1)*0.5
                 + 0.10*(LAB_SimplexNoise2D(x*MS*4, z*MS*4)+1)*0.5;
    /*double r0 = peak_noise(x*MS*1, z*MS*1);
    double r1 = peak_noise(x*MS*2, z*MS*2)*r0;
    double r2 = peak_noise(x*MS*4, z*MS*4)*(r0+r1)*0.5;
    double small = r2;//r0*0.5 + (r1+r2)*0.25;*/

    /*double small = 0.70*peak_noise(x*MS*1, z*MS*1)
                 + 0.20*peak_noise(x*MS*2, z*MS*2)
                 + 0.10*peak_noise(x*MS*4, z*MS*4);*/

    /*ouble small = 0.90*peak_noise4(x*MS*1, z*MS*1)
                 + 0.07*peak_noise4(x*MS*2, z*MS*2)
                 + 0.03*peak_noise4(x*MS*4, z*MS*4);*/
    // displacement in [0, 1]
    double displacement = (large*large)*(large*large)*small; // [0, 1]
    //double displacement = large*small; // [0, 1]
    displacement = sqr1(sqr1(sqr1(displacement))); // [0, 1]
    //displacement = sqr1(displacement); // [0, 1]
    displacement = fade(displacement); // [0, 1]

    // base in [0, 1]
    double base  = 0.50*(LAB_SimplexNoise2D(x*ML*2+100, z*ML*2+100)+1)*0.5
                 + 0.50*(LAB_SimplexNoise2D(x*ML*4+100, z*ML*4+100)+1)*0.5;
    //base = base*base*base;

    double n = 0.50*displacement
             + 0.50*base;
    // TODO LAB_FastFloorF2I == (int)

    LAB_DEBUG_MINMAX(double, n);

    n = sqr2(n);

    //n = n + pow(8., n-1.);
    //n *= (1./LAB_SURFACE_FACTOR);

    return LAB_FastFloorF2I(n*(LAB_SURFACE_MAX_Y-LAB_SURFACE_MIN_Y-1))+LAB_SURFACE_MIN_Y; // Range [LAB_SURFACE_MIN_Y, LAB_SURFACE_MAX_Y)
}




LAB_STATIC void LAB_Gen_Surface_PopulateLayer(LAB_GenOverworld* gen, LAB_Chunk* chunk, const LAB_StructureLayer* lyr, int cx, int cy, int cz)
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
        LAB_Gen_Surface_PopulateLayer_Func(gen, &p, lyr, cx+x, cy+y, cz+z);
    }
}

LAB_STATIC void LAB_Gen_Surface_PopulateLayer_Func(LAB_GenOverworld* gen, LAB_Placer* p, const LAB_StructureLayer* lyr, int cx, int cy, int cz)
{
    LAB_Random rnd;
    LAB_ChunkRandom(&rnd, gen->seed^lyr->salt, cx, cy, cz);

    int count;

    // place plant groups
    uint64_t r = LAB_NextRandom(&rnd);
    if((int)(r&0xff) < lyr->probability)
    {
        count = lyr->min_count+((r>>8)%(lyr->max_count-lyr->min_count+1));
        for(int i = 0; i < count; ++i)
        {
            int xz = LAB_NextRandom(&rnd);
            int x = xz    & 15;
            int z = xz>>4 & 15;

            // absolute
            int ay = 1+LAB_Gen_Surface_Shape_Func(gen, x|cx<<4, z|cz<<4);
            if(ay >> 4 == cy && !LAB_Gen_Cave_Carve_Func(gen, x|cx<<4, ay-1, z|cz<<4)
                             && lyr->min_height <= ay && ay <= lyr->max_height)
            {
                LAB_Placer p2;
                p2.chunk = p->chunk;
                p2.ox = p->ox - x;
                p2.oy = p->oy - (ay&15);
                p2.oz = p->oz - z;

                lyr->structure_func(&p2, &rnd);
            }
        }
    }
}













LAB_STATIC void LAB_Gen_Cave(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    if(y <= -1)
    {
        // Underground Generation
        LAB_Gen_Cave_Carve(gen, chunk, x, y, z);
        LAB_Gen_Cave_Crystals(gen, chunk, x, y, z);
        LAB_Gen_Cave_RockVariety(gen, chunk, x, y, z);
    }
}


LAB_STATIC void LAB_Gen_Cave_Carve(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
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
        //if(LAB_Gen_Cave_Carve_Func(gen, xi, yi, zi)||LAB_Gen_Cave_Carve_Func(gen, xi, yi-1, zi))
            chunk->blocks[xx|yy<<4|zz<<8] = &LAB_BLOCK_AIR;
    }
}

LAB_STATIC bool LAB_Gen_Cave_Carve_Func(LAB_GenOverworld* gen, int xi, int yi, int zi)
{

    #define CM (1./128.)
    #define DM (1./32.)
    #define KM (1./10.)

    float x = xi, y = yi, z = zi;
    //x*=0.5;
    //y*=0.5;
    //z*=0.5;

    double ox = LAB_SimplexNoise2D(x*KM, z*KM)*0.02; //0.03
    double oz = LAB_SimplexNoise2D(x*KM, z*KM+100)*0.02;

    double d0 = LAB_SimplexNoise3D(x*CM+ox, 2*y*CM, z*CM+oz);

    double d1 = LAB_SimplexNoise3D(x*DM+ox, 2*y*CM, z*CM+oz);
    double d2 = LAB_SimplexNoise3D(x*CM+ox, 2*y*CM, z*DM+oz);

    double d = d1*d1*d2*d2+d0*d0*2;

    double treshold = 1-1/(double)(fabs(y)*32*0.001+20)*20;
    return d < treshold*0.2;
}


LAB_STATIC int  LAB_Gen_Cave_Carve_Ceiling(LAB_GenOverworld* gen, int x, int y, int z, int y_dist)
{
    for(int yi = y; yi < y+y_dist; ++yi)
    {
        if(!LAB_Gen_Cave_Carve_Func(gen, x, yi, z)) return yi;
    }
    return y+y_dist;
}

LAB_STATIC int  LAB_Gen_Cave_Carve_Floor(LAB_GenOverworld* gen, int x, int y, int z, int y_dist)
{
    for(int yi = y; yi < y-y_dist; --yi)
    {
        if(!LAB_Gen_Cave_Carve_Func(gen, x, yi, z)) return yi;
    }
    return y+y_dist;
}


LAB_STATIC void LAB_Gen_Cave_Crystals(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, gen->seed^LAB_GEN_CRYSTAL_SALT, x, y, z);

    // Less probability that crystals generate at the top
    unsigned amount = LAB_MIN(2*LAB_MAX(0, -y), 100);

    if((LAB_NextRandom(&random)&255) < amount)
    {
        for(int i = LAB_NextRandom(&random)&15; i > 0; --i)
        {
            LAB_Block*const LIGHTS[4] = {
                &LAB_BLOCK_BLUE_CRYSTAL,
                &LAB_BLOCK_YELLOW_CRYSTAL,
                &LAB_BLOCK_GREEN_CRYSTAL,
                &LAB_BLOCK_RED_CRYSTAL,
            };

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


LAB_STATIC void LAB_Gen_Cave_RockVariety(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    #if 0
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
    #else
    for(int zz = 0; zz < 16; ++zz)
    for(int yy = 0; yy < 16; ++yy)
    for(int xx = 0; xx < 16; ++xx)
    {
        int xi = xx|x<<4;
        int yi = yy|y<<4;
        int zi = zz|z<<4;
        if(chunk->blocks[xx|yy<<4|zz<<8] == &LAB_BLOCK_STONE)
        {
            #define F (1.f/42.f)
            double n = LAB_SimplexNoise3D(xi*F, yi*F+gen->seed+0x12345, zi*F);
            //if(n >= 0.375 && n <= 0.625)
            //if(n <= 0.25)
            if(n <= 0.17)
                chunk->blocks[xx|yy<<4|zz<<8] = &LAB_BLOCK_MARBLE;
            //else if(n >= 0.25 && n <= 0.75)
            //else if(n >= 0.75)
            else if(n >= 0.83)
                chunk->blocks[xx|yy<<4|zz<<8] = &LAB_BLOCK_BASALT;
        }
    }
    #endif
}
