#include "LAB_gen_overworld_structures.h"

#include <limits.h> // INT_MIN

#include "LAB_attr.h"
#include "LAB_gen_overworld_shape.h"
#include "LAB_simplex_noise.h"
#include "LAB_gen_tags.h"
#include "LAB_builtin_blocks.h"

LAB_STATIC void LAB_Gen_Overworld_DirtPatch(LAB_Placer* p, LAB_Random* rnd, const void* param);
LAB_STATIC void LAB_Gen_Overworld_Plant(LAB_Placer* p, LAB_Random* rnd, const void* param);
LAB_STATIC void LAB_Gen_Overworld_Bush(LAB_Placer* p, LAB_Random* rnd, const void* param);
LAB_STATIC void LAB_Gen_Overworld_Rock(LAB_Placer* p, LAB_Random* rnd, const void* param);
LAB_STATIC void LAB_Gen_Overworld_Tree(LAB_Placer* p, LAB_Random* rnd, const void* param);
LAB_STATIC void LAB_Gen_Overworld_LargeTree(LAB_Placer* p, LAB_Random* rnd, const void* param);
LAB_STATIC void LAB_Gen_Overworld_Tower(LAB_Placer* p, LAB_Random* rnd, const void* param);
LAB_STATIC void LAB_Gen_Overworld_House(LAB_Placer* p, LAB_Random* rnd, const void* param);

LAB_STATIC void LAB_Gen_Cave_CeilingCrystal(LAB_Placer* p, LAB_Random* rnd, const void* param);
LAB_STATIC void LAB_Gen_Ore(LAB_Placer* p, LAB_Random* rnd, const void* param);


typedef struct LAB_Gen_Overworld_Tree_Params
{
    LAB_BlockID* wood;
    LAB_BlockID* leaves;
} LAB_Gen_Overworld_Tree_Params;

static const LAB_Gen_Overworld_Tree_Params oak_tree   = { &LAB_BLOCK_WOOD, &LAB_BLOCK_LEAVES };
static const LAB_Gen_Overworld_Tree_Params birch_tree = { &LAB_BLOCK_BIRCH_WOOD, &LAB_BLOCK_BIRCH_LEAVES };


// TODO: UB: floor is a library function name and
//           could possibly be defined as a macro
typedef struct LAB_Gen_BuildingPalette
{
    LAB_BlockID* corner,* wall,* floor,* ceiling;
} LAB_Gen_BuildingPalette;

static const LAB_Gen_BuildingPalette stone_palette = {
    .corner  = &LAB_BLOCK_STONE.smooth,
    .wall    = &LAB_BLOCK_STONE.bricks,
    .floor   = &LAB_BLOCK_STONE.cobble,
    .ceiling = &LAB_BLOCK_WOOD_PLANKS_DARK,
};
static const LAB_Gen_BuildingPalette basalt_palette = {
    .corner  = &LAB_BLOCK_BASALT.smooth,
    .wall    = &LAB_BLOCK_BASALT.bricks,
    .floor   = &LAB_BLOCK_BASALT.cobble,
    .ceiling = &LAB_BLOCK_WOOD_PLANKS,
};
static const LAB_Gen_BuildingPalette marble_palette = {
    .corner  = &LAB_BLOCK_MARBLE.smooth,
    .wall    = &LAB_BLOCK_MARBLE.bricks,
    .floor   = &LAB_BLOCK_MARBLE.cobble,
    .ceiling = &LAB_BLOCK_WOOD_PLANKS_DARK,
};
static const LAB_Gen_BuildingPalette desert_palette = {
    .corner  = &LAB_BLOCK_SANDSTONE.smooth,
    .wall    = &LAB_BLOCK_SANDSTONE.bricks,
    .floor   = &LAB_BLOCK_SANDSTONE.cobble,
    .ceiling = &LAB_BLOCK_WOOD_PLANKS_DARK,
};


