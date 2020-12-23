#include "LAB_gen_overworld_structures.h"

#include "LAB_attr.h"
#include "LAB_gen_overworld_shape.h"

LAB_STATIC void LAB_Gen_Overworld_DirtPatch(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Plant(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Bush(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Rock(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Tree(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Overworld_Tower(LAB_Placer* p, LAB_Random* rnd);

LAB_STATIC void LAB_Gen_Cave_CeilingCrystal(LAB_Placer* p, LAB_Random* rnd);
LAB_STATIC void LAB_Gen_Cave_FloorCrystal(LAB_Placer* p, LAB_Random* rnd);


const LAB_StructureLayer overworld_layers[] =
{ //    salt   probability   count      height range           place                           func
  //                        <?   >?       <?   >?
    {0x93475493,     32,     1,   5,     -80,  80,     LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_DirtPatch},
    {0x91827364,    256,     0,   1,      20, 200,     LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Rock},
    {0x32547698,    256,     0,   4,     -80,  60,     LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Bush},
    {0x56789abc,    256,     7,  70,     -80,  70,     LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Plant},
    {0x13579bdf,    256,     0,   3,     -80,  30,     LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tree},
    {0xfdb97531,      2,     1,   1,     -80,  10,     LAB_Gen_PlaceOnSurface,        LAB_Gen_Overworld_Tower},

    {0x21436587,    256,     0,   7, INT_MIN, -50,     LAB_Gen_PlaceOnCaveCeiling,    LAB_Gen_Cave_CeilingCrystal},
    {0x78563412,    256,     0,   7, INT_MIN, -50,     LAB_Gen_PlaceOnCaveFloor,      LAB_Gen_Cave_FloorCrystal},
};
const size_t overworld_layers_count = sizeof(overworld_layers)/sizeof(overworld_layers[0]);



/***** Surface *****/
LAB_STATIC void LAB_Gen_Overworld_DirtPatch(LAB_Placer* p, LAB_Random* rnd)
{
    uint64_t R = LAB_NextRandom(rnd);
    int r = 1+(R&7);
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
    uint64_t R = LAB_NextRandom(rnd);
    int h = 4+(R    & 3);
    int r = 2+(R>>2 & 1);
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

        if((x==-r||x==r || z==-r||z==r) && (x==z||x==-z))
            LAB_Placer_SetBlock(p, x, y, z, corner);
        else if(y<h)
        {
            if(x==-r||x==r || z==-r||z==r)
            {
                if(y>=0&&(x==0||z==0)&&(y==0||(y&3)==1))
                    LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_AIR);
                else
                    LAB_Placer_SetBlock(p, x, y, z, wall);
            }
            else if(y<0)
                LAB_Placer_SetBlock(p, x, y, z, floor);

            else if((y&3)==3)
                LAB_Placer_SetBlock(p, x, y, z, ceiling);

            else
                LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_AIR);
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

    int h = (LAB_NextRandom(rnd)&3)+2;
    for(int y = 0; h > 0; --y, --h)
    {
        LAB_Placer_SetBlockIfBlock(p, 0, y, 0, light, &LAB_BLOCK_AIR);
    }
}

LAB_STATIC void LAB_Gen_Cave_FloorCrystal(LAB_Placer* p, LAB_Random* rnd)
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

    int h = (LAB_NextRandom(rnd)&3)+2;
    for(int y = 0; h > 0; ++y, --h)
    {
        LAB_Placer_SetBlockIfBlock(p, 0, y, 0, light, &LAB_BLOCK_AIR);
    }
}
