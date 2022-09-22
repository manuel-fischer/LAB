#include "LAB_world_light.h"
#include "LAB_debug.h"
#include "LAB_chunk_pseudo.h"
#include "LAB_chunk_neighborhood.h"
#include "LAB_blocks.h"
// TODO: use pseudo chunks instead of NULL chunks -> no branching required
//       - build chunk neighborhood with these pseudo chunks
// TODO: reduce range of searching for possibly changed block positions
//       - use crammed chunk pos set, stored in the chunks

#include <stdio.h> // DBG

// minimum light level for completely unlit block with that dia
//#define LAB_DARK_LIGHT(dia) ((((dia)>>4) & 0x0f0f0fu) | 0xff000000u)
//#define LAB_DARK_LIGHT(dia) (0)
//#define LAB_MIN_LIGHT(dia) ((((dia)>>6) & 0x030303u) | 0xff000000u)
//#define LAB_MIN_LIGHT(dia) (0)
//#define LAB_LIGHT_FALL_OFF(lum) ((lum) - ((lum)>>3 & 0x1f1f1f)-((lum)>>4 & 0x0f0f0f))
//#define LAB_LIGHT_FALL_OFF(lum) ((lum) - ((lum)>>3 & 0x1f1f1f))
//#define LAB_LIGHT_FALL_OFF(lum) ((lum) - ((((lum)&0xf8f8f8)+0x070707) >> 3 & 0xf8f8f8))
#define LAB_LIGHT_FALL_OFF(lum) ((lum) - (((lum)+(~((lum)>>5) & 0x070707)) >> 3 & 0x1f1f1f) - ((lum)>>4 & 0x0f0f0f))


