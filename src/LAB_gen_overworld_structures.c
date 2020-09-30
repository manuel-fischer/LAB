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
