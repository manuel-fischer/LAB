#include "LAB_gen_overworld_structures.h"


void LAB_Gen_Overworld_Tree(LAB_Placer* p, LAB_Random* rnd)
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
        if(x==0 && z==0 && y <= h)
            LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_WOOD);

        else if(x*x+(y-h)*(y-h)+z*z <= r*r+2)
            LAB_Placer_SetBlock(p, x, y, z, &LAB_BLOCK_LEAVES);
    }
}

void LAB_Gen_Overworld_Tower(LAB_Placer* p, LAB_Random* rnd)
{
    // tree height
    uint64_t R = LAB_NextRandom(rnd);
    int h = 8+4*(R    & 1);
    int r = 2+(R>>1 & 1);
    //if(LAB_Placer_IsInside...)

    LAB_Block* corner,* wall,* floor,* ceiling;
    switch((R>>2)&3)
    {
        case 0:
        case 1:
            corner  = &LAB_BLOCK_SMOOTHSTONE;
            wall    = &LAB_BLOCK_STONEBRICKS;
            floor   = &LAB_BLOCK_COBBLESTONE;
            ceiling = &LAB_BLOCK_WOOD_PLANKS_DARK;
        break;

        case 2:
            corner  = &LAB_BLOCK_SMOOTHBASALT;
            wall    = &LAB_BLOCK_BASALTBRICKS;
            floor   = &LAB_BLOCK_BASALTCOBBLE;
            ceiling = &LAB_BLOCK_WOOD_PLANKS;
        break;

        case 3:
            corner  = &LAB_BLOCK_SMOOTHMARBLE;
            wall    = &LAB_BLOCK_MARBLEBRICKS;
            floor   = &LAB_BLOCK_MARBLECOBBLE;
            ceiling = &LAB_BLOCK_WOOD_PLANKS_DARK;
        break;
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
