#include "LAB_gen_overworld_shape.h"

#include "LAB_simplex_noise.h"
#include "LAB_math.h"
#include "LAB_random.h"
#include "LAB_gen_overworld_biomes.h"

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
                const LAB_Gen_Biome* biome = LAB_Gen_Biome_Func(gen, xi, zi, LAB_NextRandom(&random));

                int yi = 16*y|yy;

                const LAB_Block* b = &LAB_BLOCK_AIR;

                //if(sheight < (int)(LAB_NextRandom(&random)&15) + 20)
                if(sheight < (int)(LAB_NextRandom(&random)&31) + 40)
                {
                    if(yi == sheight)
                        b = biome->surface_block;
                    else if(yi <= sheight)
                    {
                        //uint64_t fact = 0x100000000ll/(32+16);
                        //if((~LAB_NextRandom(&random)>>32) >= (2u*(-yi)-(-sheight))*fact)
                        if(yi >= sheight-(int)(LAB_NextRandom(&random)&7)-1)
                        //if(yi >= sheight-2)
                            b = biome->ground_block;
                        else
                            continue; // keep stone
                    }
                }
                else if(yi <= sheight)
                    continue; // keep stone

                if(river && b != &LAB_BLOCK_AIR) b = &LAB_BLOCK_LAPIZ;

                chunk->blocks[LAB_CHUNK_OFFSET(xx, yy, zz)] = (LAB_Block* /*TODO make this all const*/)b;
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

    double seed_a = (gen->seed^gen->seed<<1) & 0xffff;
    double seed_b = gen->seed & 0xffff;

    double x = xi, z = zi;
    // dx, dy in range [-1, 1]
    double dx = LAB_SimplexNoise2D(x*0.03, z*0.1 + 10000);
    double dz = LAB_SimplexNoise2D(x*0.1 + 10000, z*0.03);
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
    double large = (LAB_SimplexNoise2D(seed_a+x*0.001, seed_b+z*0.001)+1)*0.5;
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

#if 0
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
#elif 0
    float x = xi, y = yi, z = zi;
    //x*=0.5;
    //y*=0.5;
    //z*=0.5;

    //float rot = LAB_SimplexNoise3D(x*0.01, y*0.01, z*0.01);
    //float xx = x*rot+z*(1-rot);
    //float zz = z*rot+x*(1-rot);
    //x=xx; z=zz;

    //float r1 = LAB_SimplexNoise2D(4123789+x*0.01, z*0.01);
    //float r2 = LAB_SimplexNoise2D(8923543+x*0.01, z*0.01);
    //x = x*0.2+r1*10;
    //z = z*0.2+r2*10;

    const float CM = 1./128.;
    const float DM = 1./32.;
    const float KM = 1./10.;
    //x*=2;
    //y*=2; y = -y;
    //z*=2;
    const double C = 0.005;
    const double CY = C;//0.01;

    double noise_temp1, noise_temp2;
    #define NOISE(rand, scale, x, y, z) \
        LAB_SimplexNoise3D((rand)+(scale)*(scale)*(x)*C,(rand)+(scale)*(scale)*(y)*CY, (rand)+(scale)*(scale)*(z)*C)/(scale)


    double a  = NOISE(67234, 0.9, x, y, z); a*=a;
    double b  = NOISE(78955, 0.9, x, y, z); b*=b;
    double c  = NOISE(46754, 1, x*0.8, y*2, z*0.7); c*=c;
    //double c1 = NOISE(98670, 1, x*0.8, y*2, z*0.7); c1*=c1;
    //double c2 = NOISE(76546, 1, x*0.7, y*1, z*0.8); c2*=c2;
    //double c3 = NOISE(34289, 1, x*0.7, y*3, z*0.8); c3*=c3;

    //double c  = NOISE(46754, 1.3, x, y, z); c*=c;
    double c1 = NOISE(98670, 1.3, x, y, z); c1*=c1;
    double c2 = NOISE(76546, 1.3, x, y, z); c2*=c2;
    double c3 = NOISE(34289, 1.3, x, y, z); c3*=c3;

    //a*= NOISE(67234, 1, x, y+1, z);
    //a*=a;

    double d  = NOISE(43525, 1, x, y, z); d*=d;
    double e  = NOISE(76547, 1, x, y, z); e*=e;
    double f  = NOISE(34643, 1, x, y, z); f*=f;

    double blob_ones = fabs(y)*0.01;

    //double h = LAB_MIN3(a+b+c, a+b*c+d, a+b-c+d);
    //double h = LAB_MIN(a+b*c+d, a+b-c+d);
    //double h = LAB_fSmoothMin(a+b*c+d, a+b-c*d, 0.5);

    //double j =  LAB_MIN3(a+b+c1*c, a+b+c2*(1-c), a+b+c3);
    //double j =  LAB_MIN3(a+b+c1*c, a+b*c+c2, a*c+b+c3);
    //double j =  LAB_MIN3(a+b+c1, a+b+c2, a+b+c3);
    double j =  LAB_fSmoothMax(a+b+c1, a+b+c2, 0.5);
    double k = j-blob_ones*(e*f)+d;

    //double h = LAB_fSmoothMin((a+b+c)*3, k, 0.25);
    double h = LAB_MIN(j, k);

    //double treshold = 1-1/(double)(fabs(y)*32*0.001+20)*20;
    double treshold = 1-1/(double)(fabs(y)*32*0.001+20)*20;
    return h < treshold*0.2;
