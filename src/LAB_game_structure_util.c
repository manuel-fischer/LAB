#include "LAB_game_structure_util.h"

#include "LAB_intmathlib.h"
#include "LAB_random.h"


/*LAB_Game_IStructureDensity
LAB_Game_IStructurePlacement*/

#define LAB_GAME_MAX_PROB (1<<16)
typedef struct LAB_Game_ProbAndCount_Ctx
{
    uint32_t probability; // range [0, LAB_GAME_MAX_PROB] instead of [0.0, 1.0]
    int16_t min, max;
} LAB_Game_ProbAndCount_Ctx;
LAB_STATIC LAB_Game_StructureDensity LAB_Game_ProbAndCount_Density_Func(const void* vctx, LAB_Game_StructureSpan span, uint64_t world_seed, LAB_Random* random, int cx, int cy, int cz)
{
    const LAB_Game_ProbAndCount_Ctx* ctx = vctx;


    bool has_some = true;
    if(ctx->probability != LAB_GAME_MAX_PROB)
        has_some = ((LAB_NextRandom(random) & (LAB_GAME_MAX_PROB-1)) < ctx->probability);

    LAB_Game_StructureDensity density;
    if(!has_some)
        density.count = 0;
    else if(ctx->min == ctx->max)
        density.count = ctx->min;
    else
        density.count = LAB_NextRandomRange(random, ctx->min, ctx->max+1);

    return density;
}
/**
 * Variables per span
 */
LAB_Game_IStructureDensity LAB_Game_StructureDensity_ProbAndCount(double probability, int min, int max)
{
    uint32_t prob = probability * (double)LAB_GAME_MAX_PROB;

    return LAB_CREATE_FUNC(LAB_Game_IStructureDensity, LAB_Game_ProbAndCount_Density_Func, (LAB_Game_ProbAndCount_Ctx)
    {
        .probability = prob,
        .min = min, .max = max,
    });
}





typedef struct LAB_Game_RestrictY_Ctx
{
    int16_t min, max;
    int16_t cmin, cmax;
} LAB_Game_RestrictY_Ctx;
LAB_STATIC LAB_Game_StructureDensity LAB_Game_RestrictYRange_Density_Func(const void* vctx, LAB_Game_StructureSpan span, uint64_t world_seed, LAB_Random* random, int cx, int cy, int cz)
{
    const LAB_Game_RestrictY_Ctx* ctx = vctx;

    LAB_Game_StructureDensity density = { .count = 0 };

    if(ctx->cmin <= cy && cy < ctx->cmax)
    if(ctx->cmin <= cy && cy < ctx->cmax)
        density = LAB_CALL_DECORATED(LAB_Game_StructureDensity_Func, ctx, density, span,/**/ world_seed, random, cx, cy, cz);

    return density;
}
LAB_STATIC bool LAB_Game_RestrictYRange_Placement_Func(const void* vctx, uint64_t world_seed, int* x, int* y, int* z)
{
    const LAB_Game_RestrictY_Ctx* ctx = vctx;

    bool place = LAB_CALL_DECORATED(LAB_Game_StructurePlacement_Func, ctx, false,/**/ world_seed, x, y, z);
    return place && ctx->min <= *y && *y < ctx->max;
}
bool LAB_Game_Structure_RestrictYRange_Impl(LAB_Game_Structure* s, int min, int max)
{
    const LAB_Game_RestrictY_Ctx ctx = {
        .min  = min == INT_MIN ? INT16_MIN : min,
        .max  = max == INT_MAX ? INT16_MAX : max,
        .cmin = min == INT_MIN ? INT16_MIN : LAB_FloorDivPow2(min, 16),
        .cmax = max == INT_MAX ? INT16_MAX : LAB_CeilDivPow2(max, 16),
    };

    LAB_Game_IStructureDensity density;
    density = LAB_DECORATE_FUNC(LAB_Game_IStructureDensity,
        LAB_Game_RestrictYRange_Density_Func, ctx, s->density);
    
    if(!density.ctx) return false;

    LAB_Game_IStructurePlacement placement;
    placement = LAB_DECORATE_FUNC(LAB_Game_IStructurePlacement,
        LAB_Game_RestrictYRange_Placement_Func, ctx, s->placement);

    if(!placement.ctx)
    {
        LAB_FREE_FUNC(density);
        return false;
    }

    LAB_FREE_FUNC(s->density);
    LAB_FREE_FUNC(s->placement);

    s->density = density;
    s->placement = placement;

    return true;
}


LAB_Game_Structure LAB_Game_Structure_RestrictYRange_Move(LAB_Game_Structure s, int min, int max)
{
    if(!LAB_Game_Structure_RestrictYRange_Impl(&s, min, max))
        memset(&s, 0, sizeof s);

    return s;
}


LAB_STATIC
bool LAB_Game_PlaceAnywhere_Func(const void* vctx, uint64_t world_seed, int* x, int* y, int* z)
{
    return true;
}
LAB_Game_IStructurePlacement LAB_Game_PlaceAnywhere(void)
{
    return LAB_CREATE_EMPTY_FUNC(LAB_Game_IStructurePlacement, LAB_Game_PlaceAnywhere_Func);
}