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

    return LAB_SimplexNoise2D(xx*0.05*0.01 , zz*0.05*0.01 ) * 0.7
         + LAB_SimplexNoise2D(xx*0.05*0.002, zz*0.05*0.002) * 1
         + LAB_SimplexNoise2D(xx*0.05*0.004, zz*0.05*0.004) * 0.3;
}

float LAB_BuiltinOverworld_BiomeHumidity_Func(uint64_t world_seed, int x, int z)
{
    int xx = ((world_seed&0xffff)^0x6543)+x;
    int zz = ((world_seed&0xffff)^0x789a)+z;

    return LAB_SimplexNoise2D(xx*0.05*0.03, zz*0.05*0.03);
}



LAB_SurfaceBiomeID LAB_BuiltinOverworld_SurfaceBiome_Func(uint64_t world_seed, int x, int z, LAB_Random* random)
{
    uint64_t r = LAB_NextRandom(random);

    float temperature = LAB_BuiltinOverworld_BiomeTemperature_Func(world_seed, x+(r&0x3f), z+(r>>6&0x3f));
    float humidity    = LAB_BuiltinOverworld_BiomeHumidity_Func(world_seed, x+(r>>12&0x3f), z+(r>>18&0x3f));

    int sheight = LAB_BuiltinOverworld_SurfaceHeight_Func(world_seed, x, z);



//#define LAB_ROCKY_ALTITUDE (72)
#define LAB_ROCKY_ALTITUDE (50)

    if(sheight >= (int)(LAB_NextRandom(random)&31) + LAB_ROCKY_ALTITUDE)
    {
        int ry = sheight + (LAB_NextRandom(random) & 15);
        int treshold = ry > 125 ? -1 : ry > 100 ? 0 : ry > 75 ? 1 : 2;

        size_t count = 0;

        int xz = LAB_NextRandom(random);
        int dd = 1;
        int xx = x+(xz      & dd)-(xz >>  4 & dd);
        int zz = z+(xz >> 8 & dd)-(xz >> 12 & dd);
        int dd2 = 3;
        struct { int dx, dz; } points[] = {
            { dd2, 0 }, { -dd2, 0 }, { 0, dd2 }, { 0, -dd2 }
        };
        for(size_t i = 0; i < LAB_LEN(points); ++i)
        {
            int dx = points[i].dx;
            int dz = points[i].dz;
            if(LAB_BuiltinOverworld_SurfaceHeight_Func(world_seed, xx+dx, zz+dz) > sheight-2)
                count++;
        }

        if(count >= LAB_LEN(points)-treshold)
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