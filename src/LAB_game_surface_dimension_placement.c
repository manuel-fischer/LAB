#include "LAB_game_surface_dimension_placement.h"

typedef struct LAB_SurfaceDimension_PlaceOnSurface_Ctx
{
    const LAB_SurfaceDimension* dim;
} LAB_SurfaceDimension_PlaceOnSurface_Ctx;
bool LAB_SurfaceDimension_PlaceOnSurface_Func(const void* vctx, uint64_t world_seed, int* x, int* y, int* z)
{
    const LAB_SurfaceDimension_PlaceOnSurface_Ctx* ctx = vctx;
    const LAB_SurfaceDimension* dim = ctx->dim;

    *y = 1+LAB_SurfaceDimension_SurfaceHeight(dim, world_seed, *x, *z);
    return !LAB_SurfaceDimension_IsCave(dim, world_seed, *x, *y-1, *z);
        //&& !LAB_Gen_River_Func((LAB_GenOverworld*)gen, *x, *z);
}
LAB_Game_IStructurePlacement LAB_SurfaceDimension_PlaceOnSurface(LAB_SurfaceDimension* dim)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructurePlacement, LAB_SurfaceDimension_PlaceOnSurface_Func,
        (LAB_SurfaceDimension_PlaceOnSurface_Ctx) { .dim = dim }
    );
}

typedef struct LAB_SurfaceDimension_PlaceOnCaveWall_Ctx
{
    const LAB_SurfaceDimension* dim;
    LAB_DirIndex into_wall;
} LAB_SurfaceDimension_PlaceOnCaveWall_Ctx;
bool LAB_SurfaceDimension_PlaceOnCaveWall_Func(const void* vctx, uint64_t world_seed, int* x, int* y, int* z)
{
    const LAB_SurfaceDimension_PlaceOnCaveWall_Ctx* ctx = vctx;
    const LAB_SurfaceDimension* dim = ctx->dim;
    LAB_DirIndex f = ctx->into_wall;

    // do not place inside the rock
    bool prev_is_cave = LAB_SurfaceDimension_IsCave(dim, world_seed, *x, *y, *z);

    int prev_pos[3];
    int pos[3] = {*x, *y, *z};
    
    int* t = &pos[LAB_FACE_AXIS(f)];
    int* pt = &prev_pos[LAB_FACE_AXIS(f)];
    int dt = LAB_FACE_POSITIVE(f) ? +1 : -1;

    for(int i = 0; i < 16; ++i)
    {
        //if(!prev_is_cave) return false;

        memcpy(prev_pos, pos, sizeof pos);
        *t += dt;
        bool is_cave = LAB_SurfaceDimension_IsCave(dim, world_seed, pos[0], pos[1], pos[2]);

        if(prev_is_cave && !is_cave)
        {
            int y_surface = LAB_SurfaceDimension_SurfaceHeight(dim, world_seed, *x, *z);

            if(y_surface <= prev_pos[1]) return false;
            *x = prev_pos[0];
            *y = prev_pos[1];
            *z = prev_pos[2];

            return true;
        }

        prev_is_cave = is_cave;

        // chunk boundary: wrap back to other side of the chunk
        if((*t & ~15) != (*pt & ~15))
        {
            *t -= dt*16;
            *pt -= dt*16;
            prev_is_cave = LAB_SurfaceDimension_IsCave(dim, world_seed, prev_pos[0], prev_pos[1], prev_pos[2]);
        }
    }
    return false;
}
LAB_Game_IStructurePlacement LAB_SurfaceDimension_PlaceOnCaveWall(LAB_SurfaceDimension* dim, LAB_DirIndex into_wall)
{
    LAB_ASSERT(LAB_DirIndex_IsValid(into_wall));
    return LAB_CREATE_FUNC(LAB_Game_IStructurePlacement, LAB_SurfaceDimension_PlaceOnCaveWall_Func,
        (LAB_SurfaceDimension_PlaceOnCaveWall_Ctx)
        {
            .dim = dim,
            .into_wall = into_wall,
        }
    );
}