// return blocks that changed
LAB_STATIC LAB_HOT
LAB_CCPS LAB_TickLight_ProcessQuadrant(
    LAB_Chunk_Blocks* blocks_ctr, LAB_LightNbHood_Mut* light_chunks,
    int quadrant, bool init, LAB_Color default_color, 
    int faces_changed, LAB_CCPS update_blocks)
{

    LAB_CCPS relit_blocks = 0;

    LAB_Chunk_Light* light_ctr = light_chunks->bufs[1+3+9];

    LAB_ASSERT(blocks_ctr);

    #define LAB_TickLight_ProcessBlock(getLightAt) do \
    { \
        int block_index = LAB_XADD3(x, y<<4, z<<8); \
        \
        LAB_Block* b = LAB_BlockP(blocks_ctr->blocks[block_index]); \
        LAB_Color c = 0; \
        LAB_UNROLL(3) \
        for(int i = 0; i < 3; ++i) \
        { \
            int xyz[3] = {x, y, z}; \
            xyz[i] -= dd[i]; \
            \
            bool is_down = i==1 && !(quadrant&2); \
            \
            LAB_Color cf = getLightAt(light_chunks, xyz[0], xyz[1], xyz[2], quadrant); \
            if(!is_down || (cf&LAB_COL_MASK) != LAB_COL_MASK) \
                cf = LAB_LIGHT_FALL_OFF(cf); \
            c = LAB_MaxColor(c, cf); \
        } \
        \
        c = LAB_MaxColor(LAB_MulColor_Fast(c, b->dia), b->lum); \
        c |= LAB_ALP_MASK; \
        \
        bool relit = c_init || light_ctr->light[block_index].quadrants[quadrant] != c; \
        \
        /*if(relit)*/ \
        { \
            light_ctr->light[block_index].quadrants[quadrant] = c; \
            relit_blocks  |= LAB_CCPS_Pos(x, y, z)*relit; \
            update_blocks |= LAB_CCPS_Pos(x, y, z)*relit; \
        } \
    } while(0)



    int dd[3];
    int dx, dy, dz; // delta index = sign
    int x0, y0, z0; // start index
    int xm, ym, zm; // scan kernel
    
    if(quadrant & 1) { dd[0] = dx =  1; x0 =  0; xm = 3; } 
    else             { dd[0] = dx = -1; x0 = 15; xm = 6; }
    if(quadrant & 2) { dd[1] = dy =  1; y0 =  0; ym = 3; } 
    else             { dd[1] = dy = -1; y0 = 15; ym = 6; }
    if(quadrant & 4) { dd[2] = dz =  1; z0 =  0; zm = 3; } 
    else             { dd[2] = dz = -1; z0 = 15; zm = 6; }



    int axis_changed = (!!(faces_changed & ( (1|2)  ^  1<<!!(quadrant&1))))
                     | (!!(faces_changed & ( (4|8)  ^  4<<!!(quadrant&2)))) << 1
                     | (!!(faces_changed & ((16|32) ^ 16<<!!(quadrant&4)))) << 2;


    LAB_PULL_CONST(int, c_init, 2, init)
    {

        int zi, yi, xi;
        int x, y, z;

        // Hide uninitialized warning message
        zi=yi=xi = x=y=z = 0;

        // if-condition: check if the previous or current column has changes
        #define x_loop(start, all_if) \
            for(xi = (start), x = x0 + (start)*dx; xi < 16; ++xi, x += dx) \
            if(c_init || (all_if) || update_blocks <<  1 & (xm << x) & 0x1fffe)
            
        #define y_loop(start, all_if) \
            for(yi = (start), y = y0 + (start)*dy; yi < 16; ++yi, y += dy) \
            if(c_init || (all_if) || update_blocks >> 15 & (ym << y) & 0x1fffe)
            
        #define z_loop(start, all_if) \
            for(zi = (start), z = z0 + (start)*dz; zi < 16; ++zi, z += dz) \
            if(c_init || (all_if) || update_blocks >> 31 & (zm << z) & 0x1fffe)


        {
            int cur_loop = 0;



            #define LOOP_YIELD(i) do \
            { \
                cur_loop = (i); \
                goto process_block; \
                case (i):; \
            } while(0)

            if(axis_changed || LAB_CCPS_HasPos(update_blocks, x0, y0, z0))
            {
                x = x0, y = y0; z = z0;
        process_block:;
                #define LAB_GETLIGHTAT(light_chunks, x, y, z, quadrant) \
                    (LAB_LightNbHood_RefLightNode(light_chunks, x, y, z)->quadrants[quadrant])

                LAB_TickLight_ProcessBlock(LAB_GETLIGHTAT);
                #undef LAB_GETLIGHTAT
            }

            switch(cur_loop)
            {
                case 0:
                

                // x-axis:
                z = z0; y = y0;
                x_loop(1, axis_changed & (4|2))
                    LOOP_YIELD(1);

                // y-axis:
                z = z0; x = x0;
                y_loop(1, axis_changed & (4|1))
                    LOOP_YIELD(2);

                // z-axis:
                y = y0; x = x0;
                z_loop(1, axis_changed & (2|1))
                    LOOP_YIELD(3);

                        
                // x-plane:
                x = x0;
                z_loop(1, axis_changed & (1))
                y_loop(1, axis_changed & (1))
                    LOOP_YIELD(4);
  
                // y-plane
                y = y0;
                z_loop(1, axis_changed & (2))
                x_loop(1, axis_changed & (2))
                    LOOP_YIELD(5);

                // z-plane
                z = z0;
                y_loop(1, axis_changed & (4))
                x_loop(1, axis_changed & (4))
                    LOOP_YIELD(6);
            }
        }



        z_loop(1, false)
        y_loop(1, false)
        x_loop(1, false)
        {
            #define LAB_GETLIGHTAT(light_chunks, x, y, z, quadrant) \
                (light_ctr->light[LAB_XADD3((x), (y)<<4, (z)<<8)].quadrants[quadrant])

            LAB_TickLight_ProcessBlock(LAB_GETLIGHTAT);
            #undef LAB_GETLIGHTAT
        }
    }
    return relit_blocks;
}