#define F(percent) (int)((percent)*(float)(LAB_MAX_PROBABILITY))
const LAB_StructureLayer overworld_layers[] =
{ //       salt    probability   count      height range  radius         place                           func                        tags                      param
  //                            <?   >?       <?   >?
    {0x91827364,  F(1.    ),     0,   1,   20+32, 200+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Rock,        LAB_GEN_TAG_MOUNTAINS,         NULL                         },
    {0x32547698,  F(1.    ),     0,   4,       0,  60+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Bush,        LAB_GEN_TAG_BUSHES,            NULL                         },
    {0x56789abc,  F(1.    ),     3,  35,       0,  70+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Plant,       LAB_GEN_TAG_GRASS,             &LAB_BLOCK_TALLGRASS         },
    {0xd8f8e945,  F(1.    ),     3,  35,       0,  70+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Plant,       LAB_GEN_TAG_GRASS,             &LAB_BLOCK_TALLERGRASS       },
    {0xfd874567,  F(1  /2.),     1,  10,       0,  70+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Plant,       LAB_GEN_TAG_FLOWERS,           &LAB_BLOCK_RED_TULIP         },
    {0xfbc90356,  F(1  /2.),     1,  10,       0,  70+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Plant,       LAB_GEN_TAG_FLOWERS,           &LAB_BLOCK_YELLOW_TULIP      },
    {0xfbc90356,  F(1.    ),    64, 192,       0,  33+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Plant,       LAB_GEN_TAG_BIRCH_FOREST,      &LAB_BLOCK_FALLEN_LEAVES     },
    {0x93475493,  F(1  /8.),     1,   5,       0,  80+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_DirtPatch,   LAB_GEN_TAG_FOREST,            NULL                         },
    {0x13579bdf,  F(1.    ),     3,   5,       0,  30+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tree,        LAB_GEN_TAG_FOREST,            &oak_tree                    },
    {0x13579bdf,  F(1 /32.),     1,   1,       0,  30+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tree,        LAB_GEN_TAG_PLAINS,            &oak_tree                    },
    {0x85484857,  F(1  /2.),     0,   2,       0,  30+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tree,        LAB_GEN_TAG_FOREST,            &birch_tree                  },
    {0x85484857,  F(1.    ),     3,   6,       0,  30+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tree,        LAB_GEN_TAG_BIRCH_FOREST,      &birch_tree                  },
    {0x85484857,  F(1.    ),     4,   5,       0,  30+32,      2,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_LargeTree,   LAB_GEN_TAG_TAIGA,             NULL                         },
    {0xfdb97531,  F(1/256.),     1,   1,  -80+32,  10+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tower,       LAB_GEN_TAG_RUINS,             &stone_palette               },
    {0x7845fdf3,  F(1/512.),     1,   1,  -80+32,  10+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tower,       LAB_GEN_TAG_RUINS,             &marble_palette              },
    {0xf89df80f,  F(1/128.),     1,   1,       0,  10+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_House,       LAB_GEN_TAG_RUINS,             &marble_palette              },
    {0x783f45df,  F(1/ 64.),     1,   1,  -80+32,  10+32,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tower,       LAB_GEN_TAG_DESERT,            &desert_palette              },
    {0x783f45df,  F(1/ 64.),     1,   1, INT_MIN,      0,      1,    LAB_Gen_PlaceOnCaveFloor,      LAB_Gen_Overworld_Tower,       LAB_GEN_TAG_CAVE,              &basalt_palette              },

    {0x21436587,  F(    1.),     0,   5, INT_MIN, -50+32,      1,    LAB_Gen_PlaceOnCaveCeiling,    LAB_Gen_Cave_CeilingCrystal,   LAB_GEN_TAG_CAVE,              NULL                         },

    {0x78578947,  F(   0.7),     1,   5, INT_MIN, 100+32,      1,    LAB_Gen_PlaceAnywhere,         LAB_Gen_Ore,                   LAB_GEN_TAG_MINERALS,          &LAB_BLOCK_IRON_ORE          },
    {0x67856465,  F(   0.7),     1,   4, INT_MIN, 100+32,      1,    LAB_Gen_PlaceAnywhere,         LAB_Gen_Ore,                   LAB_GEN_TAG_MINERALS,          &LAB_BLOCK_COPPER_ORE        },
    {0x08765324,  F(   0.5),     1,   4, INT_MIN, 100+32,      1,    LAB_Gen_PlaceAnywhere,         LAB_Gen_Ore,                   LAB_GEN_TAG_MINERALS,          &LAB_BLOCK_GOLD_ORE          },
    {0x21567890,  F(   0.6),     1,   4, INT_MIN, 100+32,      1,    LAB_Gen_PlaceAnywhere,         LAB_Gen_Ore,                   LAB_GEN_TAG_MINERALS,          &LAB_BLOCK_SILVER_ORE        },
    {0x67565456,  F(   0.4),     1,   4, INT_MIN, 100+32,      1,    LAB_Gen_PlaceAnywhere,         LAB_Gen_Ore,                   LAB_GEN_TAG_MINERALS,          &LAB_BLOCK_URANIUM_ORE       },
};
const size_t overworld_layers_count = sizeof(overworld_layers)/sizeof(overworld_layers[0]);
#undef F



