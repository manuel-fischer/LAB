#include "LAB_builtin_structure_funcs.h"

#include "LAB_builtin_blocks.h"
#include "LAB_simplex_noise.h"

typedef struct LAB_Block_Ctx
{
    LAB_BlockID block;
} LAB_Block_Ctx;

typedef struct LAB_BlockReplacement_Ctx
{
    LAB_BlockID block, replaced;
} LAB_BlockReplacement_Ctx;



#define LAB_PRIO_PLANT          10
#define LAB_PRIO_LEAVES         20
#define LAB_PRIO_LOG            30
#define LAB_PRIO_ROCK           40
#define LAB_PRIO_ORE            40
#define LAB_PRIO_BUILDING_AIR   50
#define LAB_PRIO_BUILDING       60




LAB_STATIC
void LAB_Builtin_Patch_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_BlockReplacement_Ctx* ctx = vctx;

    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R&7)+5;
    for(int z =-r; z <= r; ++z)
    for(int y =-r; y <= r; ++y)
    for(int x =-r; x <= r; ++x)
    {
        R = LAB_NextRandom(rnd);
        int d = R%r;
        if(x*x+y*y+z*z < d*d)
            LAB_Placer_SetBlockIfBlock(p, x, y, z, ctx->block, ctx->replaced);
    }
}
LAB_Game_IStructure LAB_Builtin_Patch(LAB_BlockID block, LAB_BlockID replaced)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_Patch_Func, (LAB_BlockReplacement_Ctx)
    {
        .block = block,
        .replaced = replaced,
    });
}





LAB_STATIC
void LAB_Builtin_Plant_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_Block_Ctx* ctx = vctx;

    LAB_Placer_SetBlockWithPriority(p, 0, 0, 0, ctx->block, LAB_PRIO_PLANT);
}
LAB_Game_IStructure LAB_Builtin_Plant(LAB_BlockID block)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_Plant_Func, (LAB_Block_Ctx)
    {
        .block = block
    });
}





LAB_STATIC
void LAB_Builtin_Bush_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_Block_Ctx* ctx = vctx;

    // tree height
    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R & 1);
    //if(LAB_Placer_IsInside...)

    for(int z = 0; z <= r; ++z)
    for(int y =-1; y <= 1; ++y)
    for(int x = 0; x <= r; ++x)
    {
        if(!LAB_Placer_GetLandscapeBit(p, x, y, z))
            LAB_Placer_SetBlockWithPriority(p, x, y, z, ctx->block, LAB_PRIO_LEAVES);
    }
}
LAB_Game_IStructure LAB_Builtin_Bush(LAB_BlockID block)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_Bush_Func, (LAB_Block_Ctx)
    {
        .block = block
    });
}





LAB_STATIC
void LAB_Builtin_Rock_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_Block_Ctx* ctx = vctx;

    // tree height
    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R & 1);
    //if(LAB_Placer_IsInside...)

    for(int z =-r; z <= 1; ++z)
    for(int y =-r; y <= 1; ++y)
    for(int x =-r; x <= 1; ++x)
    {
        if(z*(z+1)+y*(y+1)+x*(x+1)+(LAB_NextRandom(rnd)&1)+1 < r*r)
            LAB_Placer_SetBlockWithPriority(p, x, y, z, ctx->block, LAB_PRIO_ROCK);
    }
}
LAB_Game_IStructure LAB_Builtin_Rock(LAB_BlockID block)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_Rock_Func, (LAB_Block_Ctx)
    {
        .block = block
    });
}




LAB_STATIC
void LAB_Builtin_Tree_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_Builtin_TreePalette* ctx = vctx;

    uint64_t R = LAB_NextRandom(rnd);
    int h = 4+(R    & 3)+2;
    int r = 2+(R>>2 & (1+2));
    //if(LAB_Placer_IsInside...)

    LAB_Placer_SetBlock(p, 0, -1, 0, ctx->ground);
    for(int z = -r; z <= r; ++z)
    for(int y =  0; y <= h+r; ++y)
    for(int x = -r; x <= r; ++x)
    {
        int yy = y-h;
        if(r >= 3)
            yy += (x*x+z*z)>>3;
        else
            yy += (x*x+z*z)>>2;

        if(x==0 && z==0 && y <= h)
            LAB_Placer_SetBlockWithPriority(p, x, y, z, ctx->wood, LAB_PRIO_LOG);

        else if(x*x+yy*yy+z*z <= r*r+1+(int)(LAB_NextRandom(rnd)&1))
            LAB_Placer_SetBlockWithPriority(p, x, y, z, ctx->leaves, LAB_PRIO_LEAVES);
    }
}
LAB_Game_IStructure LAB_Builtin_Tree(LAB_Builtin_TreePalette blocks)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_Tree_Func, blocks);
}





