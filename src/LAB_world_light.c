#include "LAB_world_light.h"
#include "LAB_debug.h"
#include "LAB_chunk_pseudo.h"
#include "LAB_chunk_neighborhood.h"
// TODO: use pseudo chunks instead of NULL chunks -> no branching required
//       - build chunk neighborhood with these pseudo chunks
// TODO: reduce range of searching for possibly changed block positions
//       - use crammed chunk pos set, stored in the chunks

#include <stdio.h> // DBG

// minimum light level for completely unlit block with that dia
#define LAB_DARK_LIGHT(dia) ((((dia)>>4) & 0x0f0f0fu) | 0xff000000u)
#define LAB_MIN_LIGHT(dia) ((((dia)>>6) & 0x030303u) | 0xff000000u)
#define LAB_LIGHT_FALL_OFF(lum) ((lum) - ((lum)>>3 & 0x1f1f1f)-((lum)>>4 & 0x0f0f0f))


// return blocks that changed
LAB_STATIC LAB_HOT
LAB_CCPS LAB_TickLight_ProcessQuadrant(LAB_Chunk*const chunks[27], int quadrant, bool init, LAB_Color default_color)
{
    LAB_CCPS relit_blocks = 0;

    LAB_Chunk* ctr_cnk = chunks[1+3+9];
    LAB_ASSERT(ctr_cnk);
    LAB_PULL_CONST(int, c_init, 2, init)
    {
        int dd[3];
        int i0[3];

        LAB_UNROLL(3)
        for(int i = 0, q = quadrant; i < 3; ++i, q >>= 1)
        {
            if(q & 1) { dd[i] = 1; i0[i] = 0; } else { dd[i] = -1; i0[i] = 15; }
        }

        for(int zi = 0, z = i0[2]; zi < 16; ++zi, z += dd[2])
        for(int yi = 0, y = i0[1]; yi < 16; ++yi, y += dd[1])
        for(int xi = 0, x = i0[0]; xi < 16; ++xi, x += dd[0])
        {
            int block_index = LAB_XADD3(x, y<<4, z<<8);

            LAB_Block* b = ctr_cnk->blocks[block_index];
            LAB_Color c = b->lum;
            c = LAB_MaxColor(c, LAB_DARK_LIGHT(b->dia));
            LAB_UNROLL(3)
            for(int i = 0; i < 3; ++i)
            {
                int xyz[3] = {x, y, z};
                xyz[i] -= dd[i];

                bool is_down = i==1 && !(quadrant&2);

                int block_index2;
                LAB_Chunk* cnk;
                cnk = LAB_GetNeighborhoodRef(chunks, xyz[0], xyz[1], xyz[2], &block_index2);
                LAB_Color cf;
                if(cnk)
                {
                    cf = cnk->light[block_index2].quadrants[quadrant];
                }
                else
                    cf = default_color;

                cf = LAB_MulColor_Fast(cf, b->dia);
                if(!is_down || (cf&LAB_COL_MASK) != LAB_COL_MASK)
                    cf = LAB_LIGHT_FALL_OFF(cf);
                c = LAB_MaxColor(c, cf);
            }

            if(c_init || ctr_cnk->light[block_index].quadrants[quadrant] != c)
            {
                ctr_cnk->light[block_index].quadrants[quadrant] = c;
                relit_blocks |= LAB_CCPS_Pos(x&15, y&15, z&15);
            }
        }
    }
    return relit_blocks;
}


LAB_HOT
void LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27])
{
    LAB_Chunk* chunk = chunks[1+3+9];
    LAB_ASSERT(chunk);

    for(int i = 0; i < 27; ++i)
    {
        if(i != 1+3+9)
            LAB_ASSERT(!chunks[i] || chunks[i]->light_generated);
    }

    LAB_Color default_color_above = chunk->pos.y < -2 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);
    LAB_Color default_color = LAB_RGB(16, 16, 16); //chunk->pos.y < 0 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);

    LAB_CCPS relit_blocks = 0;



    /*if(chunk->empty)
    {
        chunk->relit_blocks = 0;

        if(chunk->light_generated) return;
        
        for(int j = 0; j < 16*16*16; ++j)
        {
            for(int i = 0; i < 8; ++i)
            {
                LAB_Color default_c = i & 2 ? default_color : default_color_above;
                chunk->light[j].quadrants[i] = default_c;
            }
        }

        chunk->sky_light = true;
        chunk->light_generated = true;
    }
    else*/
    {
        for(int i = 0; i < 8; ++i)
        {
            LAB_Color default_c = i & 2 ? default_color : default_color_above;
            relit_blocks |= LAB_TickLight_ProcessQuadrant(chunks, i, !chunk->light_generated, default_c);
        }

        // TODO insert check here
        chunk->sky_light = false;
    }
    
    chunk->relit_blocks |= relit_blocks;
    chunk->light_generated = true;
}