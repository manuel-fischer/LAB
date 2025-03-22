#include "LAB_builtin_overworld.h"
#include "LAB_builtin_blocks.h"

#include "LAB_builtin_structures.h"

LAB_SurfaceDimension* LAB_builtin_overworld;

#define LAB_BUILTIN_VIS
    #include "LAB_builtin_overworld_biomes_list.h"
#undef LAB_BUILTIN_VIS

LAB_Err LAB_BuiltinOverworld_Init(void)
{
    LAB_SurfaceDimension* dim = LAB_SurfaceDimension_CreateRegister();

    dim->surface_height = LAB_BuiltinOverworld_SurfaceHeight_Func;
    dim->is_cave = LAB_BuiltinOverworld_IsCave_Func;
    dim->surface_biome = LAB_BuiltinOverworld_SurfaceBiome_Func;

    LAB_TRY(LAB_SurfaceDimension_CreateBiome(dim, &LAB_SURFACE_BIOME_FOREST, (LAB_SurfaceBiome_Blocks) {
        .surface_block = LAB_BLOCK_GRASS,
        .ground_block = LAB_BLOCK_DIRT,
        .rock_block = LAB_BLOCK_STONE.raw,
        .ocean_block = LAB_BLOCK_WATER,
    }));

    LAB_TRY(LAB_SurfaceDimension_CreateBiome(dim, &LAB_SURFACE_BIOME_BIRCH_FOREST, (LAB_SurfaceBiome_Blocks) {
        .surface_block = LAB_BLOCK_GRASS,
        .ground_block = LAB_BLOCK_DIRT,
        .rock_block = LAB_BLOCK_STONE.raw,
        .ocean_block = LAB_BLOCK_WATER,
    }));

    LAB_TRY(LAB_SurfaceDimension_CreateBiome(dim, &LAB_SURFACE_BIOME_TAIGA, (LAB_SurfaceBiome_Blocks) {
        .surface_block = LAB_BLOCK_FOREST_SOIL,
        .ground_block = LAB_BLOCK_DIRT,
        .rock_block = LAB_BLOCK_STONE.raw,
        .ocean_block = LAB_BLOCK_WATER,
    }));

    LAB_TRY(LAB_SurfaceDimension_CreateBiome(dim, &LAB_SURFACE_BIOME_PLAINS, (LAB_SurfaceBiome_Blocks) {
        .surface_block = LAB_BLOCK_GRASS,
        .ground_block = LAB_BLOCK_DIRT,
        .rock_block = LAB_BLOCK_STONE.raw,
        .ocean_block = LAB_BLOCK_WATER,
    }));

    LAB_TRY(LAB_SurfaceDimension_CreateBiome(dim, &LAB_SURFACE_BIOME_DESERT, (LAB_SurfaceBiome_Blocks) {
        .surface_block = LAB_BLOCK_SAND,
        .ground_block = LAB_BLOCK_SANDSTONE.layered,
        .rock_block = LAB_BLOCK_STONE.raw,
        .ocean_block = LAB_BLOCK_WATER,
    }));

    LAB_TRY(LAB_SurfaceDimension_CreateBiome(dim, &LAB_SURFACE_BIOME_MOUNTAIN, (LAB_SurfaceBiome_Blocks) {
        .surface_block = LAB_BLOCK_STONE.raw,
        .ground_block = LAB_BLOCK_STONE.raw,
        .rock_block = LAB_BLOCK_STONE.raw,
        .ocean_block = LAB_BLOCK_WATER,
    }));

    LAB_TRY(LAB_SurfaceDimension_CreateBiome(dim, &LAB_SURFACE_BIOME_SNOWY_MOUNTAIN, (LAB_SurfaceBiome_Blocks) {
        .surface_block = LAB_BLOCK_SNOW,
        .ground_block = LAB_BLOCK_SNOW,
        .rock_block = LAB_BLOCK_STONE.raw,
        .ocean_block = LAB_BLOCK_WATER,
    }));

    LAB_TRY(LAB_SurfaceDimension_CreateBiome(dim, &LAB_SURFACE_BIOME_MEADOW, (LAB_SurfaceBiome_Blocks) {
        .surface_block = LAB_BLOCK_GRASS,
        .ground_block = LAB_BLOCK_STONE.raw,
        .rock_block = LAB_BLOCK_STONE.raw,
        .ocean_block = LAB_BLOCK_WATER,
    }));

    LAB_TRY(LAB_BuiltinStructures_Init(dim));

    return LAB_OK;
}