LAB_STATIC
void LAB_Builtin_LargeTree_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_Builtin_TreePalette* ctx = vctx;

    uint64_t R = LAB_NextRandom(rnd);
    int l = 3+(R    &  7);
    int h = l+(R>>3 & 15);
    int r = 5+(R>>7 & (1+2));
    //if(LAB_Placer_IsInside...)

    for(int z = 0; z < 2; ++z)
    for(int x = 0; x < 2; ++x)
    {
        LAB_Placer_SetBlock(p, x, -1, z, ctx->ground);
        LAB_Placer_SetBlockWithPriority(p, x, h+r+1, z, ctx->leaves, LAB_PRIO_LEAVES);
    }

    int hr = h+2*r;
    int hr2 = hr*hr;
    int rr = (int)(LAB_NextRandom(rnd)&3);
    for(int y =  0; y <= h+r; ++y, rr = (int)(LAB_NextRandom(rnd)&3))
    for(int z = -r; z <= r; ++z)
    for(int x = -r; x <= r; ++x)
    {
        int hry2 = (hr-y)*(hr-y);

        if((x&~1)==0 && (z&~1)==0 && y <= h)
            LAB_Placer_SetBlockWithPriority(p, x, y, z, ctx->wood, LAB_PRIO_LOG);

        else if(y > l && hr2*(x*(x-1)+z*(z-1)) < hry2*((r-rr)*(r-(rr>>1))-(int)(LAB_NextRandom(rnd)&7)))
            LAB_Placer_SetBlockWithPriority(p, x, y, z, ctx->leaves, LAB_PRIO_LEAVES);
    }
}
LAB_Game_IStructure LAB_Builtin_LargeTree(LAB_Builtin_TreePalette blocks)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_LargeTree_Func, blocks);
}






LAB_STATIC
void LAB_Builtin_Tower_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_Builtin_BuildingPalette* LAB_RESTRICT palette = vctx;
    
    // house dimensions
    uint64_t R = LAB_NextRandom(rnd);
    int h = 8+4*(R    & 1);
    int r = 2+(R>>1 & 1);
    //if(LAB_Placer_IsInside...)

    // noise offset
    uint64_t R2 = LAB_NextRandom(rnd);
    int dx = R2       & 0xffff;
    int dz = R2 >> 16 & 0xffff;

    for(int z = -r; z <= r; ++z)
    for(int y = -4; y <  h+3; ++y)
    for(int x = -r; x <= r; ++x)
    {
        if(y > (LAB_SimplexNoise2D((x+dx)*0.1, (z+dz)*0.1)*(0.5 + 0.2) + (0.5 - 0.3))*((h+3) - -4  +  2)) continue;

        if((x==-r||x==r || z==-r||z==r) && (x==z||x==-z))
            LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->corner, LAB_PRIO_BUILDING);


        else if(y<h)
        {
            if(x==-r||x==r || z==-r||z==r)
            {
                if(y>=0&&(x==0||z==0)&&(y==0||(y&3)==1))
                    (void)0;//LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_AIR);
                else
                    LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->wall, LAB_PRIO_BUILDING);
            }
            else if(y<0)
                LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->floor, LAB_PRIO_BUILDING);

            else if((y&3)==3)
                LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->ceiling, LAB_PRIO_BUILDING);

            else
                (void)0;//LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_AIR);
        }
        else if(y < h+2&&(x==-r||x==r || z==-r||z==r)&&(y==h||((x^z)&1)==0))
            LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->corner, LAB_PRIO_BUILDING);
    }
}
LAB_Game_IStructure LAB_Builtin_Tower(LAB_Builtin_BuildingPalette blocks)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_Tower_Func, blocks);
}