/***** Surface *****/
LAB_STATIC void LAB_Gen_Overworld_DirtPatch(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R&7)+5;
    for(int z =-r; z <= r; ++z)
    for(int y =-r; y <= r; ++y)
    for(int x =-r; x <= r; ++x)
    {
        R = LAB_NextRandom(rnd);
        int d = R%r;
        if(x*x+y*y+z*z < d*d)
            LAB_Placer_SetBlockIfBlock(p, x, y, z, LAB_BLOCK_DIRT, LAB_BLOCK_GRASS);
    }
}

LAB_STATIC void LAB_Gen_Overworld_Plant(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    LAB_BlockID plant = *(const LAB_BlockID*)param;

    LAB_Placer_SetBlockIfBlock(p, 0, 0, 0, plant, LAB_BLOCK_AIR);
}

LAB_STATIC void LAB_Gen_Overworld_Bush(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    // tree height
    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R & 1);
    //if(LAB_Placer_IsInside...)

    for(int z = 0; z <= r; ++z)
    for(int y =-1; y <= 1; ++y)
    for(int x = 0; x <= r; ++x)
    {
        LAB_Placer_SetBlockIfAll(p, x, y, z, LAB_BLOCK_LEAVES, LAB_BLOCK_TAG_REPLACEABLE);
    }
}


LAB_STATIC void LAB_Gen_Overworld_Rock(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    // tree height
    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R & 1);
    //if(LAB_Placer_IsInside...)

    for(int z =-r; z <= 1; ++z)
    for(int y =-r; y <= 1; ++y)
    for(int x =-r; x <= 1; ++x)
    {
        if(z*(z+1)+y*(y+1)+x*(x+1)+(LAB_NextRandom(rnd)&1)+1 < r*r)
            LAB_Placer_SetBlockIfAll(p, x, y, z, LAB_BLOCK_STONE.cobble, LAB_BLOCK_TAG_REPLACEABLE);
    }
}


LAB_STATIC void LAB_Gen_Overworld_Tree(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    // tree height
    //if((LAB_NextRandom(rnd)&3) == 0) LAB_Gen_Overworld_DirtPatch(p, rnd, NULL);

    const LAB_Gen_Overworld_Tree_Params* blocks = param;

    uint64_t R = LAB_NextRandom(rnd);
    int h = 4+(R    & 3)+2;
    int r = 2+(R>>2 & (1+2));
    //if(LAB_Placer_IsInside...)

    LAB_Placer_SetBlock(p, 0, -1, 0, LAB_BLOCK_DIRT);
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
            LAB_Placer_SetBlock(p, x, y, z, *blocks->wood);

        else if(x*x+yy*yy+z*z <= r*r+1+(int)(LAB_NextRandom(rnd)&1))
            LAB_Placer_SetBlockIfAll(p, x, y, z, *blocks->leaves, LAB_BLOCK_TAG_REPLACEABLE);
    }
}

LAB_STATIC void LAB_Gen_Overworld_LargeTree(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    uint64_t R = LAB_NextRandom(rnd);
    int l = 3+(R    &  7);
    int h = l+(R>>3 & 15);
    int r = 5+(R>>7 & (1+2));
    //if(LAB_Placer_IsInside...)

    for(int z = 0; z < 2; ++z)
    for(int x = 0; x < 2; ++x)
    {
        LAB_Placer_SetBlock(p, x, -1, z, LAB_BLOCK_DIRT);
        LAB_Placer_SetBlock(p, x, h+r+1, z, LAB_BLOCK_SPRUCE_LEAVES);
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
            LAB_Placer_SetBlock(p, x, y, z, LAB_BLOCK_SPRUCE_WOOD);

        else if(y > l && hr2*(x*(x-1)+z*(z-1)) < hry2*((r-rr)*(r-(rr>>1))-(int)(LAB_NextRandom(rnd)&7)))
            LAB_Placer_SetBlockIfAll(p, x, y, z, LAB_BLOCK_SPRUCE_LEAVES, LAB_BLOCK_TAG_REPLACEABLE);
    }
}

LAB_STATIC void LAB_Gen_Overworld_Tower(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    const LAB_Gen_BuildingPalette* LAB_RESTRICT palette = param;
    
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
            LAB_Placer_SetBlock(p, x, y, z, *palette->corner);


        else if(y<h)
        {
            if(x==-r||x==r || z==-r||z==r)
            {
                if(y>=0&&(x==0||z==0)&&(y==0||(y&3)==1))
                    (void)0;//LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_AIR);
                else
                    LAB_Placer_SetBlock(p, x, y, z, *palette->wall);
            }
            else if(y<0)
                LAB_Placer_SetBlock(p, x, y, z, *palette->floor);

            else if((y&3)==3)
                LAB_Placer_SetBlock(p, x, y, z, *palette->ceiling);

            else
                (void)0;//LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_AIR);
        }
        else if(y < h+2&&(x==-r||x==r || z==-r||z==r)&&(y==h||((x^z)&1)==0))
            LAB_Placer_SetBlock(p, x, y, z, *palette->corner);
    }
}


