#pragma once

#include "LAB_array.h"

#include "LAB_game_dimension.h"
#include "LAB_game_structure.h"
#include "LAB_direction.h"
#include "LAB_random.h"
#include "LAB_error_state.h"



typedef struct LAB_SurfaceBiome_Blocks
{
    LAB_BlockID surface_block,
                ground_block,
                rock_block,
                ocean_block;
} LAB_SurfaceBiome_Blocks;


typedef struct LAB_SurfaceBiome_Entry
{
    LAB_SurfaceBiome_Blocks blocks;
    LAB_Game_StructureArray structures;
} LAB_SurfaceBiome_Entry;

typedef size_t LAB_SurfaceBiomeID;
#define LAB_SurfaceBiomeID_NULL ((LAB_SurfaceBiomeID)-1)


typedef int (*LAB_SurfaceHeight_Func)(uint64_t world_seed, int x, int z);
typedef bool (*LAB_Cave_Func)(uint64_t world_seed, int x, int y, int z);
typedef LAB_SurfaceBiomeID (*LAB_SurfaceBiome_Func)(uint64_t world_seed, int x, int z, LAB_Random* random);


typedef struct LAB_SurfaceDimension
{
    LAB_Game_StructureArray common_structures;
    LAB_Game_StructureSpanArray spans;

    LAB_DEF_ARRAY(LAB_SurfaceBiome_Entry, biomes);
    #define LAB_SurfaceDimension_biomes(o) \
        LAB_REF_ARRAY(LAB_SurfaceBiome_Entry, o, biomes)

    // Function contexts, common and for all biomes
    LAB_PolyBuffer structure_args;

    LAB_SurfaceHeight_Func surface_height;
    LAB_Cave_Func is_cave;
    LAB_SurfaceBiome_Func surface_biome;

} LAB_SurfaceDimension;

LAB_MUT_Game_Dimension
LAB_SurfaceDimension* LAB_SurfaceDimension_CreateRegister(void);


LAB_Err LAB_SurfaceDimension_CreateBiome(LAB_SurfaceDimension* dim, LAB_OUT LAB_SurfaceBiomeID* out_biome_id, LAB_SurfaceBiome_Blocks def);
LAB_Err LAB_SurfaceDimension_AddStructure(LAB_SurfaceDimension* dim, LAB_SurfaceBiomeID biome, uint64_t salt, const LAB_Game_Structure* structure);

LAB_INLINE
int LAB_SurfaceDimension_SurfaceHeight(const LAB_SurfaceDimension* dim, uint64_t world_seed, int x, int z)
{
    LAB_ASSERT(dim->surface_height);
    return dim->surface_height(world_seed, x, z);
}

LAB_INLINE
int LAB_SurfaceDimension_IsCave(const LAB_SurfaceDimension* dim, uint64_t world_seed, int x, int y, int z)
{
    LAB_ASSERT(dim->is_cave);
    return dim->is_cave(world_seed, x, y, z);
}

LAB_INLINE
LAB_SurfaceBiomeID LAB_SurfaceDimension_SurfaceBiome(const LAB_SurfaceDimension* dim, uint64_t world_seed, int x, int y, int z, LAB_Random* random)
{
    (void)y;
    LAB_ASSERT(dim->surface_biome);
    return dim->surface_biome(world_seed, x, z, random);
}

LAB_INLINE
size_t LAB_SurfaceDimension_EnumerateBiomes(const LAB_SurfaceDimension* dim, uint64_t world_seed,
    LAB_SurfaceBiomeID* biomes, size_t biomes_count,
    int x0, int y0, int z0,
    int dx, int dy, int dz,
    LAB_Random* random
)
{
    size_t count = 0;

    for(int z = z0; z < z0+dz; z+=dz>>2)
    for(int y = y0; y < y0+dy; y+=dy>>2)
    for(int x = x0; x < x0+dx; x+=dx>>2)
    {
        LAB_SurfaceBiomeID id = LAB_SurfaceDimension_SurfaceBiome(dim, world_seed, x, y, z, random);
        for(size_t i = 0; i < count; ++i)
            if(biomes[i] == id)
                goto found;

        if(count < biomes_count)
            biomes[count++] = id;
        found:;
    }
    return count;
}