LAB_STATIC
void LAB_Builtin_House_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_Builtin_BuildingPalette* LAB_RESTRICT palette = vctx;

    // house dimensions
    uint64_t R = LAB_NextRandom(rnd);
    int lx = (R   &3)+2;
    int lz = (R>>3&3)+2;
    int h  = 4; //((R>>6&3)<<1)+3;
    int r  = LAB_MIN(lx, lz);

    int door = (R >> 9) & 3;
    int shape = (R >> 12) & 1;

    // noise offset
    //uint64_t R2 = LAB_NextRandom(rnd);
    //int dx = R2       & 0xffff;
    //int dz = R2 >> 16 & 0xffff;

    const int slope_shf = 0;
    for(int z = -lz-1; z <= lz+1; ++z)
    for(int y = -4;    y < h+((r+2)<<slope_shf); ++y)
    for(int x = -lx-1; x <= lx+1; ++x)
    {
        //if(y > (LAB_SimplexNoise2D((x+dx)*0.1, (z+dz)*0.1)*(0.5 + 0.2) + (0.5 - 0.3))*((h+r+2) - -4  +  2)) continue;

        int x_wall = LAB_MIN(lx-x, lx+x);
        int z_wall = LAB_MIN(lz-z, lz+z);

        int f = (y-h-1)>>slope_shf;
        int d = shape ? LAB_MIN(x_wall-f,  z_wall-f)
                      : lx==r ? x_wall-f : z_wall-f;

        if(d == 0)
        {
            LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->roof, LAB_PRIO_BUILDING);
        }
        else if(d > 0 && x_wall >= 0 && z_wall >= 0)
        {
            if(x_wall==0 || z_wall==0)
            {
                if(x_wall==0 && z_wall==0)
                    LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->corner, LAB_PRIO_BUILDING);
                else if(y>=0&&(x==0||z==0))
                {
                    int k = (door&1) ? x : z;
                    if(door&2) k = -k;

                    if(k > 0 && y < 2)
                        LAB_Placer_SetBlockWithPriority(p, x, y, z, LAB_BLOCK_AIR, LAB_PRIO_BUILDING_AIR);
                    else if((y&3)==1)
                        LAB_Placer_SetBlockWithPriority(p, x, y, z, LAB_BLOCK_GLASS, LAB_PRIO_BUILDING);
                    else
                        LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->wall, LAB_PRIO_BUILDING);
                }
                else
                    LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->wall, LAB_PRIO_BUILDING);
            }
            else
            {
                if(y >= 0)
                    LAB_Placer_SetBlockWithPriority(p, x, y, z, LAB_BLOCK_AIR, LAB_PRIO_BUILDING_AIR);
                else
                    LAB_Placer_SetBlockWithPriority(p, x, y, z, palette->floor, LAB_PRIO_BUILDING);
            }
        }
    }
}
LAB_Game_IStructure LAB_Builtin_House(LAB_Builtin_BuildingPalette blocks)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_House_Func, blocks);
}





LAB_STATIC
void LAB_Builtin_CeilingCrystal_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    static LAB_BlockID* const LIGHTS[4] = {
        &LAB_BLOCK_BLUE_CRYSTAL,
        &LAB_BLOCK_YELLOW_CRYSTAL,
        &LAB_BLOCK_GREEN_CRYSTAL,
        &LAB_BLOCK_RED_CRYSTAL,
    };

    LAB_BlockID light = *LIGHTS[LAB_NextRandom(rnd)&3];

    for(int i = LAB_NextRandomRange(rnd, 1, 5); i; --i)
    {
        int xo = LAB_NextRandomRange(rnd, -2, 2+1);
        int zo = LAB_NextRandomRange(rnd, -2, 2+1);
        int dx, dz, h;
        if(i == 1)
        {
            dx = LAB_NextRandomRange(rnd, 0, 5+1)-LAB_NextRandomRange(rnd, 0, 5+1);
            dz = LAB_NextRandomRange(rnd, 0, 5+1)-LAB_NextRandomRange(rnd, 0, 5+1);
            h = LAB_NextRandomRange(rnd, 2, 9);//16);
        }
        else
        {
            dx = /*((i&1)*2-1)*/ + LAB_NextRandomRange(rnd, -5, 5+1);
            dz = /*((i&2)  -1)*/ + LAB_NextRandomRange(rnd, -5, 5+1);
            h = LAB_NextRandomRange(rnd, 2, 9);//10+6);
        }

        int r = LAB_MAX(abs(dx), abs(dz))*h;

        for(int x = -r; x <= r; ++x)
        for(int z = -r; z <= r; ++z)
        for(int y =  0; y <  h; ++y)
        {
            #define F 8
            int xx = x*F-dx*y;
            int zz = z*F-dz*y;
            if(xx*xx + zz*zz < (h - y)*(F*F/4) + 3)
                LAB_Placer_SetBlockWithPriority(p, x+xo, -y, z+zo, light, LAB_PRIO_ORE);
        }
    }
}
LAB_Game_IStructure LAB_Builtin_CeilingCrystal(void)
{
    return LAB_CREATE_EMPTY_FUNC(LAB_Game_IStructure, LAB_Builtin_CeilingCrystal_Func);
}





LAB_STATIC void LAB_Builtin_Ore_Func(const void* vctx, LAB_Placer* p, LAB_Random* rnd)
{
    const LAB_BlockReplacement_Ctx* ctx = vctx;

    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R & 1);

    for(int z = 0; z <= r; ++z)
    for(int y =-1; y <= 1; ++y)
    for(int x = 0; x <= r; ++x)
    {
        LAB_Placer_SetBlockIfBlock(p, x, y, z, ctx->block, ctx->replaced);
        //LAB_Placer_SetBlockIfAll(p, x, y, z, ore_block, LAB_BLOCK_TAG_RAW_STONE); // << TODO
    }
}
LAB_Game_IStructure LAB_Builtin_Ore(LAB_BlockID block, LAB_BlockID replaced)
{
    return LAB_CREATE_FUNC(LAB_Game_IStructure, LAB_Builtin_Ore_Func, (LAB_BlockReplacement_Ctx)
    {
        .block = block,
        .replaced = replaced,
    });
}