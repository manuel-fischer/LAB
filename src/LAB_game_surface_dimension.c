#include "LAB_game_surface_dimension.h"

#include "LAB_obj.h"

#include "LAB_game_surface_dimension_gen.h"

LAB_STATIC
void LAB_SurfaceDimension_SpawnPoint_Func(const void* ctx, uint64_t world_seed, double* x, double* y, double* z);

LAB_STATIC
void LAB_SurfaceBiome_Destroy(LAB_SurfaceBiome_Entry* biome)
{
    LAB_Game_StructureArray_Destroy(&biome->structures);
}

LAB_STATIC
LAB_SurfaceDimension* LAB_SurfaceDimension_Obj(LAB_OBJ_Action action, LAB_SurfaceDimension* dim)
{
    LAB_BEGIN_OBJ(action);

    LAB_OBJ(dim = LAB_Calloc(1, sizeof(LAB_SurfaceDimension)),
            LAB_Free(dim),

    LAB_OBJ(LAB_PolyBuffer_Create(&dim->structure_args),
            LAB_PolyBuffer_Destroy(&dim->structure_args),

    LAB_OBJ(LAB_Game_StructureArray_Create(&dim->common_structures),
            LAB_Game_StructureArray_Destroy(&dim->common_structures),
    
    LAB_OBJ(LAB_Game_StructureSpanArray_Create(&dim->spans),
            LAB_Game_StructureSpanArray_Destroy(&dim->spans),

    LAB_OBJ(LAB_ARRAY_CREATE_EMPTY(LAB_SurfaceDimension_biomes(dim)),
            LAB_ARRAY_DESTROY_WITH(LAB_SurfaceDimension_biomes(dim), LAB_SurfaceBiome_Destroy),

    {
        LAB_YIELD_OBJ(dim);
    });););););
    
    LAB_END_OBJ(NULL);
}


void LAB_SurfaceDimension_Destroy_Func(void* ctx)
{
    LAB_SurfaceDimension* dim = ctx;
    LAB_SurfaceDimension_Obj(LAB_OBJ_DESTROY, dim);
}

bool LAB_SurfaceDimension_FinishInit_Func(void* ctx)
{
    LAB_SurfaceDimension* dim = ctx;

    bool success = true;

    success &= LAB_Game_StructureArray_Sort(&dim->spans, &dim->common_structures);
    for(size_t i = 0; i < dim->biomes_count; ++i)
        success &= LAB_Game_StructureArray_Sort(&dim->spans, &dim->biomes[i].structures);

    return success;
}


LAB_MUT_Game_Dimension
LAB_SurfaceDimension* LAB_SurfaceDimension_CreateRegister(void)
{
    LAB_SurfaceDimension* dim;

    LAB_OBJ(dim = LAB_SurfaceDimension_Obj(LAB_OBJ_CREATE, NULL),
            LAB_SurfaceDimension_Obj(LAB_OBJ_DESTROY, dim),
    {
        if(LAB_Dimension_Register(&(LAB_IDimension)
        {
            .ctx = dim,
            .destroy = LAB_SurfaceDimension_Destroy_Func,
            .finish_init = LAB_SurfaceDimension_FinishInit_Func,
            .generate_chunk = LAB_SurfaceDimension_GenerateChunk_Func,
            .spawn_point = LAB_SurfaceDimension_SpawnPoint_Func,
        }))
            return dim;
    });

    return NULL;
}









LAB_Err LAB_SurfaceDimension_CreateBiome(LAB_SurfaceDimension* dim, LAB_OUT LAB_SurfaceBiomeID* out_biome_id, LAB_SurfaceBiome_Blocks def)
{
    LAB_SurfaceBiome_Entry* biome;
    LAB_ARRAY_APPEND(LAB_SurfaceDimension_biomes(dim), 1, &biome);
    if(!biome) return LAB_RAISE_C();

    biome->blocks = def;

    LAB_Game_StructureArray_Create(&biome->structures); // always suceeds

    *out_biome_id = biome - dim->biomes;
    return LAB_OK;
}


LAB_Err LAB_SurfaceDimension_AddStructure(LAB_SurfaceDimension* dim, LAB_SurfaceBiomeID biome, uint64_t salt, const LAB_Game_Structure* structure)
{
    //if(structure->span.cx == 0) return false;
    LAB_ASSERT(structure->span.cx != 0);

    LAB_Game_StructureArray* array = biome == LAB_SurfaceBiomeID_NULL
                                   ? &dim->common_structures
                                   : &dim->biomes[biome].structures;

    LAB_Game_Structure_Opt* s_opt;
    LAB_ARRAY_APPEND(LAB_Game_StructureArray_structures(array), 1, &s_opt);
    if(!s_opt) return LAB_RAISE_C();

    s_opt->span = LAB_Game_StructureSpanID(&dim->spans, structure->span);
    s_opt->salt = salt;
    bool success = true;
    success &= LAB_OPTIMIZE_FUNC(&dim->structure_args, structure->density,   &s_opt->density);
    success &= LAB_OPTIMIZE_FUNC(&dim->structure_args, structure->placement, &s_opt->placement);
    success &= LAB_OPTIMIZE_FUNC(&dim->structure_args, structure->structure, &s_opt->structure);

    return success ? LAB_OK : LAB_RAISE_C();
}




LAB_STATIC
void LAB_SurfaceDimension_SpawnPoint_Func(const void* ctx, uint64_t world_seed, double* x, double* y, double* z)
{
    int height = LAB_SurfaceDimension_SurfaceHeight(ctx, world_seed, (int)floor(*x), (int)floor(*z));
    *y = height+3;
}