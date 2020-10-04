#include "LAB_world_light.h"

// minimum light level for completely unlit block with that dia
#define LAB_DARK_LIGHT(dia) ((((dia)>>4) & 0x0f0f0fu) | 0xff000000u)

// heuristic lighting algorithm
/*static void LAB_PrepareLight(LAB_Chunk* chunk, LAB_Color default_color)
{
    for(int z = 0; z < 16; ++z)
    for(int x = 0; x < 16; ++x)
    {
        int xz_off = x | z<<8;
        LAB_Color l = default_color;
        for(int y = 15; y >= 0; --y)
        {
            int xyz_off = xz_off | y<<4;
            LAB_Color dia = chunk->blocks[xyz_off]->dia;
            l = LAB_MulColor(l, dia);
            l = LAB_MaxColor(l, LAB_DARK_LIGHT(dia));
            chunk->light[xyz_off] = l;
        }
    }
}*/

// heuristic lighting algorithm
// run first time when the chunk has been created
static void LAB_PrepareLight(LAB_Chunk* chunk, LAB_Chunk* chunk_above, LAB_Color default_color)
{
    for(int z = 0; z < 16; ++z)
    for(int x = 0; x < 16; ++x)
    {
        int xz_off = x | z<<8;
        LAB_Color l = chunk_above ? chunk_above->light[xz_off] : default_color;
        for(int y = 15; y >= 0; --y)
        {
            int xyz_off = xz_off | y<<4;
            LAB_Color dia = chunk->blocks[xyz_off]->dia;
            l = LAB_MulColor(l, dia);
            l = LAB_MaxColor(l, LAB_DARK_LIGHT(dia));
            chunk->light[xyz_off] = l;
        }
    }
}

static LAB_Color LAB_CalcLight(LAB_World* world, LAB_Chunk*const chunks[27], int x, int y, int z, LAB_Color default_color)
{
    //return ~0;
    #if 0
    LAB_Color lum;
    //lum = LAB_RGB(16, 16, 16);
    //lum = LAB_GetNeighborhoodBlock(chunks, x,y,z)->lum;
    LAB_Chunk* cnk = chunks[1+3+9];
    int off = LAB_CHUNK_OFFSET(x, y, z);
    if(!(cnk->blocks[off]->flags & LAB_BLOCK_OPAQUE))
    {
        LAB_Color dia = cnk->blocks[off]->dia;
        lum = LAB_RGB(16, 16, 16);
        for(int i = 0; i < 6; ++i)
        {
            const int* o = LAB_offset[i];
            int nlum;

            LAB_Block* block = LAB_GetNeighborhoodBlock(chunks, x+o[0], y+o[1], z+o[2]);
            if(block->flags & LAB_BLOCK_EMISSIVE)
                nlum = block->lum;
            else
            {
                nlum = LAB_GetNeighborhoodLight(chunks, x+o[0], y+o[1], z+o[2], default_color);
                if(i!=3 || (nlum&0xffffff) != 0xffffff)
                    nlum = nlum - (nlum>>2 & 0x3f3f3f);
            }
            lum = LAB_MaxColor(lum, LAB_MulColor(nlum, dia));
        }
    }
    else
    {
        lum = LAB_GetNeighborhoodBlock(chunks, x,y,z)->lum;
    }
    return lum;
    #else
    LAB_Color lum;
    //LAB_Chunk* cnk = chunks[1+3+9];
    //int off = LAB_CHUNK_OFFSET(x, y, z);


    //LAB_Color dia = cnk->blocks[off]->dia;
    LAB_Color dia = LAB_GetNeighborhoodBlock(chunks, x, y, z)->dia;

    //lum = LAB_RGB(16, 16, 16);
    lum = LAB_DARK_LIGHT(dia);
    //lum = LAB_MaxColor(lum, cnk->blocks[off]->lum);
    lum = LAB_MaxColor(lum, LAB_GetNeighborhoodLight(chunks, x, y, z, default_color));
    for(int i = 0; i < 6; ++i)
    {
        //const int* o = LAB_offset[i];
        int nlum, nlum1, nlum2;

        LAB_Block* block = LAB_GetNeighborhoodBlock(chunks, x+LAB_OX(i), y+LAB_OY(i), z+LAB_OZ(i));
        nlum1 = block->lum;
        nlum2 = LAB_GetNeighborhoodLight(chunks, x+LAB_OX(i), y+LAB_OY(i), z+LAB_OZ(i), default_color);
        //if(i!=3 || (nlum2&0xffffff) != 0xffffff)
        //    nlum2 = nlum2 - (nlum2>>2 & 0x3f3f3f);

        // simplified
        //uint32_t mask = -(i!=3); // alpha needs to be 0
        uint32_t is_white_24 = ((nlum2&0xffffffu)+1)&0x01000000u;
        uint32_t mask = is_white_24-(unsigned)((!!is_white_24) & (i==3)); // alpha needs to be 0
        nlum2 = nlum2 - ((nlum2>>2 & 0x3f3f3f)&~mask); // don't care about alpha here

        nlum = LAB_MaxColor(nlum1, nlum2);
        lum = LAB_MaxColor(lum, LAB_MulColor(nlum, dia));
    }

    return lum;
    #endif
}

