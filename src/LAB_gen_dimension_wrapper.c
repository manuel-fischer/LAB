#include "LAB_gen_dimension_wrapper.h"


bool LAB_GenDimensionWrapper_Gen_Func(void* user, LAB_Chunk* chunk, int x, int y, int z)
{
    const LAB_GenDimensionWrapper_Ctx* ctx = user;
    return ctx->dim.generate_chunk(ctx->dim.ctx, ctx->world_seed, chunk, x, y, z);
}