LAB_STATIC void LAB_Gen_Overworld_House(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    const LAB_Gen_BuildingPalette* LAB_RESTRICT palette = param;

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
            LAB_Placer_SetBlock(p, x, y, z, LAB_BLOCK_CLAY.bricks);
        }
        else if(d > 0 && x_wall >= 0 && z_wall >= 0)
        {
            if(x_wall==0 || z_wall==0)
            {
                if(x_wall==0 && z_wall==0)
                    LAB_Placer_SetBlock(p, x, y, z, *palette->corner);
                else if(y>=0&&(x==0||z==0))
                {
                    int k = (door&1) ? x : z;
                    if(door&2) k = -k;

                    if(k > 0 && y < 2)
                        LAB_Placer_SetBlock(p, x, y, z, LAB_BLOCK_AIR);
                    else if((y&3)==1)
                        LAB_Placer_SetBlock(p, x, y, z, LAB_BLOCK_GLASS);
                    else
                        LAB_Placer_SetBlock(p, x, y, z, *palette->wall);
                }
                else
                    LAB_Placer_SetBlock(p, x, y, z, *palette->wall);
            }
            else
            {
                if(y >= 0)
                    LAB_Placer_SetBlock(p, x, y, z, LAB_BLOCK_AIR);
                else
                    LAB_Placer_SetBlock(p, x, y, z, *palette->floor);
            }
        }
    }
}















/***** Cave *****/
LAB_STATIC void LAB_Gen_Cave_CeilingCrystal(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    /** TODO

        unsigned amount = LAB_MIN(2*LAB_MAX(0, -y), 100);

        if((LAB_NextRandom(&random)&255) < amount)

     **/



    static LAB_BlockID* const LIGHTS[4] = {
        &LAB_BLOCK_BLUE_CRYSTAL,
        &LAB_BLOCK_YELLOW_CRYSTAL,
        &LAB_BLOCK_GREEN_CRYSTAL,
        &LAB_BLOCK_RED_CRYSTAL,
    };

    LAB_BlockID light = *LIGHTS[LAB_NextRandom(rnd)&3];

    #if 0
    int h = (LAB_NextRandom(rnd)&3)+2;
    for(int y = 0; h > 0; --y, --h)
    {
        LAB_Placer_SetBlockIfBlock(p, 0, y, 0, light, &LAB_BLOCK_AIR);
    }
    #elif 0
    LAB_Placer_SetBlockIfBlock(p, 0, 0, 0, &LAB_BLOCK_LAPIZ, &LAB_BLOCK_AIR);
    int r = LAB_NextRandomRange(rnd, 1, 5);
    for(int x = -r; x <= r; ++x)
    for(int z = -r; z <= r; ++z)
    {
        int dc = x*x+z*z;
        int maxh = (r*r - dc)-(4*r-6);
        if(maxh > 0)
        {
            //int h = maxh;
            int h = (LAB_NextRandomRange(rnd, 0, maxh) + 2);
            for(int y = 0; h > 0; --y, --h)
            {
                int m = y -1;
                LAB_Placer_SetBlockIfBlock(p, (x*m)/2, y,   (z*m)/2, light, &LAB_BLOCK_AIR);
                LAB_Placer_SetBlockIfBlock(p, (x*m)/2, y-1, (z*m)/2, light, &LAB_BLOCK_AIR);
            }
        }
    }
    #else
    //LAB_Placer_SetBlockIfBlock(p, 0, 0, 0, &LAB_BLOCK_LAPIZ, &LAB_BLOCK_AIR);

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
                LAB_Placer_SetBlockIfAll(p, x+xo, -y, z+zo, light, LAB_BLOCK_TAG_REPLACEABLE);
        }
    }
    #endif
}




LAB_STATIC void LAB_Gen_Ore(LAB_Placer* p, LAB_Random* rnd, const void* param)
{
    LAB_BlockID ore_block = *(const LAB_BlockID*)param;

    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R & 1);

    for(int z = 0; z <= r; ++z)
    for(int y =-1; y <= 1; ++y)
    for(int x = 0; x <= r; ++x)
    {
        LAB_Placer_SetBlockIfBlock(p, x, y, z, ore_block, LAB_BLOCK_STONE.raw);
        //LAB_Placer_SetBlockIfAll(p, x, y, z, ore_block, LAB_BLOCK_TAG_RAW_STONE); // << TODO
    }
}