/**
 *  Check for wrong light values in the center chunk and update values
 *  that are to be updated. These positions are pushed into a queue
 *
 *  Return format: 0b?SNUDEW in binary
 *  ?: Any light changed
 *  S, N, U, D, E, W: Neighboring chunk needs to be updated
 */
//LAB_HOT
//static int LAB_UpdateLight_First(LAB_World* world, LAB_Chunk* chunks[27], LAB_Color default_color, int queuesize, int queue[]);

/**
 *  Return format: ?SNUDEW in binary
 *  ?: Any light changed
 *  S, N, U, D, E, W: Neighboring chunk needs to be updated
 */
LAB_HOT
static int LAB_CheckLight(LAB_World* world, LAB_Chunk*const chunks[27], LAB_Color default_color)
{
    int changed = 0;

    #if 0
    LAB_Chunk* cnk = chunks[1+3+9];
    for(int z = 0; z < 16; ++z)
    for(int y =15; y >= 0; --y)
    for(int x = 0; x < 16; ++x)
    {
        int off = LAB_CHUNK_OFFSET(x, y, z);

        if(cnk->light[off] != LAB_CalcLight(world, chunks, x, y, z, default_color))
        {
            if(x==0)  changed |=  1;
            if(x==15) changed |=  2;
            if(y==0)  changed |=  4;
            if(y==15) changed |=  8;
            if(z==0)  changed |= 16;
            if(z==15) changed |= 32;
            changed |= 64;
        }
    }
    return changed;
    #else
    for(int z = 0-1; z < 16+1; ++z)
    for(int y =15+1; y >= 0-1; --y)
    for(int x = 0-1; x < 16+1; ++x)
    {
        int off;
        LAB_Chunk* cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &off);
        if(cnk && cnk->light[off] != LAB_CalcLight(world, chunks, x, y, z, default_color))
        {
            if(x<=0)  changed |=  1;
            if(x>=15) changed |=  2;
            if(y<=0)  changed |=  4;
            if(y>=15) changed |=  8;
            if(z<=0)  changed |= 16;
            if(z>=15) changed |= 32;
            changed |= 64;
        }
    }
    return changed;
    #endif
}

LAB_HOT                                           // TODO: |--------------------| not used
int LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27], int cx, int cy, int cz)
{
    #if 0
    for(int i = 0; i < 27; ++i)
    {
        if(chunks[i] == 0) return 0;
    }
    #endif

    LAB_Chunk* cnk = chunks[1+3+9];
    if(!cnk) return 0;

    //LAB_Color default_color = cy <= -5 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);
    LAB_Color default_color = cy <  -2 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);
    int faces_changed = LAB_CheckLight(world, chunks, default_color);
    if(!faces_changed) return 0;
    faces_changed &= 63; // remove change bit

    #if 0
    memset(cnk->light, 0, sizeof cnk->light);
    #else
    LAB_PrepareLight(cnk, chunks[1+2*3+9], default_color);
    int faces_changed2 = LAB_CheckLight/*Pos*/(world, chunks, default_color);
    if(!faces_changed2) return faces_changed;
    faces_changed |= faces_changed2;
    #endif

    #if 1
    int changed = 1;
    int change_count = 0;
    while(changed)
    {
        changed = 0;
        for(int z = 0; z < 16; ++z)
        for(int y =15; y >= 0; --y)
        for(int x = 0; x < 16; ++x)
        {
            int off = LAB_CHUNK_OFFSET(x, y, z);
            LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, default_color);
            if(cnk->light[off] != lum)
            {
                cnk->light[off] = lum;
                if(x==0)  faces_changed |=  1;
                if(x==15) faces_changed |=  2;
                if(y==0)  faces_changed |=  4;
                if(y==15) faces_changed |=  8;
                if(z==0)  faces_changed |= 16;
                if(z==15) faces_changed |= 32;
                changed = 1;
            }
        }
        change_count++;
    }
    #elif 0
    // expanded by 15 blocks version
    int changed = 1;
    int change_count = 0;
    while(changed)
    {
        changed = 0;
        for(int z =  0-15; z < 16+15; ++z)
        for(int y = 15+15; y >= 0-15; --y)
        for(int x =  0-15; x < 16+15; ++x)
        {
            int block_index;
            LAB_Chunk* cnk;
            cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &block_index);
            if(cnk)
            {
                LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, default_color);

                if(cnk->light[block_index] != lum)
                {
                    cnk->light[block_index] = lum;
                    if(x<=0)  faces_changed |=  1;
                    if(x>=15) faces_changed |=  2;
                    if(y<=0)  faces_changed |=  4;
                    if(y>=15) faces_changed |=  8;
                    if(z<=0)  faces_changed |= 16;
                    if(z>=15) faces_changed |= 32;
                    changed = 1;
                }
            }
        }
        change_count++;
    }
    #else

    #endif
    return faces_changed;
}
