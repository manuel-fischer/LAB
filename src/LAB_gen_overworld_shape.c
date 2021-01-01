#include "LAB_gen_overworld_shape.h"

#include "LAB_simplex_noise.h"
#include "LAB_math.h"
#include "LAB_random.h"

#define LAB_GEN_DIRT_SALT        0x12345
#define LAB_GEN_UNDERGROUND_SALT 0x54321


// TODO cache that stores minimum and maximum values for a given cx and cz chunk coordinate
//      - thread local
//      - special function that finalizes cache,
//        - when all height values have been calculated, that is when a chunk has been generated
void LAB_Gen_Surface_Shape(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, gen->seed^LAB_GEN_DIRT_SALT, x, y, z);

    //                         v  for dirt generation below the surface
    if(y >= LAB_SURFACE_MIN_CY-1 && y < LAB_SURFACE_MAX_CY)
    {
        for(int zz = 0; zz < 16; ++zz)
        for(int xx = 0; xx < 16; ++xx)
        {
            int xi = 16*x|xx;
            int zi = 16*z|zz;
            int river = 0;//LAB_Gen_River_Func(gen, xi, zi);
            int sheight = LAB_Gen_Surface_Shape_Func(gen, xi, zi);
            sheight -= river;
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

                if(river && b != &LAB_BLOCK_AIR) b = &LAB_BLOCK_LAPIZ;

                chunk->blocks[LAB_CHUNK_OFFSET(xx, yy, zz)] = b;
            }
        }
    }
}


int LAB_Gen_Surface_Shape_Func(LAB_GenOverworld* gen, int xi, int zi)
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
    //double peak_noise2_noise, peak_noise2_dx, peak_noise2_dz;
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

    //LAB_DEBUG_MINMAX(double, n);

    n = sqr2(n);

    //n = n + pow(8., n-1.);
    //n *= (1./LAB_SURFACE_FACTOR);

    return LAB_FastFloorF2I(n*(LAB_SURFACE_MAX_Y-LAB_SURFACE_MIN_Y-1))+LAB_SURFACE_MIN_Y; // Range [LAB_SURFACE_MIN_Y, LAB_SURFACE_MAX_Y)
}


int  LAB_Gen_River_Func(LAB_GenOverworld* gen, int x, int z)
{
    double xx, zz;
    xx = x;
    zz = z;

    double xd = LAB_SimplexNoise2D(xx*0.01, zz*0.01+100);
    double zd = LAB_SimplexNoise2D(xx*0.01, zz*0.01+10000);
    xx += 20*xd;
    zz += 20*zd;

    double n0 = LAB_SimplexNoise2D(xx*0.001, zz*0.001+100000);
    double n1 = LAB_SimplexNoise2D(xx*0.001, zz*0.001+1000);

    double n = n0/(n1+1.1);

    return LAB_MAX(0, LAB_FastFloorD2I((1 - 2000*n*n) * 5));
}




void LAB_Gen_Cave_Carve(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
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

bool LAB_Gen_Cave_Carve_Func(LAB_GenOverworld* gen, int xi, int yi, int zi)
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







bool LAB_Gen_PlaceOnSurface(void* gen, int* x, int* y, int* z)
{
    *y = 1+LAB_Gen_Surface_Shape_Func((LAB_GenOverworld*)gen, *x, *z);
    return !LAB_Gen_Cave_Carve_Func((LAB_GenOverworld*)gen, *x, *y-1, *z);
        //&& !LAB_Gen_River_Func((LAB_GenOverworld*)gen, *x, *z);
}

bool LAB_Gen_PlaceOnCaveCeiling(void* gen, int* x, int* y, int* z)
{
    int y_surface = LAB_Gen_Surface_Shape_Func((LAB_GenOverworld*)gen, *x, *z);
    if(*y >= y_surface) return 0;

    // do not place inside the rock
    if(!LAB_Gen_Cave_Carve_Func((LAB_GenOverworld*)gen, *x, *y, *z)) return 0;

    for(int yi = *y; yi < *y+16; ++yi)
    {
        if(!LAB_Gen_Cave_Carve_Func((LAB_GenOverworld*)gen, *x, yi, *z))
        {
            *y = yi-1;
            return 1;
        }
    }
    return 0;
}

bool LAB_Gen_PlaceOnCaveFloor(void* gen, int* x, int* y, int* z)
{
    int y_surface = LAB_Gen_Surface_Shape_Func((LAB_GenOverworld*)gen, *x, *z);
    if(*y >= y_surface) return 0;

    // do not place inside the rock
    if(!LAB_Gen_Cave_Carve_Func((LAB_GenOverworld*)gen, *x, *y, *z)) return 0;

    for(int yi = *y; yi > *y-16; --yi)
    {
        if(!LAB_Gen_Cave_Carve_Func((LAB_GenOverworld*)gen, *x, yi, *z))
        {
            *y = yi+1;
            return 1;
        }
    }
    return 0;
}