#elif 1
    float x = xi, y = yi, z = zi;
    //x*=0.5;
    //y*=0.5;
    //z*=0.5;

    //float rot = LAB_SimplexNoise3D(x*0.01, y*0.01, z*0.01);
    //float xx = x*rot+z*(1-rot);
    //float zz = z*rot+x*(1-rot);
    //x=xx; z=zz;

    //float r1 = LAB_SimplexNoise2D(4123789+x*0.1, z*0.1);
    //float r2 = LAB_SimplexNoise2D(8923543+x*0.1, z*0.1);
    //x = x+r1*1;
    //z = z+r2*1;
    //x = x*0.2+r1*10;
    //z = z*0.2+r2*10;

    const float CM = 1./128.;
    const float DM = 1./32.;
    const float KM = 1./10.;
    //x*=2;
    //y*=2; y = -y;
    //z*=2;
    const double C = 0.005;
    const double CY = C;//0.01;

    double noise_temp1, noise_temp2;
    #define NOISE(rand, scale, x, y, z) \
        LAB_SimplexNoise3D((rand)+(scale)*(x)*C,(1234+(rand))+(scale)*(y)*CY, (235432+(rand))+(scale)*(z)*C)

    #define NOISE2(rand, scale, x, y, z) ((NOISE(rand, scale, x, y, z) + NOISE(rand, (scale)*2, x, y, z)))

    double val = 1;

    for(int i = 0; i < 4; ++i)
    {
        double a = NOISE2(67234^(i<<5), 1, x, y, z); a*=a;
        double b = NOISE2(78955^(i<<6), 1, x, y, z); b*=b;
        double c = NOISE2(46754^(i<<7), 1, x, y, z); c*=c;

        val = LAB_MIN(val, a+b+c);
    }
    val *= 0.01;

    //a*= NOISE(67234, 1, x, y+1, z);
    //a*=a;

    double yy = LAB_MIN(fabs(y), 300);
    yy *= (1./3.)*(2+NOISE(12312, 0.001, x, y, z));
    //double yy = 100 - cos(y/300)*100;

    double d  = NOISE(43525, 2, x, y, z); d*=d;
    double e  = NOISE(76547, 2, x, y, z); e*=e;
    double f  = NOISE(34643, 2, x, y, z); f*=f;

    double blob_ones = 1-1/(yy*0.01+2)*2;

    double j = val*3;
    double k = 0.1-blob_ones*(e*f);//+d;

    double h = LAB_MIN(j, k);
    //double h = k;

    double treshold = 1-1/(double)(yy*32*0.0001+20)*20;
    return h < treshold*0.2;
#endif
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

