#include "LAB_builtin_overworld_funcs.h"

#include "LAB_builtin_overworld.h"

#include "LAB_simplex_noise.h"
#include "LAB_simplex_noise_chord.h"
#include "LAB_math.h"

int LAB_BuiltinOverworld_SurfaceHeight_Func(uint64_t world_seed, int xi, int zi)
{
    double x = xi, z = zi;
    // dx, dy in range [-1, 1]
    double dx = LAB_SimplexNoise2D(x*0.03, z*0.1 + 10000);
    double dz = LAB_SimplexNoise2D(x*0.1 + 10000, z*0.03);
    x+=dx*2;
    z+=dz*2;



    float a = LAB_SimplexNoiseChord2DS(world_seed,          (float)x/2000.f, (float)z/2000.f, 6);
    float b = LAB_SimplexNoiseChord2DS(world_seed^0x342784, (float)x/2000.f, (float)z/2000.f, 6);
    float c = LAB_SimplexNoiseChord2DS(world_seed^0xf865de, (float)x/200.f,  (float)z/200.f,  2);

    float hd = a;

    float step = 1.f/(1.f+expf(hd*50));
    hd = step*(b*b*b-0.2)+0.2 + c*0.1*(1-step);

    return LAB_FastFloorF2I(hd*128 - 10);
}

#define LAB_CAVE_ALTITUDE_C (2)
#define LAB_CAVE_ALTITUDE (16*LAB_CAVE_ALTITUDE_C)
bool LAB_BuiltinOverworld_IsCave_Func(uint64_t world_seed, int xi, int yi, int zi)
{
    float x = xi, y = yi-LAB_CAVE_ALTITUDE, z = zi;

    const double C = 0.005;
    const double CY = C;//0.01;

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

    double yy = LAB_MIN(fabs(y), 300);
    yy *= (1./3.)*(2+NOISE(12312, 0.001, x, y, z));

    double d  = NOISE(43525, 2, x, y, z); d*=d;
    double e  = NOISE(76547, 2, x, y, z); e*=e;
    double f  = NOISE(34643, 2, x, y, z); f*=f;

    double blob_ones = 1-1/(yy*0.01+2)*2;

    double j = val*3;
    double k = 0.1-blob_ones*(e*f);

    double h = LAB_MIN(j, k);
    //double h = k;

    double treshold = 1-1/(double)(yy*32*0.0001+20)*20;
    return h < treshold*0.2;
}




float LAB_BuiltinOverworld_BiomeTemperature_Func(uint64_t world_seed, int x, int z)
{
    int xx = ((world_seed&0xffff)^0x1234)+x;
    int zz = ((world_seed&0xffff)^0x9876)+z;

    return LAB_SimplexNoiseChord2DS(0, xx*0.001, zz*0.001, 5)*0.5;
}

float LAB_BuiltinOverworld_BiomeHumidity_Func(uint64_t world_seed, int x, int z)
{
    int xx = ((world_seed>>16&0xffff)^0x6543)+x;
    int zz = ((world_seed>>16&0xffff)^0x789a)+z;

    return LAB_SimplexNoiseChord2DS(0, xx*0.0025, zz*0.0025, 5)*0.5;
}


LAB_STATIC
size_t LAB_BuiltinOverworld_UpwardsSlopeCount(uint64_t world_seed, int x, int z, int offset_mask, int delta, int min_height, int max_height, LAB_Random* random)
{
    size_t count = 0;

    int xz = LAB_NextRandom(random);
    int xx = x+(xz      & offset_mask)-(xz >>  4 & offset_mask);
    int zz = z+(xz >> 8 & offset_mask)-(xz >> 12 & offset_mask);
    struct { int dx, dz; } points[] = {
        { delta, 0 }, { -delta, 0 }, { 0, delta }, { 0, -delta }
    };
    for(size_t i = 0; i < LAB_LEN(points); ++i)
    {
        int dx = points[i].dx;
        int dz = points[i].dz;
        int height = LAB_BuiltinOverworld_SurfaceHeight_Func(world_seed, xx+dx, zz+dz);
        if(min_height <= height && height < max_height)
            count++;
    }
    return count;
}

LAB_SurfaceBiomeID LAB_BuiltinOverworld_SurfaceBiome_Func(uint64_t world_seed, int x, int z, LAB_Random* random)
{
    LAB_RandomBits64 r = LAB_RandomBits64_Make(random);

    const int gradient_shift = 5;
    int dx0 = LAB_RandomBits64_Next(&r, gradient_shift);
    int dz0 = LAB_RandomBits64_Next(&r, gradient_shift);
    int dx1 = LAB_RandomBits64_Next(&r, gradient_shift);
    int dz1 = LAB_RandomBits64_Next(&r, gradient_shift);

    float temperature = LAB_BuiltinOverworld_BiomeTemperature_Func(world_seed, x+dx0, z+dz0);
    float humidity    = LAB_BuiltinOverworld_BiomeHumidity_Func(world_seed, x+dx1, z+dz1);

    int sheight = LAB_BuiltinOverworld_SurfaceHeight_Func(world_seed, x, z);



//#define LAB_ROCKY_ALTITUDE (72)
#define LAB_ROCKY_ALTITUDE (50)
#define LAB_SNOWY_ALTITUDE (100)

    float snow_offset = 0.5+0.5*LAB_SimplexNoiseChord2DSN(world_seed^3483554, x*0.01, z*0.01, 2);


    if(sheight >= (int)(LAB_NextRandom(random)&7) + LAB_SNOWY_ALTITUDE + (int)(snow_offset*16))
    {
        int down = 1+LAB_FloorDivPow2(sheight - LAB_SNOWY_ALTITUDE + (LAB_NextRandom(random)&15u), 16);
        int treshold = 3;

        int upw_count = LAB_BuiltinOverworld_UpwardsSlopeCount(world_seed, x, z, 1, 3, sheight-down+1, INT_MAX, random);
        if(upw_count >= treshold)
            return LAB_SURFACE_BIOME_SNOWY_MOUNTAIN;
        return LAB_SURFACE_BIOME_MOUNTAIN;
    }
    if(sheight >= (int)(LAB_NextRandom(random)&31) + LAB_ROCKY_ALTITUDE)
    {
        int ry = sheight + (LAB_NextRandom(random) & 15);
        int treshold = ry > 125 ? -1 : ry > 100 ? 0 : ry > 75 ? 1 : 2;

        int upw_count = LAB_BuiltinOverworld_UpwardsSlopeCount(world_seed, x, z, 1, 3, sheight-2+1, INT_MAX, random);
        if(upw_count >= 4-treshold)
            return LAB_SURFACE_BIOME_MEADOW;
        return LAB_SURFACE_BIOME_MOUNTAIN;
    }

    if(temperature < -0.3)
    {
        if(humidity < -0.3)
            return LAB_SURFACE_BIOME_BIRCH_FOREST;
        else if(humidity < 0.3)
            return LAB_SURFACE_BIOME_FOREST;
        else
            return LAB_SURFACE_BIOME_TAIGA;
    }
    if(temperature <  0.3) return LAB_SURFACE_BIOME_PLAINS;
    else return LAB_SURFACE_BIOME_DESERT;
}