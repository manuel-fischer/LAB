#include "LAB_gen_overworld_structures.h"

#include <limits.h> // INT_MIN

#include "LAB_attr.h"
#include "LAB_gen_overworld_shape.h"
#include "LAB_simplex_noise.h"

LAB_STATIC void LAB_Gen_Overworld_DirtPatch(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Plant(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Bush(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Rock(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Tree(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Tower(LAB_Placer* p, LAB_Random* rnd);

LAB_STATIC void LAB_Gen_Cave_CeilingCrystal(LAB_Placer* p, LAB_Random* rnd);


const LAB_StructureLayer overworld_layers[] =
{ //    salt   probability   count      height range  radius         place                           func
  //                        <?   >?       <?   >?
    {0x93475493,     32,     1,   5,     -80,  80,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_DirtPatch},
    {0x91827364,    256,     0,   1,      20, 200,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Rock},
    {0x32547698,    256,     0,   4,     -80,  60,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Bush},
    {0x56789abc,    256,     7,  70,     -80,  70,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Plant},
    {0x13579bdf,    256,     0,   3,     -80,  30,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tree},
    {0xfdb97531,      2,     1,   1,     -80,  10,      1,    LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tower},

    {0x21436587,    256,     0,   5, INT_MIN, -50,      1,    LAB_Gen_PlaceOnCaveCeiling,    LAB_Gen_Cave_CeilingCrystal},
};
const size_t overworld_layers_count = sizeof(overworld_layers)/sizeof(overworld_layers[0]);



/***** Surface *****/
LAB_STATIC void LAB_Gen_Overworld_DirtPatch(LAB_Placer* p, LAB_Random* rnd)
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
            LAB_Placer_SetBlockIfBlock(p, x, y, z, &LAB_BLOCK_DIRT, &LAB_BLOCK_GRASS);
    }
}

LAB_STATIC void LAB_Gen_Overworld_Plant(LAB_Placer* p, LAB_Random* rnd)
{
    LAB_Block* plant = LAB_NextRandom(rnd)&1 ? &LAB_BLOCK_TALLGRASS : &LAB_BLOCK_TALLERGRASS;

    LAB_Placer_SetBlockIfBlock(p, 0, 0, 0, plant, &LAB_BLOCK_AIR);
}

LAB_STATIC void LAB_Gen_Overworld_Bush(LAB_Placer* p, LAB_Random* rnd)
{
    // tree height
    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R & 1);
    //if(LAB_Placer_IsInside...)

    for(int z = 0; z <= r; ++z)
    for(int y =-1; y <= 1; ++y)
    for(int x = 0; x <= r; ++x)
    {
        LAB_Placer_SetBlockIfBlock(p, x, y, z, &LAB_BLOCK_LEAVES, &LAB_BLOCK_AIR);
    }
}


LAB_STATIC void LAB_Gen_Overworld_Rock(LAB_Placer* p, LAB_Random* rnd)
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
            LAB_Placer_SetBlockIfBlock(p, x, y, z, &LAB_BLOCK_STONE_COBBLE, &LAB_BLOCK_AIR);
    }
}


LAB_STATIC void LAB_Gen_Overworld_Tree(LAB_Placer* p, LAB_Random* rnd)
{
    // tree height
    if((LAB_NextRandom(rnd)&3) == 0) LAB_Gen_Overworld_DirtPatch(p, rnd);

    uint64_t R = LAB_NextRandom(rnd);
    int h = 4+(R    & 3)+2;
    int r = 2+(R>>2 & (1+2));
    //if(LAB_Placer_IsInside...)

    LAB_Placer_SetBlock(p, 0, -1, 0, &LAB_BLOCK_DIRT);
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
            LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_WOOD);

        else if(x*x+yy*yy+z*z <= r*r+1+(int)(LAB_NextRandom(rnd)&1))
            LAB_Placer_SetBlockIfBlock(p, x, y, z, &LAB_BLOCK_LEAVES, &LAB_BLOCK_AIR);
    }
}

