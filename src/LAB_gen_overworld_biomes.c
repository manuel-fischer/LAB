#include "LAB_gen_overworld_biomes.h"

#include "LAB_gen_tags.h"
#include "LAB_simplex_noise.h"
#include "LAB_block.h"
#include "LAB_random.h"

const LAB_Gen_Biome LAB_biome_forest =
{
    .tags           = LAB_GEN_TAG_FOREST | LAB_GEN_TAG_BUSHES | LAB_GEN_TAG_GRASS
                    | LAB_GEN_TAG_CAVE | LAB_GEN_TAG_MOUNTAINS,
    .surface_block  = &LAB_BLOCK_GRASS,
    .ground_block   = &LAB_BLOCK_DIRT,
};

const LAB_Gen_Biome LAB_biome_birch_forest =
{
    .tags           = LAB_GEN_TAG_BIRCH_FOREST | LAB_GEN_TAG_BUSHES | LAB_GEN_TAG_GRASS | LAB_GEN_TAG_FLOWERS
                    | LAB_GEN_TAG_CAVE | LAB_GEN_TAG_MOUNTAINS,
    .surface_block  = &LAB_BLOCK_GRASS, //LAB_BLOCK_BIRCH_LEAVES,
    .ground_block   = &LAB_BLOCK_DIRT,
};

const LAB_Gen_Biome LAB_biome_plains =
{
    .tags           = LAB_GEN_TAG_BUSHES | LAB_GEN_TAG_GRASS | LAB_GEN_TAG_FLOWERS
                    | LAB_GEN_TAG_RUINS
                    | LAB_GEN_TAG_CAVE | LAB_GEN_TAG_MOUNTAINS,
    .surface_block  = &LAB_BLOCK_GRASS, //_PLAINS,
    .ground_block   = &LAB_BLOCK_DIRT,
};

const LAB_Gen_Biome LAB_biome_desert =
{
    .tags           = LAB_GEN_TAG_CAVE | LAB_GEN_TAG_MOUNTAINS,
    .surface_block  = &LAB_BLOCK_SAND,
    .ground_block   = &LAB_BLOCK_SANDSTONE,
};


float LAB_Gen_Biome_Temperature_Func(LAB_GenOverworld* gen, int x, int z)
{
    int xx = ((gen->seed&0xffff)^0x1234)+x;
    int zz = ((gen->seed&0xffff)^0x9876)+z;

    return LAB_SimplexNoise2D(xx*0.05*0.01 , zz*0.05*0.01 ) * 0.7
         + LAB_SimplexNoise2D(xx*0.05*0.002, zz*0.05*0.002) * 1
         + LAB_SimplexNoise2D(xx*0.05*0.004, zz*0.05*0.004) * 0.3;
}

float LAB_Gen_Biome_Humidity_Func(LAB_GenOverworld* gen, int x, int z)
{
    int xx = ((gen->seed&0xffff)^0x6543)+x;
    int zz = ((gen->seed&0xffff)^0x789a)+z;

    return LAB_SimplexNoise2D(xx*0.05*0.03, zz*0.05*0.03);
}

const LAB_Gen_Biome* LAB_Gen_Biome_Func(LAB_GenOverworld* gen, int x, int z, uint64_t rand_val)
{
    uint64_t r;
    r = rand_val;

    float temperature = LAB_Gen_Biome_Temperature_Func(gen, x+(r&0xf), z+(r>>4&0xf));
    float humidity    = LAB_Gen_Biome_Humidity_Func(gen, x+(r>>8&0xf), z+(r>>12&0xf));

    /*float temperature = LAB_Gen_Biome_Temperature_Func(gen, x, z) + (rand_val&0xf)*(0.4/15)-0.2;
    float humidity    = LAB_Gen_Biome_Humidity_Func(gen, x, z);*/

    if(temperature < -0.3)
    {
        if(humidity < 0)
            return &LAB_biome_forest;
        else
            return &LAB_biome_birch_forest;
    }
    if(temperature <  0.3) return &LAB_biome_plains;
    else return &LAB_biome_desert;
}