LAB_HOT
LAB_CCPS LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27],
                       int neighbors_changed, LAB_CCPS blocks_changed)
{
    LAB_Chunk* chunk = chunks[1+3+9];
    LAB_ASSERT(chunk);

    for(int i = 0; i < 27; ++i)
    {
        if(i != 1+3+9)
            LAB_ASSERT(!chunks[i] || chunks[i]->light_generated);
    }

    LAB_Color default_color_above = chunk->pos.y < 0 ? LAB_RGB(0, 0, 0) : LAB_RGB(255, 255, 255);
    LAB_Color default_color = LAB_RGB(0, 0, 0); //chunk->pos.y < 0 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);

    LAB_CCPS relit_blocks = 0;


    uint8_t face2quads[6] =
    {
        (1<<1)|(1<<3)|(1<<5)|(1<<7),
        (1<<0)|(1<<2)|(1<<4)|(1<<6),

        (1<<2)|(1<<3)|(1<<6)|(1<<7),
        (1<<0)|(1<<1)|(1<<4)|(1<<5),

        (1<<4)|(1<<5)|(1<<6)|(1<<7),
        (1<<0)|(1<<1)|(1<<2)|(1<<3),
    };

    /*uint64_t face2ccps[6] =
    {
        0xffffffffffff0001,
        0xffffffffffff8000,
        0xffffffff0001ffff,
        0xffffffff8000ffff,
        0xffff0001ffffffff,
        0xffff8000ffffffff,
    };*/

    //LAB_CCPS boundary_blocks = 0;
    int faces_changed = 0;

    int relit_quads = 0;
    int face;
    LAB_DIR_EACH(63, face,
    {
        int i = LAB_OX(face)+1 + (LAB_OY(face)+1)*3 + (LAB_OZ(face)+1)*9;
        if(neighbors_changed & 1<<i)
        {
            faces_changed |= 1<<face;
            relit_quads |= face2quads[face];
            //boundary_blocks |= face2ccps[face];
        }
    });

    if(!chunk->light_generated || blocks_changed)
        relit_quads = 255;

    /*if(!chunk->light_generated)
        blocks_changed = 0xffffffffffffffff;*/
    //blocks_changed |= boundary_blocks;

    //blocks_changed = 0xffffffffffffffff;
    /*if(chunk->empty)
    {
        chunk->relit_blocks = 0;

        if(chunk->light_generated) return 0;
        
        LAB_Color quads[8];
        for(int i = 0; i < 8; ++i)
        {
            quads[i] = i & 2 ? default_color : default_color_above;
        }

        for(int j = 0; j < 16*16*16; ++j)
        {
            //LAB_UNROLL(8)
            for(int i = 0; i < 8; ++i)
            {
                //LAB_Color default_c = i & 2 ? default_color : default_color_above;
                chunk->light[j].quadrants[i] = quads[i]; //default_c;
            }
        }

        chunk->sky_light = true;
        chunk->light_generated = true;
    }
    else*/
    {
        LAB_Chunk_Blocks* ctr_blocks = LAB_Chunk_Blocks_Read(chunk);
        if(!ctr_blocks) return 0; // TODO: correct error handling

        LAB_LightNbHood_Mut light_chunks;
        if(!LAB_LightNbHood_GetWrite(chunks, &light_chunks)) return 0; // TODO: correct error handling
        

        for(int i = 0; i < 8; ++i, relit_quads>>=1)
        {
            if(relit_quads&1)
            {
                LAB_Color default_c = i & 2 ? default_color : default_color_above;
                relit_blocks |= LAB_TickLight_ProcessQuadrant(ctr_blocks, &light_chunks, i, !chunk->light_generated, default_c, faces_changed, blocks_changed);
            }
        }

        LAB_Chunk_Light_Optimize(chunk);
    }
    
    chunk->light_generated = true;
    return relit_blocks;
}