LAB_STATIC void LAB_Gen_Overworld_Tower(LAB_Placer* p, LAB_Random* rnd)
{
    // tree height
    uint64_t R = LAB_NextRandom(rnd);
    int h = 8+4*(R    & 1);
    int r = 2+(R>>1 & 1);
    //if(LAB_Placer_IsInside...)

    // noise offset
    uint64_t R2 = LAB_NextRandom(rnd);
    int dx = R2       & 0xffff;
    int dz = R2 >> 16 & 0xffff;

    // TODO: UB: floor is a library function name and
    //           could possibly be defined as a macro
    LAB_Block* corner,* wall,* floor,* ceiling;
    switch((R>>2)&3)
    {
        case 0:
        case 1:
            corner  = &LAB_BLOCK_STONE_SMOOTH;
            wall    = &LAB_BLOCK_STONE_BRICKS;
            floor   = &LAB_BLOCK_STONE_COBBLE;
            ceiling = &LAB_BLOCK_WOOD_PLANKS_DARK;
        break;

        case 2:
            corner  = &LAB_BLOCK_BASALT_SMOOTH;
            wall    = &LAB_BLOCK_BASALT_BRICKS;
            floor   = &LAB_BLOCK_BASALT_COBBLE;
            ceiling = &LAB_BLOCK_WOOD_PLANKS;
        break;

        case 3:
            corner  = &LAB_BLOCK_MARBLE_SMOOTH;
            wall    = &LAB_BLOCK_MARBLE_BRICKS;
            floor   = &LAB_BLOCK_MARBLE_COBBLE;
            ceiling = &LAB_BLOCK_WOOD_PLANKS_DARK;
        break;
        default: LAB_UNREACHABLE();
    }

    for(int z = -r; z <= r; ++z)
    for(int y = -4; y <  h+3; ++y)
    for(int x = -r; x <= r; ++x)
    {
        if(y > (LAB_SimplexNoise2D((x+dx)*0.1, (z+dz)*0.1)*(0.5 + 0.2) + (0.5 - 0.3))*((h+3) - -4  +  2)) continue;

        //LAB_Block* b = &LAB_BLOCK_AIR;

        if((x==-r||x==r || z==-r||z==r) && (x==z||x==-z))
            LAB_Placer_SetBlock(p, x, y, z, corner);


        else if(y<h)
        {
            if(x==-r||x==r || z==-r||z==r)
            {
                if(y>=0&&(x==0||z==0)&&(y==0||(y&3)==1))
                    (void)0;//LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_AIR);
                else
                    LAB_Placer_SetBlock(p, x, y, z, wall);
            }
            else if(y<0)
                LAB_Placer_SetBlock(p, x, y, z, floor);

            else if((y&3)==3)
                LAB_Placer_SetBlock(p, x, y, z, ceiling);

            else
                (void)0;//LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_AIR);
        }
        else if(y < h+2&&(x==-r||x==r || z==-r||z==r)&&(y==h||((x^z)&1)==0))
            LAB_Placer_SetBlock(p, x, y, z, corner);
    }
}















/***** Cave *****/
LAB_STATIC void LAB_Gen_Cave_CeilingCrystal(LAB_Placer* p, LAB_Random* rnd)
{
    /** TODO

        unsigned amount = LAB_MIN(2*LAB_MAX(0, -y), 100);

        if((LAB_NextRandom(&random)&255) < amount)

     **/



    LAB_Block*const LIGHTS[4] = {
        &LAB_BLOCK_BLUE_CRYSTAL,
        &LAB_BLOCK_YELLOW_CRYSTAL,
        &LAB_BLOCK_GREEN_CRYSTAL,
        &LAB_BLOCK_RED_CRYSTAL,
    };

    LAB_Block* light = LIGHTS[LAB_NextRandom(rnd)&3];

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
            h = LAB_NextRandomRange(rnd, 2, 16);
        }
        else
        {
            dx = /*((i&1)*2-1)*/ + LAB_NextRandomRange(rnd, -5, 5+1);
            dz = /*((i&2)  -1)*/ + LAB_NextRandomRange(rnd, -5, 5+1);
            h = LAB_NextRandomRange(rnd, 2, 10+6);
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
                LAB_Placer_SetBlockIfBlock(p, x+xo, -y, z+zo, light, &LAB_BLOCK_AIR);
        }
    }
    #endif
}
